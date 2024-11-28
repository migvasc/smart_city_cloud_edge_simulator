
#include "SchedulingLCAHEFT.hpp"
#include "Util.hpp"

SchedulingLCAHEFT::SchedulingLCAHEFT(map<string, int> *hosts_cpu_availability_, ElectricityCO2eq* local_grid_power_co2_, ElectricityCO2eq* cloud_dc_power_co2_, double pv_panel_power_co2_, double battery_power_co2_, simgrid::s4u::Host* cloud_cluster_,std::map<std::string, double> *hosts_renewable_energy_,std::map<std::string, LithiumIonBattery*> *hosts_batteries_, std::map<std::string, double> *hosts_energy_consumption_)
{
    hosts_cpuavailability =hosts_cpu_availability_;
    local_grid_power_co2 =local_grid_power_co2_;
    cloud_dc_power_co2 = cloud_dc_power_co2_;
    pv_panel_power_co2 = pv_panel_power_co2_;
    battery_power_co2 = battery_power_co2_;
    cloud_cluster =cloud_cluster_;
    hosts_renewable_energy = hosts_renewable_energy_;
    hosts_batteries = hosts_batteries_;
    hosts_energy_consumption = hosts_energy_consumption_;

}

simgrid::s4u::Host* SchedulingLCAHEFT::find_host(shared_ptr<SegmentTask> ready_task)
{
    sg_host_energy_update_all();

    simgrid::s4u::Host* selected_host = nullptr;
    double min_co2 = 999999999999.0;
    double host_co2;
    for (auto candidate_host : simgrid::s4u::Engine::get_instance()->get_all_hosts())
    {
        if((*hosts_cpuavailability)[candidate_host->get_name()] > 0)
        {
            host_co2 = get_host_co2_area(candidate_host,ready_task);
            if (host_co2 < min_co2)
            {
                selected_host = candidate_host;
                min_co2 = host_co2;
            }
        }
    }        
    return selected_host;
}


double SchedulingLCAHEFT::get_host_co2_area(simgrid::s4u::Host* host,shared_ptr<SegmentTask> ready_task)
{
    // Variables to store co2 info
    double grid_co2 = 0;
    double battery_co2 = 0;
    double solar_co2 = 0;

    double brown_energy_wh    = 0.0; 
    double renewable_energy_used = 0;
    double grid_energy_used = 0;
    double energy_discharged  = 0;

    double host_consumed_energy =  sg_host_get_consumed_energy(host) -  (*hosts_energy_consumption)[host->get_name()]  ;

    double renewable_power_timeslot = 0; 
    double available_renewable_energy = 0;
    double available_battery_energy = 0;

    // We also need to remove the power consumed by the host, to update the available renewable energy info
    double default_task_flops = 1350000000.0;
    double max_power  = sg_host_get_wattmax_at(host,host->get_pstate());
    double idle_power = sg_host_get_idle_consumption(host);
    double power_per_core = (max_power - idle_power)/host->get_core_count();

    double run_time = default_task_flops/host->get_speed();
    int cores_used =  host->get_core_count() - (*hosts_cpuavailability)[host->get_name()] ;
    double dynamic_energy = cores_used * power_per_core;
    double task_energy_consumption = 0;

    double comm_time = 0;
    const std::unordered_map<std::string, std::string> * host_properties = host-> get_properties();
    bool has_renewable_infra = false;
    cores_used += 1 ; // to represent that we will allocate a task to this host

    // First we validate how much renewable we have left
    if ( hosts_renewable_energy->find(host->get_name()) !=hosts_renewable_energy->end())
    {
        renewable_power_timeslot = Util::convert_joules_to_wh((*hosts_renewable_energy)[host->get_name()]);
        has_renewable_infra = true;
    }
   
    
    host_consumed_energy = Util::convert_joules_to_wh(host_consumed_energy);
    task_energy_consumption = (idle_power + dynamic_energy)*run_time;


    if (has_renewable_infra)
    {
        //First we validate how much renewable energy we have left to execute this task
        available_renewable_energy = renewable_power_timeslot - host_consumed_energy;

        //Then we get how much energy we have from the batteries
        available_battery_energy = (*hosts_batteries)[host->get_name()]->getUsableWattsHour();

        // If we do not have enough renewable power, we may need to use energy from the batteries
        if (available_renewable_energy < 0)
        {

            available_battery_energy = max( available_battery_energy+ available_renewable_energy,0.0);
            available_renewable_energy=0;
        }


        // First we validate if the energy from the solar panels can be used for the task
        if (available_renewable_energy < task_energy_consumption)
        {

            renewable_energy_used = available_renewable_energy; 
            brown_energy_wh =  task_energy_consumption - available_renewable_energy;
            // Then we check if the energy from the batteries can be used for executing the task
            if ( available_battery_energy >= brown_energy_wh)
            {
                energy_discharged = brown_energy_wh;
                brown_energy_wh = 0;
            }
            else
            {
                energy_discharged = available_battery_energy;
                brown_energy_wh-=energy_discharged;
            }

        }
        else
        {
            renewable_energy_used = task_energy_consumption;
            brown_energy_wh = 0;
            energy_discharged = 0;
        }


    }
    else
    {
        renewable_energy_used = 0;
        brown_energy_wh = task_energy_consumption;
        energy_discharged = 0;

    }

    grid_energy_used = brown_energy_wh;



    if(host_properties->find("host_type")!=host_properties->end())
    {       
        std::string host_type = host->get_property("host_type");
        if (host_type.compare("cloud_host")==0)
        {
            grid_co2 = grid_energy_used * cloud_dc_power_co2->get_current_co2_eq(simgrid::s4u::Engine::get_clock()) *1/1000; 
        }
        else
        {
            grid_co2 = grid_energy_used * local_grid_power_co2->get_current_co2_eq(simgrid::s4u::Engine::get_clock())*1/1000;
        }   

    }
    
    battery_co2 = energy_discharged * battery_power_co2*1/1000;
    
    // Solar energy will be always the same, since we do not change the pv area    
    // therefore we have a fixed co2 emissions for the simulated day
    // The solar panels will produce the same amount of electricity no matter the algorithm    
    solar_co2   = 0; // renewable_energy_used * pv_panel_power_co2*1/1000;


    if(host_properties->find("lat")!=host_properties->end())
    {       
        double cand_host_lat =  std::stod(host->get_property("lat"));
        double cand_host_long = std::stod(host->get_property("long"));
        double cand_host_z_coord  = std::stod(host->get_property("z_coord"));
                        
        double best_host_lat = std::stod(ready_task->get_pref_host()->get_property("lat"));
        double best_host_long = std::stod(ready_task->get_pref_host() ->get_property("long"));
        double best_host_z_coord = std::stod(ready_task->get_pref_host()->get_property("z_coord"));

        double latency = Util::getNetworkLatencyVivaldi(best_host_long,best_host_lat,best_host_z_coord,cand_host_long,cand_host_lat,cand_host_z_coord);
        comm_time+= 2*latency;

    }

    return (grid_co2 + solar_co2 + battery_co2)* (run_time + comm_time);

}
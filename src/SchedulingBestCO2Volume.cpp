
#include "SchedulingBestCO2Volume.hpp"
#include "Util.hpp"

SchedulingBestCO2Volume::SchedulingBestCO2Volume(map<string, int> *hosts_cpu_availability_, ElectricityCO2eq* local_grid_power_co2_, ElectricityCO2eq* cloud_dc_power_co2_, double pv_panel_power_co2_, double battery_power_co2_, simgrid::s4u::Host* cloud_cluster_,std::map<std::string, double> *hosts_renewable_energy_,std::map<std::string, LithiumIonBattery*> *hosts_batteries_, std::map<std::string, double> *hosts_energy_consumption_)
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

simgrid::s4u::Host* SchedulingBestCO2Volume::find_host(shared_ptr<SegmentTask> ready_task)
{
    sg_host_energy_update_all();

    simgrid::s4u::Host* selected_host = nullptr;
    double min_co2 = 999999999999.0;
    double host_co2;
    for (auto candidate_host : simgrid::s4u::Engine::get_instance()->get_all_hosts())
    {
        if((*hosts_cpuavailability)[candidate_host->get_name()] > 0)
        {
            host_co2 = get_host_expected_volume(candidate_host,ready_task);
            if (host_co2 < min_co2)
            {
                selected_host = candidate_host;
                min_co2 = host_co2;
            }
        }
    }        
    return selected_host;
}


double SchedulingBestCO2Volume::get_host_expected_volume(simgrid::s4u::Host* host,shared_ptr<SegmentTask> ready_task)
{
    // Variables to store co2 info
    double grid_co2 = 0;
    double battery_co2 = 0;
    double solar_co2 = 0;
    double brown_energy_wh    = 0.0; 
    double energy_discharged  = 0;
    double default_task_flops = 1350000000.0;
    double renewable_energy_used = 0;
    double grid_energy_used = 0;
    double host_consumed_energy =  sg_host_get_consumed_energy(host) -  (*hosts_energy_consumption)[host->get_name()]  ;

    // First the energy from the solar panels
    double available_renewable_power = 0; 
    
    if ( hosts_renewable_energy->find(host->get_name()) !=hosts_renewable_energy->end())
    {
        available_renewable_power = Util::convert_joules_to_wh((*hosts_renewable_energy)[host->get_name()]);
    }

    // We also need to remove the power consumed by the host, to update the available renewable energy info
    double max_power  = sg_host_get_wattmax_at(host,host->get_pstate());
    double idle_power = sg_host_get_idle_consumption(host);
    double power_per_core = (max_power - idle_power)/host->get_core_count();

    double run_time = default_task_flops/host->get_speed();
    int cores_used =  host->get_core_count() - (*hosts_cpuavailability)[host->get_name()] ;

    cores_used += 1 ; // to represent that we will allocate a task to this host
    double dynamic_energy = cores_used * power_per_core;

    double comm_time = 0;
    const std::unordered_map<std::string, std::string> * host_properties = host-> get_properties();

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

    host_consumed_energy +=  (idle_power +    dynamic_energy ) * run_time;
    host_consumed_energy = Util::convert_joules_to_wh(host_consumed_energy);
    
    // If we have less renewable than what the host is consuming, we will use energy from the grid
    // so we first validate if we have energy in the batteries
    if (available_renewable_power < host_consumed_energy)
    {
        brown_energy_wh = host_consumed_energy - available_renewable_power;

        renewable_energy_used = available_renewable_power;
        if ( hosts_batteries->find(host->get_name()) != hosts_batteries->end() )
        {
            if ((*hosts_batteries)[host->get_name()]->getUsableWattsHour()>=brown_energy_wh)
            {
                energy_discharged = (*hosts_batteries)[host->get_name()]->discharge(brown_energy_wh);
                brown_energy_wh =0;
            }
            // Otherwise, we only discharge what was possible
            else
            {
                energy_discharged = (*hosts_batteries)[host->get_name()]->discharge( (*hosts_batteries)[host->get_name()] ->getUsableWattsHour() );
                brown_energy_wh-=energy_discharged;
            }
        }
        
    }
    else
    {
        renewable_energy_used = host_consumed_energy;
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
    

    battery_co2 = energy_discharged * battery_power_co2;
    solar_co2   = renewable_energy_used * pv_panel_power_co2;

    return (grid_co2 + solar_co2 + battery_co2)* (run_time + comm_time);

}
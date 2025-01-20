
#include "SchedulingLCAHEFT.hpp"
#include "Util.hpp"

SchedulingLCAHEFT::SchedulingLCAHEFT(map<string, int> *hosts_cpu_availability_, ElectricityCO2eq* local_grid_power_co2_, ElectricityCO2eq* cloud_dc_power_co2_, double pv_panel_power_co2_, double battery_power_co2_,std::map<std::string, double> *hosts_renewable_energy_,std::map<std::string, LithiumIonBattery*> *hosts_batteries_, std::map<std::string, double> *hosts_energy_consumption_,std::unordered_map<std::string, double> *lat_cache)
{
    hosts_cpuavailability =hosts_cpu_availability_;
    local_grid_power_co2 =local_grid_power_co2_;
    cloud_dc_power_co2 = cloud_dc_power_co2_;
    pv_panel_power_co2 = pv_panel_power_co2_;
    battery_power_co2 = battery_power_co2_;
    hosts_renewable_energy = hosts_renewable_energy_;
    hosts_batteries = hosts_batteries_;
    hosts_energy_consumption = hosts_energy_consumption_;
    latency_cache = lat_cache;
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

double SchedulingLCAHEFT::calculate_co2(shared_ptr<SegmentTask> ready_task,simgrid::s4u::Host * host)
{
    double grid_co2 = 0;
    double battery_co2 = 0;
    double solar_co2 = 0;
    double grid_energy_wh    = 0.0; 
    double renewable_energy_used = 0;
    double energy_discharged  = 0;
    double host_consumed_energy =  sg_host_get_consumed_energy(host) -  (*hosts_energy_consumption)[host->get_name()]  ;
    double renewable_power_timeslot = 0; 
    double available_renewable_energy = 0;
    double available_battery_energy = 0;

    // We also need to remove the power consumed by the host, to update the available renewable energy info    
    const std::unordered_map<std::string, std::string> * host_properties = host-> get_properties();
    bool is_cloud_host = false;

    if (host_properties->find("host_type")!=host_properties->end())
    {               
        std::string host_type = host->get_property("host_type");
        if (host_type.compare("cloud_host")==0)
        {
            is_cloud_host = true;
        }
    }

    int pstate_on = 1;

    double max_power  = sg_host_get_wattmax_at(host,host->get_pstate());
    double idle_power = sg_host_get_idle_consumption(host);
    double host_speed = host->get_speed();
    
    if (is_cloud_host)
    {        
        max_power  = sg_host_get_wattmax_at(host,pstate_on);
        idle_power = sg_host_get_idle_consumption_at(host,pstate_on);
        host_speed = host->get_pstate_speed(pstate_on);
    }

    double run_time = ready_task->get_exec()->get_remaining()/host_speed;
    double power_per_core = (max_power - idle_power)/host->get_core_count();

    int cores_used =  host->get_core_count() - (*hosts_cpuavailability)[host->get_name()];
    cores_used += 1 ; // to represent that we will allocate a task to this host

    double dynamic_energy = cores_used * power_per_core;
    double task_energy_consumption = 0;

    double comm_time = 0;

    bool has_renewable_infra = false;

    // First we validate how much renewable we have left
    if ( hosts_renewable_energy->find(host->get_name()) !=hosts_renewable_energy->end())
    {
        renewable_power_timeslot = Util::convert_joules_to_wh((*hosts_renewable_energy)[host->get_name()]);
        has_renewable_infra = true;                                
    }
   
    host_consumed_energy = Util::convert_joules_to_wh(host_consumed_energy);
    task_energy_consumption = (idle_power + dynamic_energy) * run_time;

    if (has_renewable_infra)
    {
        //First we validate how much renewable energy we have left to execute this task
        available_renewable_energy = renewable_power_timeslot - host_consumed_energy;

        //Then we get how much energy we have from the batteries
        available_battery_energy = (*hosts_batteries)[host->get_name()]->getUsableWattsHour();

        // If we do not have enough renewable power, we may need to use energy from the batteries
        if (available_renewable_energy < 0)
        {
            // Here we allow the negative value to simulate the usage of battery energy to supply the host inside the window
            available_battery_energy = max( available_battery_energy+ available_renewable_energy,0.0);
            available_renewable_energy = 0;
        }

        // First we validate if the energy from the solar panels can be used for the task
        if (available_renewable_energy < task_energy_consumption)
        {
            renewable_energy_used = available_renewable_energy; 
            grid_energy_wh =  task_energy_consumption - available_renewable_energy;
            // Then we check if the energy from the batteries can be used for executing the task
            if ( available_battery_energy >= grid_energy_wh)
            {
                energy_discharged = grid_energy_wh;
                grid_energy_wh = 0;
            }
            else
            {
                energy_discharged = available_battery_energy;
                grid_energy_wh -= energy_discharged;
            }            
        }
        else
        {

            renewable_energy_used = task_energy_consumption;
            grid_energy_wh = 0;
            energy_discharged = 0;
        }        
    }
    else
    {        
        renewable_energy_used = 0;
        grid_energy_wh = task_energy_consumption;
        energy_discharged = 0;
    }


    if (is_cloud_host)
    {
        grid_co2 = grid_energy_wh * cloud_dc_power_co2->get_current_co2_eq(simgrid::s4u::Engine::get_clock()) *1/1000; 
    }        
    else
    {
        grid_co2 = grid_energy_wh * local_grid_power_co2->get_current_co2_eq(simgrid::s4u::Engine::get_clock())*1/1000;
    }
    
    battery_co2 = energy_discharged * battery_power_co2*1/1000;
    
    // Solar energy will be always the same, since we do not change the pv area    
    // therefore we have a fixed co2 emissions for the simulated day
    // The solar panels will produce the same amount of electricity no matter the algorithm        
    solar_co2   = 0; // renewable_energy_used * pv_panel_power_co2*1/1000;


    return solar_co2 + grid_co2 + battery_co2;

}

double SchedulingLCAHEFT::calculate_response_time(shared_ptr<SegmentTask> ready_task,simgrid::s4u::Host * host)
{

    double host_speed = host->get_speed();
    bool is_cloud_host = false;
    const std::unordered_map<std::string, std::string> * host_properties = host-> get_properties();        
    
    if (host_properties->find("host_type")!=host_properties->end())
    {                           
        std::string host_type = host->get_property("host_type");
        if (host_type.compare("cloud_host")==0)
        {
            is_cloud_host = true;
        }                        
    }
    
    if(is_cloud_host)
    {
        host_speed = host->get_pstate_speed(1);
    }
        
    double run_time = ready_task->get_exec()->get_remaining()/host_speed;
    double comm_time = 0;

    //First, the communication from the parents
    double max_parent_comms =0;
    for(auto& parent :ready_task->get_parents() )
    {
        simgrid::s4u::Host* src_host = nullptr;;
        if (parent.second->get_exec()->is_assigned()) 
        {
            src_host = parent.second->get_exec()->get_host();
        }
       
        
        std::string key = src_host->get_name() + "-" + host->get_name();

        double parent_latency = 0.0;    

        if (latency_cache->find(key)!=latency_cache->end())
        {
            parent_latency = (*latency_cache)[key];
        }
        else
        {
            const std::unordered_map<std::string, std::string> * host_properties = src_host-> get_properties();
            if(host_properties->find("lat")!=host_properties->end())
            {       
                double parent_lat =  std::stod(src_host->get_property("lat"));
                double parent_long = std::stod(src_host->get_property("long"));
                double parent_z_coord = std::stod(src_host->get_property("z_coord"));
                                
                double cand_host_lat = std::stod(host->get_property("lat"));
                double cand_host_long = std::stod(host ->get_property("long"));
                double cand_host_z_coord = std::stod(host->get_property("z_coord"));

                parent_latency = Util::getNetworkLatencyVivaldi(parent_long,parent_lat,parent_z_coord,cand_host_long,cand_host_lat,cand_host_z_coord);
            }
            (*latency_cache)[key] = parent_latency;
        }
                
        if (parent_latency > max_parent_comms)
        {
            max_parent_comms = parent_latency;
        }                

    }
    // Since the communications occurs in parallel, it will use the value from the parent that has
    // the longest communication delay
    comm_time += max_parent_comms;

    // Now, we validate if the host does not has the local data
    if(host != ready_task->get_pref_host())
    {

        std::string key = ready_task->get_pref_host()->get_name() + "-" + host->get_name();                        
        
        if (latency_cache->find(key)!=latency_cache->end())
        {
            comm_time+= 2* (*latency_cache)[key];

        }
        else
        {                                
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
                (*latency_cache)[key]=latency;
                comm_time+= 2*latency;
            }
        }            
    }

    return run_time + comm_time;
}


double SchedulingLCAHEFT::get_host_co2_area(simgrid::s4u::Host* host,shared_ptr<SegmentTask> ready_task)
{
    // Variables to store co2 info
    double estimated_co2 = calculate_co2(ready_task,host);
    double estimated_response_time = calculate_response_time(ready_task,host);
    return estimated_co2 * estimated_response_time;
}
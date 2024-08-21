#include "SchedulingBestFit.hpp"

simgrid::s4u::Host* SchedulingBestFit::find_host(shared_ptr<SegmentTask> ready_task)
{
    int free_cores = 1;
    simgrid::s4u::Host* selected_host = nullptr;
    int max_cores = 4; //initial value based on the Raspberry pi
    while(free_cores < max_cores)
    {
        for (auto& host : simgrid::s4u::Engine::get_instance()->get_all_hosts())
        {
            if((*hosts_cpuavailability)[host->get_name()] == free_cores) 
            {
                selected_host = host;
                return selected_host;
            } 
            if (host->get_core_count()>max_cores){
                max_cores= host->get_core_count();
            }
        }
        free_cores++;
    }
    return selected_host;
}

SchedulingBestFit::SchedulingBestFit(map<string, int> *hosts_cpu_availability){
    this->hosts_cpuavailability = hosts_cpu_availability;
}
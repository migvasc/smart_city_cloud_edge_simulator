#include "SchedulingBaseline.hpp"


simgrid::s4u::Host* SchedulingBaseline::find_host(shared_ptr<SegmentTask> ready_task)
{
    simgrid::s4u::Host* host =   ready_task->get_pref_host();
    
    
    if(  (*hosts_cpuavailability)[host->get_name()]>0)
    {
        return host;
    }
    return nullptr;
}

SchedulingBaseline::SchedulingBaseline(map<string, int> *hosts_cpu_availability){
    this->hosts_cpuavailability = hosts_cpu_availability;
}
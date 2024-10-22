#include "SchedulingHostType.hpp"


simgrid::s4u::Host* SchedulingHostType::find_host(shared_ptr<SegmentTask> ready_task)
{
    for (auto * host : selected_host_type_array)
    {
        if(  (*hosts_cpuavailability)[host->get_name()]>0)
        {
            return host;
        }

    }
    return nullptr;
}

SchedulingHostType::SchedulingHostType(map<string, int> *hosts_cpu_availability_, std::vector<simgrid::s4u::Host*> selected_host_type_array_){
    this->hosts_cpuavailability = hosts_cpu_availability_;
    this->selected_host_type_array = selected_host_type_array_;
}
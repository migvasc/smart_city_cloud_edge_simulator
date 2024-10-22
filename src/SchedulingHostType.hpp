
#ifndef SCHEDULINGHOSTTYPE_HPP
#define SCHEDULINGHOSTTYPE_HPP

#include "SchedulingStrategy.hpp"

class SchedulingHostType: public SchedulingStrategy {

private:

// Map used to store the CPU cores availability of the hosts being used
map<string, int> *hosts_cpuavailability;
std::vector<simgrid::s4u::Host*> selected_host_type_array;

public:
    SchedulingHostType(map<string, int> *hosts_cpu_availability, std::vector<simgrid::s4u::Host*> selected_host_type_array);
    simgrid::s4u::Host* find_host(shared_ptr<SegmentTask> ready_task);
};
#endif
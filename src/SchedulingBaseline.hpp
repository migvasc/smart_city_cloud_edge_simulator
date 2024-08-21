
#ifndef SCHEDULINGBASELINE_HPP
#define SCHEDULINGBASELINE_HPP

#include "SchedulingStrategy.hpp"

class SchedulingBaseline : public SchedulingStrategy {

private:

// Map used to store the CPU cores availability of the hosts being used
map<string, int> *hosts_cpuavailability;


public:
    SchedulingBaseline(map<string, int> *hosts_cpu_availability);
    simgrid::s4u::Host* find_host(shared_ptr<SegmentTask> ready_task);

};
#endif
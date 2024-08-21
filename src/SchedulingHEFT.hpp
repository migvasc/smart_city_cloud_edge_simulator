
#ifndef SCHEDULINGHEFT_HPP
#define SCHEDULINGHEFT_HPP

#include "SchedulingStrategy.hpp"
#include <stdlib.h>

class SchedulingHEFT : public SchedulingStrategy {

private:

// Map used to store the CPU cores availability of the hosts being used
map<string, int> *hosts_cpuavailability;

double getNetworkLatencyVivaldi(double x1, double y1, double z1, double x2, double y2, double z2);
// Used to store where the cache is (which host)
std::unordered_map<std::string, string> *task_cache;

public:

    SchedulingHEFT(map<string, int> *hosts_cpu_availability,std::unordered_map<std::string, string> * cache);
    simgrid::s4u::Host* find_host(shared_ptr<SegmentTask> ready_task);
    simgrid::s4u::Host* find_host(shared_ptr<SegmentTask> ready_task,std::vector<simgrid::s4u::Host *> hosts);

};
#endif
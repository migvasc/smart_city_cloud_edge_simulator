
#ifndef SCHEDULINGHEFT_HPP
#define SCHEDULINGHEFT_HPP

#include "SchedulingStrategy.hpp"
#include <stdlib.h>

class SchedulingHEFT : public SchedulingStrategy {

private:

// Map used to store the CPU cores availability of the hosts being used
map<string, int> *hosts_cpuavailability;

// Used to store where the cache is (which host)
std::unordered_map<std::string, string> *task_cache;

std::unordered_map<std::string, double> *latency_cache;

public:

    SchedulingHEFT(map<string, int> *hosts_cpu_availability,std::unordered_map<std::string, string> * cache,std::unordered_map<std::string, double> * lat_cache);
    simgrid::s4u::Host* find_host(shared_ptr<SegmentTask> ready_task);
    simgrid::s4u::Host* find_host(shared_ptr<SegmentTask> ready_task,std::vector<simgrid::s4u::Host *> hosts);
    void create_tasks_ranking(std::vector<std::shared_ptr<SegmentTask>> & tasks); 
    void create_task_ranking_recursive(simgrid::s4u::ActivityPtr task,std::map<std::string,int>& rank);
};
#endif
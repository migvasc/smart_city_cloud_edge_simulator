#ifndef EDGEHOST_HPP
#define EDGEHOST_HPP

#include <string>
#include <unordered_map>
#include "robin_hood.h"
#include "simgrid/s4u.hpp"
#include "Message.hpp"
#include "SegmentTask.hpp"
using namespace std; 


class EdgeHost{
  
private:

std::string host_name;

int host_mem_limit;

// Map structure to represent the cache. The key is the street name
// the value is the timeslot when the task was registrered
std::map<std::string, int> cache;

// Map structure to represent the cache. The key is the street name
// the value is the last instant of time the cache was used.
std::map<std::string, double> cache_usage;


public:
    EdgeHost(std::string host_name, int mem_limit);
    void operator()();
    simgrid::s4u::Mailbox *mailbox;
    void add_task_in_cache(const std::string & task_name, int timeslot);
    void delete_task_from_cache(const std::string & task_name);
    void expire_tasks(int timeslot);
    bool has_free_memory_in_cache();
    bool validate_task_expiration(std::string task_name, int timeslot);

};
#endif
#ifndef EDGEHOST_HPP
#define EDGEHOST_HPP

#include <string>
#include <unordered_map>
#include "robin_hood.h"
#include "simgrid/s4u.hpp"

using namespace std; 

class EdgeHost{
    public:
    // Map structure to represent the cache. The key is the street name
    // the value is the Time to To Live (to compute when to expire).
    std::map<std::string, double> cache;
    
    // Map structure to represent the cache. The key is the street name
    // the value is the last instant of time the cache was used.
    std::map<std::string, double> cache_usage;
    
    // Removes the items that have expired
    void expire_cache(double time);


};
#endif

#ifndef SCHEDULINGGEFT_HPP
#define SCHEDULINGGEFT_HPP

#include "SchedulingHEFT.hpp"
#include <stdlib.h>
#include <simgrid/s4u.hpp>
#include "LithiumIonBattery.hpp"
#include "simgrid/plugins/energy.h"

class SchedulingGEFT : public SchedulingHEFT {

private:

// Map used to store the CPU cores availability of the hosts being used
map<string, int> *hosts_cpuavailability;
// Temporary map to include batteries of hosts
std::map<std::string, LithiumIonBattery*> *hosts_batteries;
// Temporary map to include solar panels of hosts
std::map<std::string, double> *hosts_energy_consumption;
// Envelope with available renewable energy for 5 min 
std::map<std::string, double> *hosts_renewable_energy;
std::unordered_map<std::string, string> *task_cache;
std::unordered_map<std::string, double> *latency_cache;
double getNetworkLatencyVivaldi(double x1, double y1, double z1, double x2, double y2, double z2);
double get_host_available_renewable_energy(simgrid::s4u::Host* host);

public:
    SchedulingGEFT(map<string, int> *hosts_cpu_availability,std::map<std::string, double> *hosts_renewables, std::map<std::string, double> *hosts_energy,std::map<std::string, LithiumIonBattery*> *hosts_bat,std::unordered_map<std::string, string> *task_cache, std::unordered_map<std::string, double> *lat_cache);
    simgrid::s4u::Host* find_host(shared_ptr<SegmentTask> ready_task);

};
#endif
#ifndef SCHEDULINGLCAHEFT_HPP
#define SCHEDULINGLCAHEFT_HPP

#include "SchedulingStrategy.hpp"
#include <stdlib.h>
#include "ElectricityCO2eq.hpp"
#include <simgrid/s4u.hpp>
#include "LithiumIonBattery.hpp"
#include "simgrid/plugins/energy.h"
#include <stdlib.h>

class SchedulingLCAHEFT : public SchedulingStrategy {

private:

// Map used to store the CPU cores availability of the hosts being used
map<string, int> *hosts_cpuavailability;
ElectricityCO2eq* local_grid_power_co2;
ElectricityCO2eq* cloud_dc_power_co2;
double pv_panel_power_co2; 
double battery_power_co2; 
std::vector<simgrid::s4u::Host*> sorted_hosts;

std::map<std::string, double> *hosts_renewable_energy;
std::map<std::string, LithiumIonBattery*> *hosts_batteries;
// Temporary map to include solar panels of hosts
std::map<std::string, double> *hosts_energy_consumption;
std::map<std::string, double> *hosts_energy_consumption_check_point;

std::unordered_map<std::string, double> *latency_cache;


public:
    SchedulingLCAHEFT(map<string, int> *hosts_cpu_availability_, ElectricityCO2eq* local_grid_power_co2_, ElectricityCO2eq* cloud_dc_power_co2_, double pv_panel_power_co2_, double battery_power_co2_,std::map<std::string, double> *hosts_renewable_energy_,std::map<std::string, LithiumIonBattery*> *hosts_batteries_, std::map<std::string, double> *hosts_energy_consumption_,std::map<std::string, double> *hosts_energy_consumption_checkpoint_,std::unordered_map<std::string, double> *latency_cache, std::vector<simgrid::s4u::Host*> all_hosts);
    simgrid::s4u::Host* find_host(shared_ptr<SegmentTask> ready_task);
    double get_host_co2_area(simgrid::s4u::Host* host,shared_ptr<SegmentTask> ready_task);
    double calculate_co2(shared_ptr<SegmentTask> ready_task,simgrid::s4u::Host * candidate_host);
    double calculate_response_time(shared_ptr<SegmentTask> ready_task,simgrid::s4u::Host * candidate_host);
    void update_sorted_hosts();

};
#endif
#ifndef SCHEDULINGBESTCO2VOLUME_HPP
#define SCHEDULINGBESTCO2VOLUME_HPP

#include "SchedulingStrategy.hpp"
#include <stdlib.h>
#include "ElectricityCO2eq.hpp"
#include <simgrid/s4u.hpp>
#include "LithiumIonBattery.hpp"
#include "simgrid/plugins/energy.h"
#include <stdlib.h>

class SchedulingBestCO2Volume : public SchedulingStrategy {

private:

// Map used to store the CPU cores availability of the hosts being used
map<string, int> *hosts_cpuavailability;
ElectricityCO2eq* local_grid_power_co2;
ElectricityCO2eq* cloud_dc_power_co2;
double pv_panel_power_co2; 
double battery_power_co2; 

simgrid::s4u::Host* cloud_cluster;
std::map<std::string, double> *hosts_renewable_energy;
std::map<std::string, LithiumIonBattery*> *hosts_batteries;
// Temporary map to include solar panels of hosts
std::map<std::string, double> *hosts_energy_consumption;


public:
    SchedulingBestCO2Volume(map<string, int> *hosts_cpu_availability_, ElectricityCO2eq* local_grid_power_co2_, ElectricityCO2eq* cloud_dc_power_co2_, double pv_panel_power_co2_, double battery_power_co2_, simgrid::s4u::Host* cloud_cluster_,std::map<std::string, double> *hosts_renewable_energy_,std::map<std::string, LithiumIonBattery*> *hosts_batteries_, std::map<std::string, double> *hosts_energy_consumption_);
    simgrid::s4u::Host* find_host(shared_ptr<SegmentTask> ready_task);
    double get_host_expected_volume(simgrid::s4u::Host* host,shared_ptr<SegmentTask> ready_task);

};
#endif
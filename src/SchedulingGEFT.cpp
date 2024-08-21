#include "SchedulingGEFT.hpp"
#include <bits/stdc++.h>
#include <boost/algorithm/string.hpp>
#include <memory>
#include "Util.hpp"
XBT_LOG_NEW_DEFAULT_CATEGORY(SchedulingGEFT, "SchedulingGEFT category");


SchedulingGEFT::SchedulingGEFT(map<string, int> *hosts_cpu_availability,std::map<std::string, double> *hosts_renewables, std::map<std::string, double> *hosts_energy,std::map<std::string, LithiumIonBattery*> *hosts_bat,std::unordered_map<std::string, string> *task_cache) : SchedulingHEFT(hosts_cpu_availability,task_cache)
{

    this->hosts_cpuavailability = hosts_cpu_availability;
    this->hosts_renewable_energy=hosts_renewables;
    this->hosts_energy_consumption=hosts_energy;
    this->hosts_batteries=hosts_bat;
}

/*  Returns the available renewable energy of the hosts during the time slot duration (for example, during 1 min).
 *  The renewable energy comes from the solar panels production and from the batteries.
 *  @param host the selected host
 *  @return the renewable energy (joules) that the host has during the current time slot
*/
double SchedulingGEFT::get_host_available_renewable_energy(simgrid::s4u::Host* host)
{
    // First the energy from the solar panels
    double available_renewable_power = (*hosts_renewable_energy)[host->get_name()];
    // Then we add the energy from the batteries
    available_renewable_power += Util::convert_wh_to_joules( (*hosts_batteries)[host->get_name()]->getUsableWattsHour());    
    // We also need to remove the power consumed by the host, to update the available renewable energy info
    double host_consumed_energy = sg_host_get_consumed_energy(host) - (*hosts_energy_consumption)[host->get_name()];
    double power_per_core = 1.2;
    double idle_power = 2.5;
    double run_time = 0.1;
    if (host->get_name().compare("cloud_cluster")==0)
    {
        idle_power = 117.0;
        power_per_core = 2.21875;
        run_time = 0.05;
    }
    int cores_used =  host->get_core_count() - (*hosts_cpuavailability)[host->get_name()] ;
    cores_used += 1 ; // to represent that we will allocate a task to this host
    double dynamic_energy = cores_used*power_per_core;
    host_consumed_energy += ((dynamic_energy+idle_power)*run_time);
    available_renewable_power-= host_consumed_energy;
    if (available_renewable_power >= 0 )
    {
        return available_renewable_power;
    }
    return 0;
}



simgrid::s4u::Host* SchedulingGEFT::find_host(shared_ptr<SegmentTask> ready_task)
{
    std::vector<simgrid::s4u::Host *> all_hosts = simgrid::s4u::Engine::get_instance()->get_all_hosts();
    std::vector<simgrid::s4u::Host *> hosts_with_renewable_energy;
    for (auto host : all_hosts)
    {

        double available_renewable_energy = get_host_available_renewable_energy(host);

        XBT_INFO("ENERGIA VERDE DO HOST %s = %f",host->get_cname(),available_renewable_energy);
        if(available_renewable_energy>0.0)
        {
            XBT_INFO("added na lista");

            hosts_with_renewable_energy.push_back(host);
        }
    }
    return SchedulingHEFT::find_host(ready_task,hosts_with_renewable_energy);

}

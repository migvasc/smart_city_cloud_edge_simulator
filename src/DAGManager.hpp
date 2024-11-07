#ifndef DAGMANAGER_HPP
#define DAGMANAGER_HPP

#include <simgrid/s4u.hpp>
#include <stdlib.h>
#include "DAGOfTasks.hpp"
#include "SegmentTask.hpp"
#include "CommunicationInterface.hpp"
#include "PVPanel.hpp"
#include "ElectricityCO2eq.hpp"
#include "simgrid/plugins/energy.h"
#include "LithiumIonBattery.hpp"
#include "EdgeHost.hpp"
#include <memory>
#include "SchedulingStrategy.hpp"
#include "SchedulingBaseline.hpp"
#include "SchedulingHEFT.hpp"
#include "SchedulingGEFT.hpp"
#include "SchedulingBestFit.hpp"
#include "SchedulingBestCO2.hpp"
#include "SchedulingBestCO2Volume.hpp"
#include "SchedulingBestCO2Neighbours.hpp"
#include "SchedulingHostType.hpp"

#include "WriteBuffer.hpp"

using namespace std::chrono; 


class DAGManager : CommunicationInterface {
private:
  // List of possible scheduling algorithms
  int SCHEDULING_ALGORITHM = -1;
  int SCHEDULING_BASELINE = 0;
  int SCHEDULING_HEFT = 1;
  int SCHEDULING_CO2 = 2;
  int SCHEDULING_CO2VOLUME = 3;
  int SCHEDULING_FIXED_HOST_TYPE = 4;
  int SCHEDULING_BESTFIT = 5;  
  int SCHEDULING_FIRSTFITSOLAR = 6;
  int SCHEDULING_GEFT = 7;
  int SCHEDULING_BASELINE_ON_OFF = 8;
  int SCHEDULING_CO2_NEIGHBOUR = 9;


  SchedulingStrategy *schedulingstrategy;
  // Host for the algorithm where all the tasks are scheduled to it (for example, offloading everything to the cloud)

  //Time slot duration in seconds (used for the energy envelope)
  double timeslot_duration = 60.0;
  // Auxiliary variable to keep track the next instant of time to update
  // renewable energy information
  double next_time_to_update = 0.0;
  double time_to_turn_on = 0.0;

  double cache_duration = -1;

  bool use_cache = false;

  // List with the current requests being processed
  std::vector<DAGOfTasks*> requests;
  
  // Map used to store the CPU cores availability of the hosts being used
  std::map<std::string, DAGOfTasks*> requests_map;


  // Map used to store the CPU cores availability of the hosts being used
  std::map<std::string, int> hosts_cpuavailability;


  // Map used to store the Hosts info, for example caching info
  std::map<std::string, EdgeHost*> hosts_info;



  // VARIABLES FOR CACHING

  // Used to store where the cache is (which host)
  std::unordered_map<std::string, string> task_cache;
  std::unordered_map<std::string, std::list<std::string>> cache_of_tasks;

  // Used to store when the cache was stored (for expiration)
  std::unordered_map<std::string, int> task_time_cache;
  void add_task_in_cache(std::string & task_name, const std::string& host_name);
  std::unordered_map<std::string, int> host_cache_mem_used;


  // Map used to store the information of the host that will manages the load,
  // for example, in the case that the host A is turned off, host B will 
  // handle the load
  std::map<std::string, string> hosts_manager_map;

  std::map<std::string, int> number_of_tasks_allocated;

  // The power state of the host will define if it is on or off:
  int PSTATE_ON = 1;
  int PSTATE_OFF = 0;


  // Temporary map to include solar panels of hosts
  std::map<std::string, PVPanel*> hosts_pvpanels;

  // Temporary map to include batteries of hosts
  std::map<std::string, LithiumIonBattery*> hosts_batteries;

  // Temporary map to include solar panels of hosts
  std::map<std::string, double> hosts_energy_consumption;

  // Envelope with available renewable energy for 5 min 
  std::map<std::string, double> hosts_renewable_energy;

  // Power production CO2eq values
  ElectricityCO2eq* local_grid_power_co2;
  ElectricityCO2eq* cloud_dc_power_co2;
  double pv_panel_power_co2; 
  double battery_power_co2; 
  simgrid::s4u::Host* cloud_cluster;

  
  // Attributes for writing the output
  WriteBuffer* tasks_output;
  WriteBuffer* requests_output;
  WriteBuffer* energy_output;
  WriteBuffer* co2_output;;

  std::string output_dir;


  // Max request execute time before it is canceled
  double max_request_run_time;

  void handle_message(Message *message);
  void turn_host_off(simgrid::s4u::Host *host);
  void turn_host_on(simgrid::s4u::Host *host);
  void init();
  void handle_request_submission(DAGOfTasks* dag);
  std::vector<std::string> argsClass;
  void perform_schedule();
  vector<shared_ptr<SegmentTask>> get_ready_tasks_from_requests();
  vector<shared_ptr<SegmentTask>> get_ready_tasks_cache();
  double get_host_available_renewable_energy(simgrid::s4u::Host* host);
  void update_hosts_energy_information();
  void update_battery_state(simgrid::s4u::Host* host);
  void update_valid_requests();
  void evaluate_turn_on_or_off();
  simgrid::s4u::Host* get_nearest_neighbour_host(simgrid::s4u::Host* host);
  // Get the network latency using Simgrid's Vivaldi network topology. More info: https://simgrid.org/doc/latest/Platform_routing.html#vivaldi
  double getNetworkLatencyVivaldi(double x1, double y1, double z1, double x2, double y2, double z2);

public:
  explicit DAGManager(std::vector<std::string> args);
  void operator()();
  void handle_task_finished(simgrid::s4u::Exec const& exec);
  void finish_request(const std::string last_task_id);  
  simgrid::s4u::Host* find_host(shared_ptr<SegmentTask> ready_task);
  simgrid::s4u::Host* find_host_bestfit(shared_ptr<SegmentTask> ready_task);
  simgrid::s4u::Host* find_host_HEFT(shared_ptr<SegmentTask> ready_task, bool renewable_energy_required);
  simgrid::s4u::Host* find_host_baseline(shared_ptr<SegmentTask> ready_task);
  simgrid::s4u::Host* find_host_renewable_energy(shared_ptr<SegmentTask> ready_task);

};
#endif
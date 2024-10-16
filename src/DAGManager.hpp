#ifndef DAGMANAGER_HPP
#define DAGMANAGER_HPP

#include <simgrid/s4u.hpp>
#include <stdlib.h>
#include "DAGOfTasks.hpp"
#include "SegmentTask.hpp"
#include "CommunicationInterface.hpp"
#include "PVPanel.hpp"
#include "simgrid/plugins/energy.h"
#include "LithiumIonBattery.hpp"

using namespace std::chrono; 


class DAGManager : CommunicationInterface {
private:
  // List of possible scheduling algorithms
  int SCHEDULING_ALGORITHM = -1;
  int SCHEDULING_BASELINE = 0;
  int SCHEDULING_BESTFIT = 1;  
  int SCHEDULING_FIRSTFITSOLAR = 2;
  int SCHEDULING_HEFT = 3;
  int SCHEDULING_GEFT = 4;
  int SCHEDULING_FIXED_HOST = 5;
  int SCHEDULING_BASELINE_ON_OFF = 6;


  // Host for the algorithm where all the tasks are scheduled to it (for example, offloading everything to the cloud)
  simgrid::s4u::Host *fixed_host = nullptr;

  //Time slot duration in seconds (used for the energy envelope)
  double timeslot_duration = 60.0;
  // Auxiliary variable to keep track the next instant of time to update
  // renewable energy information
  double next_time_to_update = 0.0;
  double time_to_turn_on = 0.0;
  // List with the current requests being processed
  std::vector<DAGOfTasks*> requests;
  // Map used to store the CPU cores availability of the hosts being used
  std::map<std::string, int> hosts_cpuavailability;




  // Turn off or on variables

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
  //void performSchedule();

  void handle_message(Message *message);
  void turn_host_off(simgrid::s4u::Host *host);
  void turn_host_on(simgrid::s4u::Host *host);
  void init();
  void handle_request_submission(DAGOfTasks* dag);
  std::vector<std::string> argsClass;
  void perform_schedule();
  vector<SegmentTask*> get_ready_tasks_from_requests();
  vector<SegmentTask*> get_one_ready_task_per_request();
  double get_host_available_renewable_energy(simgrid::s4u::Host* host);
  void update_hosts_energy_information();
  void update_battery_state(simgrid::s4u::Host* host);

  void evaluate_turn_on_or_off();
  simgrid::s4u::Host* get_nearest_neighbour_host(simgrid::s4u::Host* host);
public:
  explicit DAGManager(std::vector<std::string> args);
  void operator()();
  void handle_task_finished(simgrid::s4u::Exec const& exec);
  void finish_request(const std::string last_task_id);  
  simgrid::s4u::Host* find_host(SegmentTask *ready_task);
  simgrid::s4u::Host* find_host_bestfit(SegmentTask *ready_task);
  simgrid::s4u::Host* find_host_HEFT(SegmentTask *ready_task, bool renewable_energy_required);
  simgrid::s4u::Host* find_host_baseline(SegmentTask *ready_task);
  simgrid::s4u::Host* find_host_renewable_energy(SegmentTask *ready_task);

};
#endif
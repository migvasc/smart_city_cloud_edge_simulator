#ifndef DAGMANAGER_HPP
#define DAGMANAGER_HPP

#include <simgrid/s4u.hpp>
#include <stdlib.h>
#include "DAGOfTasks.hpp"
#include "SegmentTask.hpp"
#include "CommunicationInterface.hpp"

using namespace std::chrono; 


class DAGManager : CommunicationInterface {
private:

  int SCHEDULING_ALGORITHM = -1;
  int SCHEDULING_BASELINE = 0;
  int SCHEDULING_BESTFIT = 1;

  // List with the current requests being processed
  std::vector<DAGOfTasks*> requests;
  // Map used to store the CPU cores availability of the hosts being used
  std::map<std::string, int> hosts_cpuavailability;

  //void performSchedule();
  void handle_message(Message *message);
  void shut_down_host(simgrid::s4u::Host *host);
  void init();

  void handle_request_submission(DAGOfTasks* dag);
  std::vector<std::string> argsClass;
  void perform_schedule();
  vector<SegmentTask*> get_ready_tasks_from_requests();

public:
  explicit DAGManager(std::vector<std::string> args);
  void operator()();
  void handle_task_finished(simgrid::s4u::Exec const& exec);
  void finish_request(const std::string last_task_id);  
  simgrid::s4u::Host* find_host_bestfit();
  simgrid::s4u::Host* find_host_baseline(SegmentTask *ready_task);

};
#endif
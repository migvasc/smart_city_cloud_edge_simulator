
#include "DAGOfTasks.hpp"
XBT_LOG_NEW_DEFAULT_CATEGORY(log, "Messages specific for this s4u tutorial");

using namespace std; 

DAGOfTasks::DAGOfTasks(std::string& input_file)
{
    create_DAG_from_JSON(input_file);
}

std::vector<SegmentTask> DAGOfTasks::get_DAG()
{
    return this->dag;
}


/*Get list of ready tasks*/
std::vector<SegmentTask*> DAGOfTasks::get_ready_tasks()
{

  std::vector<SegmentTask*> ready_tasks;
  std::map<simgrid::s4u::Exec*, unsigned int> candidate_execs;

  for (SegmentTask& a : dag) {
    // Only look at activity that have their dependencies solved but are not assigned
    if (a.get_exec()->dependencies_solved() && !a.get_exec()->is_assigned() ) {
      // if it is an exec, it's ready
      if (auto* exec = dynamic_cast<simgrid::s4u::Exec*>(a.get_exec().get()))
        ready_tasks.push_back(&a);
    }
  }
  return ready_tasks;
}

/*Get list of ready tasks*/
SegmentTask* DAGOfTasks::get_one_ready_task()
{

  SegmentTask* ready_task = nullptr;

  for (SegmentTask& a : dag) {
    // Only look at activity that have their dependencies solved but are not assigned
    if (a.get_exec()->dependencies_solved() && !a.get_exec()->is_assigned() ) {
      // if it is an exec, it's ready
      if (auto* exec = dynamic_cast<simgrid::s4u::Exec*>(a.get_exec().get()))
        ready_task = &a;
    }
  }
  return ready_task;
}



/** @brief loads a JSON file describing a DAG
 * Simplified version of the SimGrid source code for generating DAGs
 * See https://github.com/wfcommons/wfformat for more details. We support wfformat 1.4.
 */
void DAGOfTasks::create_DAG_from_JSON(const std::string& filename)
{
  //Opens the json 
  std::ifstream f(filename);
  // Extract the json file contents into the data object
  auto data = nlohmann::json::parse(f);
  // Our dag is represented as a list of segments (streets)
  std::vector<SegmentTask> dag = {};
  // Map/Dictionary used for setting the dependences between the tasks
  std::map<std::string,simgrid::s4u::ExecPtr > map_parents = {};
  this->name = data["name"];


  for (auto const& task: data["tasks"]) 
  {
    // Simgrid's computational task, the exec prtr
    simgrid::s4u::ExecPtr exec_task = nullptr; 
    // Segment task in our model
    SegmentTask seg_task;
    // The host that has the local data of the segment (street)
    simgrid::s4u::Host* pref_host = simgrid::s4u::Host::by_name(task["machine"].get<std::string>());
    //Loads the task data from the json
    exec_task = simgrid::s4u::Exec::init()->set_name(task["name"].get<std::string>())->set_flops_amount(task["flopsAmount"].get<double>());
   
    std::string key = exec_task->get_name();
    map_parents[key] = exec_task;
    seg_task.set_pref_host(pref_host);
    seg_task.set_exec(exec_task);

    for(std::string parent : task["parents"]){
      simgrid::s4u::ExecPtr& parent_task =      map_parents[parent];
      seg_task.add_parent(parent_task);
      parent_task->add_successor(exec_task);
    }
    
    dag.push_back(seg_task);

  }
  
  for(auto& task : dag){
    // The final task of the DAG is the one that has no sucessor
    if (task.get_exec()->get_successors().size()==0)
      this->last_exec= task.get_exec();
  }  

  this->dag= dag;

}

simgrid::s4u::ExecPtr DAGOfTasks::get_last_exec()
{
  return this->last_exec;
}

std::string DAGOfTasks::get_name()
{
  return this->name;
}
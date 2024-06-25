#include "DAGOfTasks.hpp"
#include <bits/stdc++.h>
#include <boost/algorithm/string.hpp>
#include <memory>

XBT_LOG_NEW_DEFAULT_CATEGORY(log, "Messages specific for this s4u tutorial");

using namespace std; 
DAGOfTasks::DAGOfTasks(std::string& input_file)
{
    create_DAG_from_JSON(input_file);
}

std::vector<shared_ptr<SegmentTask>> DAGOfTasks::get_DAG()
{
    return this->dag;
}

/*Get list of ready tasks*/
std::vector<shared_ptr<SegmentTask>> DAGOfTasks::get_ready_tasks()
{
  std::vector<shared_ptr<SegmentTask>> ready_tasks;
  std::map<simgrid::s4u::Exec*, unsigned int> candidate_execs;
  for (auto& a : dag) {
    // Only look at activity that have their dependencies solved but are not assigned
    if (a->get_exec()->dependencies_solved() && !a->get_exec()->is_assigned() ) {
      // if it is an exec, it's ready
      if (auto* exec = dynamic_cast<simgrid::s4u::Exec*>(a->get_exec().get()))
        ready_tasks.push_back(a);
    }
  }
  return ready_tasks;
}

std::vector<shared_ptr<SegmentTask>>  DAGOfTasks::get_ready_tasks_cache(unordered_map<string,string>& cache,unordered_map<string,int>& time_cache )
{
  std::vector<shared_ptr<SegmentTask>> ready_tasks;
  std::map<simgrid::s4u::Exec*, unsigned int> candidate_execs;
  // Used to see if the cache is still valid
  int current_time_for_cache = int(simgrid::s4u::Engine::get_clock()/5);
  
  for (auto& task : dag) 
  {
    vector<string> result;
    boost::split(result, task->get_exec()->get_name(), boost::is_any_of("-"));
    std::string task_name = result[2];

    // These ini and end tasks are used to simulate the user 
    // sending the request and receiving the result
    if (task_name.compare("ini")!=0 )
    {

      // If the ini task has not been completed, it is not possible to execute the other tasks
      if(!can_start_computations()) break;


      // If it has not been allocated yet, we can validate if there is cache for it
      // because if it has been alocated, it is already execution/in execution and therefore we
      // cannot kill it
      if  (task->get_allocated_host()==nullptr)
      {
        // First we validate if we already computed information 
        // for this street and if it is in cache
        auto it_cache = cache.find(task_name);    
        bool hasCache = it_cache != cache.end();    

        if (hasCache)
        {
          // If it is in cache, we need to validate if the data is still up to date
          // (if the information has not expired)
          bool cache_expired = false;      
          auto it_time = time_cache.find(task_name);    

          if(it_time!= time_cache.end())
          {
            cache_expired = it_time->second != current_time_for_cache;
          }

          // If it has expired, we remove the data of the task from the cache
          if(cache_expired)
          {
            cache.erase(it_cache);
            time_cache.erase(it_time);
      //      XBT_INFO("task %s is in cache but it is not valid anymore, so the task will be executed",task->get_exec()->get_cname());

          }
          else // otherwise, we do not need to add it to the list of ready tasks
          {
            simgrid::s4u::Host* host = simgrid::s4u::Host::by_name(cache.at(task_name));
            task->set_pref_host(host);
            task->set_cache();
        //    XBT_INFO("task %s is in cache and it  is still valid, so the task will NOOTTT be executed",task->get_exec()->get_cname());
            continue;
          }

        }
 
        // Now we evaluate for the dependencies (parents) of the task if they are in cache
        for (auto& parent : task->get_parents())
        {
          
          // If it has been allocated, we do not need to validate
          if (parent->get_allocated_host()!=nullptr) 
          {
            continue;
          }

          boost::split(result, parent->get_exec()->get_name(), boost::is_any_of("-"));
          std::string parent_name = result[2];
          it_cache = cache.find(parent_name);
          bool hasCache = it_cache != cache.end();
          if (hasCache)
          {
            // Similar to the evaluation for the cache done aboove, we will 
            // erase the task data from the cache if it expired
            bool cache_expired = false;      
            auto it_time = time_cache.find(parent_name);    
            if(it_time!= time_cache.end())
            {
              cache_expired = it_time->second != current_time_for_cache;
            }
            if(cache_expired)
            {
              cache.erase(it_cache);
              time_cache.erase(it_time);
            }
            else
            {
              // If the cache is still valid, we mark the parent as completed, in order
              // to be able to execute the tasks (finish all its dependencies)
              if( parent->get_allocated_host()==nullptr && (parent->get_exec()->get_state() == simgrid::s4u::Activity::State::INITED ||parent->get_exec()->get_state() == simgrid::s4u::Activity::State::STARTING ))

              {
                if(!parent->get_exec()->is_assigned() )
                {
                  parent->get_exec()->set_host(simgrid::s4u::Host::by_name(it_cache->second));
                }
           //     XBT_INFO("parent %s of task task %s is in cache and will not be executed",parent->get_exec()->get_cname(), task->get_exec()->get_cname());

                // Validates if the parent is already in the execution list,
                // if so, we need to remove it from the list
                std::vector<std::shared_ptr<SegmentTask>>::iterator it = ready_tasks.begin();

                while(it != ready_tasks.end()) {
                    if( parent->get_exec()->get_name().compare((*it)->get_exec()->get_name())==0 )
                    {

                        it = ready_tasks.erase(it);
                        break;
                    }
                    else ++it;
                }

                parent->get_exec()->complete(simgrid::s4u::Activity::State::FINISHED);
              }
            }          
          }
        }
      }      
    }

    // Only look at activity that have their dependencies solved but are not assigned    
    if ( ( task->is_ready_for_execution(cache,time_cache) ) && !task->get_exec()->is_assigned() ) 
    {
      // if it is an exec, it's ready
      if (auto* exec = dynamic_cast<simgrid::s4u::Exec*>(task->get_exec().get()))
      {
        ready_tasks.push_back(task);
      //  XBT_INFO("TASK %s foi adicionada na lista pra ser executada",task->get_exec()->get_cname());
        if (task_name.compare("ini")==0 ) break;

      }
    }
  }

  return ready_tasks;

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
  std::vector<shared_ptr<SegmentTask>> dag = {};
  // Map/Dictionary used for setting the dependences between the tasks
  std::map<std::string,shared_ptr<SegmentTask>> map_parents = {};
  this->name = data["name"];

  for (auto const& task: data["tasks"]) 
  {
    // Simgrid's computational task, the exec prtr
    simgrid::s4u::ExecPtr exec_task = nullptr; 
    
    // Segment task in our model
    shared_ptr<SegmentTask> seg_task = make_shared<SegmentTask>();

    // The host that has the local data of the segment (street)
    simgrid::s4u::Host* pref_host = simgrid::s4u::Host::by_name(task["machine"].get<std::string>());

    //Loads the task data from the json
    exec_task = simgrid::s4u::Exec::init()->set_name(task["name"].get<std::string>())->set_flops_amount(task["flopsAmount"].get<double>());

    std::string key = exec_task->get_name();
    
    seg_task->set_pref_host(pref_host);
    seg_task->set_exec(exec_task);
    
    map_parents[key] = seg_task;

    for(std::string parent : task["parents"])
    {
      shared_ptr<SegmentTask> parent_task = map_parents[parent];
      parent_task->get_exec()->add_successor(exec_task);
      seg_task->add_parent(parent_task);
    }

    dag.push_back(seg_task);

  }
  
  for(auto& task : dag){
    // The final task of the DAG is the one that has no sucessor
    if (task->get_exec()->get_successors().size()==0)
    {
      this->last_exec= task->get_exec();
    } 
  }  

  std::shared_ptr<SegmentTask> ini = make_shared<SegmentTask>() ;
  std::string req_ini_id = name + "-ini";
  simgrid::s4u::ExecPtr exec_task_ini =   simgrid::s4u::Exec::init()->set_name(req_ini_id)->set_flops_amount(0);
  ini->set_exec(exec_task_ini);

  ini->set_pref_host(dag[1]->get_pref_host());


  std::string req_end_id = name + "-end";
  std::shared_ptr<SegmentTask> end = make_shared<SegmentTask>() ;
  simgrid::s4u::ExecPtr exec_task_end =   simgrid::s4u::Exec::init()->set_name(req_end_id)->set_flops_amount(0);
  end->set_pref_host(ini->get_pref_host());
  end->set_exec(exec_task_end);

  for (auto& task: dag)
  {
    if (task->get_exec()->get_name().compare(ini->get_exec()->get_name())==0)
    {
      continue;
    }
    // Não tem pai
    if(task->get_exec()->dependencies_solved())
    {
      ini->get_exec()->add_successor(task->get_exec());
      task->add_parent(ini);

    }
  }

  end->add_parent(dag[dag.size()-1]);
  this->last_exec->add_successor(end->get_exec());
  this->last_exec = end->get_exec();

  dag.insert(dag.begin(),ini);
  dag.push_back(end);

  this->dag = dag;

}

simgrid::s4u::ExecPtr DAGOfTasks::get_last_exec()
{
  return this->last_exec;
}

std::string DAGOfTasks::get_name()
{
  return this->name;
}


void DAGOfTasks::request_received()
{
  this->ini_task_finished = true;
}

bool DAGOfTasks::can_start_computations()
{
  return this->ini_task_finished;
}

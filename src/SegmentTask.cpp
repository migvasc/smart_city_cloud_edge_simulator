#include "SegmentTask.hpp"

void SegmentTask::clear_parents_in_cache()
{
  /*for (auto& task : this->parents_in_cache)
  {
      if (auto ptr =  (task.second).lock())  
      {  
        if(ptr->get_exec()->is_assigned()){
          ptr->get_exec()->unset_host();
        }
          
      }
  }*/

  this->parents_in_cache.clear();
}

bool SegmentTask::is_ready_for_execution()
{

  bool dependencies_solved = this->ptr->dependencies_solved();

  if(dependencies_solved)
  {
    return true;
  }
  
  int number_of_dependencies = ptr->get_dependencies().size();

  for(auto & dep : ptr->get_dependencies() )
  {
    if(is_parent_in_cache(dep->get_name()))
    {
      number_of_dependencies-=1;
    }
  }
  

  return number_of_dependencies == 0 ;

}

bool SegmentTask::is_assigned()
{
  return this->ptr->is_assigned();
}

simgrid::s4u::Host* SegmentTask::get_pref_host()
{
  return this->pref_host;
}
  
void SegmentTask::start()
{
  this->ptr->start();
}

simgrid::s4u::ExecPtr SegmentTask::get_exec()
{
  return  this->ptr;
}

void SegmentTask::set_exec(simgrid::s4u::ExecPtr exec)
{
  this->ptr =exec;
}

void SegmentTask::set_pref_host(simgrid::s4u::Host* host)
{
  this->pref_host = host;
}

void SegmentTask::add_parent(const std::shared_ptr<SegmentTask> parent)
{
  this->parents[parent->get_exec()->get_name()] = parent;
}

void SegmentTask::add_parent_in_cache(const std::shared_ptr<SegmentTask> parent)
{
  this->parents_in_cache[parent->get_exec()->get_name()] = parent;
}

//std::vector<weak_ptr<SegmentTask>> SegmentTask::get_parents(){
std::map<std::string,std::shared_ptr<SegmentTask>> SegmentTask::get_parents() const
{

  std::map<std::string,std::shared_ptr<SegmentTask>> valid_parents;

  for (const auto & p : this->parents) 
  {
      if (auto ptr =  (p.second).lock())  
      {  
          valid_parents[p.first] = ptr;
      }
  }
  return valid_parents;
}

std::map<std::string,std::shared_ptr<SegmentTask>>  SegmentTask::get_parents_in_cache() const
{

  std::map<std::string,std::shared_ptr<SegmentTask>>  valid_parents;

  for (const auto & p : this->parents_in_cache) 
  {
      if (auto ptr =  (p.second).lock())  
      {  
          valid_parents[p.first] = ptr;
      }
  }
  return valid_parents;
}


bool SegmentTask::is_parent_in_cache(std::string parent_id)
{
  auto it_cache = this->parents_in_cache.find(parent_id);    
  return it_cache != this->parents_in_cache.end();    
}

void SegmentTask::clear_parents()
{
  this->parents.clear();
}



void SegmentTask::set_allocated_host(simgrid::s4u::Host* host)
{
  this->allocated_host = host;
}

simgrid::s4u::Host* SegmentTask::get_allocated_host()
{
  return this->allocated_host;
}



bool SegmentTask::has_cache()
{
  return data_in_cache;
}

void SegmentTask::set_cache()
{
  data_in_cache=true;
}
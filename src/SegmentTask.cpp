#include "SegmentTask.hpp"

bool SegmentTask::is_ready_for_execution(std::unordered_map<std::string,std::string>& cache,std::unordered_map<std::string,int>& time_cache )
{
  bool dependencies_solved = this->ptr->dependencies_solved();

  if(dependencies_solved)
  {
    return true;
  }
  
  int number_of_dependencies = ptr->get_dependencies().size();
  
  // If not, we validate the parents/dependencies that are not complete yet !
  for(auto dep : ptr->get_dependencies())
  {
    std::string dep_name = dep->get_name();

  }

  return false;
}
bool SegmentTask::is_assigned(){
  return this->ptr->is_assigned();
}

simgrid::s4u::Host* SegmentTask::get_pref_host(){
  return this->pref_host;
}

void SegmentTask::start(){
  this->ptr->start();
}

simgrid::s4u::ExecPtr SegmentTask::get_exec(){
  return  this->ptr;
}

void SegmentTask::set_exec(simgrid::s4u::ExecPtr exec){
  this->ptr =exec;
}

void SegmentTask::set_pref_host(simgrid::s4u::Host* host){
  this->pref_host = host;
}

void SegmentTask::add_parent(std::shared_ptr<SegmentTask> parent){
  //auto ptr = make_shared<SegmentTask>((*parent));
  this->parents.push_back((parent));
}

//std::vector<weak_ptr<SegmentTask>> SegmentTask::get_parents(){
std::vector<std::shared_ptr<SegmentTask>> SegmentTask::get_parents() const{

  std::vector<std::shared_ptr<SegmentTask>> valid_parents;

  for (const auto p : this->parents) 
  {

      std::shared_ptr<SegmentTask> s;
      s = p.lock();
      if (auto ptr = p.lock()) {  // Verifica se o ponteiro ainda é válido
          valid_parents.push_back(ptr);
      }
  }
  return valid_parents;
}


void SegmentTask::clear_parents(){
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
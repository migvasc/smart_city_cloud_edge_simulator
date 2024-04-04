#include "SegmentTask.hpp"

bool SegmentTask::dependencies_solved(){
  return this->ptr->dependencies_solved();
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

void SegmentTask::add_parent(simgrid::s4u::ExecPtr parent){
  this->parents.push_back(parent);
}

std::vector<simgrid::s4u::ExecPtr> SegmentTask::get_parents(){
  return this->parents;
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



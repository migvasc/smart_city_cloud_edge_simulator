
#include "DAGDispatcher.hpp"
#include "DAGOfTasks.hpp"
#include "simgrid/plugins/energy.h"
#include "MailBoxSingleton.hpp"

XBT_LOG_NEW_DEFAULT_CATEGORY(dagDisptacher, "dagDisptacher category");


DAGDispatcher::DAGDispatcher(std::vector<std::string> args)
{
    const char* actor_name     = args.at(0).c_str();
    const char* trace_filename = args.size() > 1 ? args[1].c_str() : nullptr;
    simgrid::xbt::replay_runner(actor_name, trace_filename);
}

void DAGDispatcher::operator()()
{
// Nothing to do here
}

void DAGDispatcher::send_DAG_of_tasks(simgrid::xbt::ReplayAction& action){

    double time  =  std::stod(action[2]);
    wait_for(time);
    std::string input_file;
    DAGOfTasks* dag = new DAGOfTasks(action[3]);
    
    for(auto task : dag->get_DAG()){
        XBT_INFO("TASK '%s'",task->get_exec()->get_cname());
    }
    MailBoxSingleton::get_instance()->send_message("dagmanager",new Message(MESSAGE_DAG_SUBMISSION,dag));

}

void DAGDispatcher::stop(simgrid::xbt::ReplayAction& action){
    double time  =  std::stod(action[2]);
    wait_for(time);
    //sg_host_energy_update_all();
    simgrid::s4u::Actor::kill_all();
    simgrid::s4u::this_actor::exit();
}

void DAGDispatcher::wait_for(double time){
    if (time > simgrid::s4u::Engine::get_clock())
    {
        double wait_forTime = time - simgrid::s4u::Engine::get_clock();
        simgrid::s4u::this_actor::sleep_for(wait_forTime);

    }
         
}



#include "EdgeHost.hpp"

using namespace std; 

EdgeHost::EdgeHost(std::string name, int mem_limit)
{
    host_name = name;
    host_mem_limit = mem_limit;
    mailbox = simgrid::s4u::Mailbox::by_name(name);
}

static void run_parent_com(simgrid::s4u::CommPtr comm)
{    
    comm->start();
    comm->wait();
    simgrid::s4u::this_actor::exit();
}


static void execute(SegmentTask *segment )
{

  simgrid::s4u::ExecPtr exec = segment->get_exec();
  

  simgrid::s4u::ExecPtr ask_data   = simgrid::s4u::Exec::init();
  simgrid::s4u::ExecPtr send_data   = simgrid::s4u::Exec::init();

  
  ask_data->set_flops_amount(0);
  send_data->set_flops_amount(0);

  ask_data->set_host(segment->get_allocated_host());
  send_data->set_host(segment->get_pref_host());

  std::vector<simgrid::s4u::CommPtr> parent_comm_list;


  for (auto parent : segment->get_parents())
  {
    if(parent.second->get_allocated_host()!=segment->get_allocated_host())
    {   
        simgrid::s4u::CommPtr comm = simgrid::s4u::Comm::sendto_init();                        
        comm->set_payload_size(1e3);      
        std::string id = "p@"+parent.first;
        comm->set_name(id);
        comm->add_successor(ask_data);        
        comm->set_source(parent.second->get_allocated_host());
        comm->set_destination(exec->get_host());
        parent_comm_list.push_back(comm);
    }
  }


  simgrid::s4u::CommPtr comm_ask = simgrid::s4u::Comm::sendto_init();                        
  comm_ask->set_payload_size(1e3);      
  std::string id = "ask";
  comm_ask->set_name(id);
  comm_ask->set_source(ask_data->get_host());
  comm_ask->set_destination(send_data->get_host());




  simgrid::s4u::CommPtr comm_send = simgrid::s4u::Comm::sendto_init();                        
  comm_send->set_payload_size(1e3);      
  id = "send";
  comm_send->set_name(id);
  comm_send->set_source(send_data->get_host());
  comm_send->set_destination(ask_data->get_host());


  ask_data->add_successor(comm_ask);  
  comm_ask->add_successor(send_data);        
  send_data->add_successor(comm_send);
  comm_send->add_successor(exec);

  
  std::string  name= exec->get_name(); 

  for (auto parent_comm: parent_comm_list)
  {    
    simgrid::s4u::Actor::create("comm_", simgrid::s4u::this_actor::get_host(), run_parent_com,parent_comm);        

  }

  ask_data->start();
  ask_data->wait();  
  comm_ask->start();
  comm_ask->wait();
  send_data->start();
  send_data->wait();
  comm_send->start();
  comm_send->wait();
  exec->start();
  exec->wait();
  Message *message = new Message(MESSAGE_TASK_COMPLETED,&name);
  simgrid::s4u::Mailbox::by_name("dagmanager")->put(message,0);


  simgrid::s4u::this_actor::exit();
}

void EdgeHost::operator()()
{

    bool can_execute = true;

    do
    {

        Message *message = mailbox->get<Message>();
        if (message->type==MESSAGE_END_SIMULATION)
        {
            can_execute = false;
        }
        else
        {

            SegmentTask * segment = static_cast<SegmentTask*>(message->data);            
            simgrid::s4u::ExecPtr executor = segment->get_exec();
            simgrid::s4u::Actor::create("exec_", simgrid::s4u::this_actor::get_host(), execute,segment);        

        }
    } while (can_execute);
    

}


void EdgeHost::add_task_in_cache(const std::string & task_name, int timeslot)
{
    cache[task_name]  = timeslot;
}

void EdgeHost::delete_task_from_cache(const std::string & task_name)
{
    cache.erase(task_name);
}

void EdgeHost::expire_tasks(int timeslot)
{
    for (auto it = cache.cbegin(); it != cache.cend() /* not hoisted */; /* no increment */)
    {
        // Second is the value of the hashmap, considering the Time to Live
        if ( it->second < timeslot)
        {
            cache.erase(it++);    // or "it = m.erase(it)" since C++11
        }
        else
        {
            ++it;
        }
    }
}


bool EdgeHost::has_free_memory_in_cache()
{
    return cache.size() < host_mem_limit;
}


bool EdgeHost::validate_task_expiration(std::string task_name, int timeslot)
{
    if(cache[task_name] < timeslot)
    {
        cache.erase(task_name);
        return false;
    }
    return true;
}


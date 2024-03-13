#include <algorithm>
#include "simgrid/plugins/live_migration.h"
#include <map>
#include "DAGManager.hpp"

XBT_LOG_NEW_DEFAULT_CATEGORY(DAGManager, "DAGManager category");

/**
 * Manages the distributed edge infrastructure with on-site green (e.g. solar) energy productions.
 * The edge infrastructure is composed of low capacity nodes (as bus stops equipped with Raspberry PI nodes).
 * The DAGManager is in charge of dispatching an incoming workload to the different computing nodes (edge or the cloud).
 * Rather than directly interacting with the hosts, the DAGManager deploy and interact with worker applications which encapsulate the hosts desired behavior. 
 * The workload is composed of requests represented by a DAG of tasks.
 *
 */

DAGManager::DAGManager(std::vector<std::string> args)
{      
    argsClass = args;    
}

void DAGManager::operator()()
{
    register_object("dagmanager",this);            
    init();
    while(true)
    {
        perform_schedule(); 
        simgrid::s4u::this_actor::sleep_for(0.0001);            
    }
}

void DAGManager::handle_task_finished(simgrid::s4u::Exec const& exec)
{
    this->hosts_cpuavailability[exec.get_host()->get_name()]= this->hosts_cpuavailability[exec.get_host()->get_name()]+1;
    if (exec.get_successors().size() ==0)
    {
        this->finish_request(exec.get_name());
    }
}

void DAGManager::init()
{
    // When the simulation starts, all hosts are available to receive tasks
    for (auto& host : simgrid::s4u::Engine::get_instance()->get_all_hosts())
    {
        hosts_cpuavailability[host->get_name()] = host->get_core_count();
    }
    // Log when a tasks finishes
    simgrid::s4u::Exec::on_completion_cb([this](simgrid::s4u::Exec const& exec) 
    {

        XBT_INFO("#FE;%s;%f;%f;%s", exec.get_cname(), exec.get_start_time(), exec.get_finish_time(),exec.get_host()->get_cname());
        this->handle_task_finished(exec); 
    });    
    // Log when a communication finishes
    simgrid::s4u::Comm::on_completion_cb([this](simgrid::s4u::Comm const& comm) {
        XBT_INFO("#FC;%s;%f;%f;%s;%s", comm.get_cname(), comm.get_start_time(), comm.get_finish_time(),comm.get_source()->get_cname(),comm.get_destination()->get_cname());
        
    });
    SCHEDULING_ALGORITHM  = std::stoi(argsClass[argsClass.size()-1]); 
}

void DAGManager::shut_down_host(simgrid::s4u::Host *host)
{
    if(host->get_name()!=simgrid::s4u::this_actor::get_host()->get_name())
    {
        host->set_pstate(1);
        host->turn_off();
    }
}

void DAGManager::handle_message(Message* message)
{
    if (message != nullptr)
    {            
        switch (message->type)
        {
            case MESSAGE_VM_SUBMISSION:
                handle_request_submission(static_cast<DAGOfTasks*>(message->data));        
                break;      
            default:
                break;
        }
        message = nullptr;
    }
}

void DAGManager::handle_request_submission(DAGOfTasks* dag){
    requests.push_back(dag);
}

/**
 * Starts executing the computational task of the request.
 */
static void execute(simgrid::s4u::ExecPtr exec)
{
  XBT_INFO("SCHEDULED TASK %s on host %s",exec->get_cname(),exec->get_host()->get_cname());
  exec-> init();
  exec-> wait(); 
  simgrid::s4u::this_actor::exit();
}


/**
 * Creates a communication between two hosts, for sending the requested data.
 */
static void communicate(simgrid::s4u::ExecPtr task, simgrid::s4u::Host* src_host,simgrid::s4u::Host* dest_host)
{             
    simgrid::s4u::CommPtr comm = simgrid::s4u::Comm::sendto_init();
    comm->set_payload_size(1e3);  
    std::string id = "p@"+task->get_name() + "@" + src_host->get_name()+ "@" + dest_host->get_name();
    std::vector<simgrid::s4u::Link *> links;                            
    src_host->route_to(dest_host,links,nullptr);        
    XBT_INFO("CAMINHO ENTRE %s E %s",src_host->get_cname(),dest_host->get_cname());
    for(auto link : links)
    {
        XBT_INFO("%s",link->get_cname());
    }                 
    comm->set_name(id);
    comm->add_successor(task);
    comm->set_source(src_host);
    comm->set_destination(dest_host);
    comm->wait();
}

static void communicate_different_best_host(SegmentTask* segment, simgrid::s4u::Host* src_host,simgrid::s4u::Host* dest_host)
{
    simgrid::s4u::CommPtr comm1 = simgrid::s4u::Comm::sendto_init();
    simgrid::s4u::CommPtr comm2 = simgrid::s4u::Comm::sendto_init();
    comm1->set_payload_size(1e3);
    std::string id = "comm1@"+ segment->get_exec()->get_name() + "@" + src_host->get_name()+ "@" + dest_host->get_name(); 
    comm1->set_name(id);
    comm1->set_source(src_host);
    comm1->set_destination(dest_host);
    comm2->set_payload_size(1e3);
    std::string id2 = "comm2@"+ segment->get_exec()->get_name() + "@" + dest_host->get_name()+ "@" + src_host->get_name();    
    comm2->set_name(id2);
    comm2->set_source(dest_host);
    comm2->set_destination(src_host);
    segment->parent_coms.push_back(comm1);
    segment->parent_coms.push_back(comm2);
    comm2->add_successor(segment->get_exec()); 
    comm1->add_successor(comm2);
    comm1->wait();
    comm2->wait();
}

/**
 * Returns the list of tasks from all the requests that have their dependencies solved and can be executed.
 */
vector<SegmentTask*> DAGManager::get_ready_tasks_from_requests()
{
    vector<SegmentTask*> ready_tasks; 
    for(auto& request : requests)
    {
        for(auto& task : request->get_ready_tasks())
        {
            ready_tasks.push_back(task);
        }
    }
    return ready_tasks;
}

/**
 * Allocates the tasks to the hosts, respecting the computational capacity and the selected scheduling policy.
 */
void DAGManager::perform_schedule()
{   
    //Lists of tasks that are ready to be scheduled in the current instant of time
    vector<SegmentTask*> ready_tasks = get_ready_tasks_from_requests(); 

    // Flag used to validate if there is need to perform communications to get its required data,
    // for example, if the parent of a task was executed in a different host, or if the tasks will
    // be executed in a host that does not have all its necessary data
    bool no_comm_needed = true;

    for(auto& segment : ready_tasks)
    {
        auto task = segment->get_exec();
        simgrid::s4u::Host* candidate_host;
        if(SCHEDULING_ALGORITHM == SCHEDULING_BASELINE)
        {
            candidate_host =  find_host_baseline(segment);
        }
        else if (SCHEDULING_ALGORITHM == SCHEDULING_BESTFIT)
        {
            candidate_host =  find_host_bestfit();
        }    
        if (candidate_host == nullptr) continue;
        
        // First we validate if all the parent tasks (if any) have been executed in different hosts than the canidate host
        if (no_comm_needed)
        {
            for(auto& parent :segment->get_parents() )
            {
                simgrid::s4u::Host* src_host = parent->get_host();
                if(src_host!=candidate_host)
                {
                    no_comm_needed = false;
                    simgrid::s4u::Actor::create("comm_worker", src_host, communicate,task,src_host,candidate_host);
                }
            }
            segment->clear_parents();
        }

        // Then, we check if the candidate host has all the necessary data for the task
        if(no_comm_needed && candidate_host!=segment->get_pref_host())
        {
            XBT_INFO("hosts diffs %s vs %s",candidate_host->get_cname(),segment->get_pref_host()->get_cname());
            // 
            if(segment->parent_coms.size()==0)
            {
                no_comm_needed = false;
                simgrid::s4u::Actor::create("comm_worker_diff_best", candidate_host, communicate_different_best_host,segment,candidate_host,segment->get_pref_host());
            }
            else
            {
                no_comm_needed==true;
                for (auto& com :segment->parent_coms )
                {
                    auto state= com->get_state();
                    if(state != simgrid::s4u::Activity::State::FINISHED)
                    {
                        no_comm_needed == false;
                        break;
                    }                    
                }
            }
        }

        // No comunincation was needed for this tasks (due to parent or being allocated to diferent host)
        if(no_comm_needed)
        {
            task->set_host(candidate_host);
            XBT_INFO("#SCHEDULE;%s;%d;%s",task->get_cname(), hosts_cpuavailability[candidate_host->get_name()],candidate_host->get_cname());
            hosts_cpuavailability[candidate_host->get_name()]  = hosts_cpuavailability[candidate_host->get_name()] -1;                
            simgrid::s4u::Actor::create("worker", segment->get_pref_host(), execute,task);
        }
    }
    ready_tasks.clear();
}


void DAGManager::finish_request(const std::string last_task_id)
{    
    for(auto& request : requests)
    {
        if (request->get_last_exec()->get_name().compare(last_task_id)==0)
        {        
            XBT_INFO("#FR;%s;%f", request->get_name().c_str(), simgrid::s4u::Engine::get_clock());
            this->requests.erase(std::remove(this->requests.begin(), this->requests.end(), request), this->requests.end());
            break;
        }
    }
}   

simgrid::s4u::Host* DAGManager::find_host_baseline(SegmentTask *ready_task)
{
    simgrid::s4u::Host* host = ready_task->get_pref_host();
    if(  hosts_cpuavailability[host->get_name()]>0)
    {
        return host;
    }
    return nullptr;
}


/**
 * Starts executing the computational task of the request.
 */
simgrid::s4u::Host* DAGManager::find_host_bestfit()
{
    int free_cores = 1;
    simgrid::s4u::Host* selected_host = nullptr;
    while(free_cores <5)
    {
        for (auto& host : simgrid::s4u::Engine::get_instance()->get_all_hosts())
        {
            if(hosts_cpuavailability[host->get_name()] == free_cores) 
            {
                selected_host = host;
                return selected_host;
            } 
        }
        free_cores++;
    }
    return selected_host;
}
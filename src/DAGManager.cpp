#include <algorithm>
#include "simgrid/plugins/live_migration.h"
#include <map>
#include "DAGManager.hpp"
#include <bits/stdc++.h>
#include <boost/algorithm/string.hpp>

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

static double convert_wh_to_joules(double energy_wh)
{
    return energy_wh * 3600;
}

static double convert_joules_to_wh(double energy_joules)
{
    return energy_joules/ 3600;
}

void DAGManager::operator()()
{
    register_object("dagmanager",this);
    init();
    while(true)
    {
        if(int(simgrid::s4u::Engine::get_clock()) == next_time_to_update)
        {
            update_hosts_energy_information();
            if (SCHEDULING_ALGORITHM == SCHEDULING_BASELINE_ON_OFF)
            {
                evaluate_turn_on_or_off();
            }
        }
        perform_schedule(); 
        simgrid::s4u::this_actor::sleep_for(0.005);
    }        
}

void DAGManager::handle_task_finished(simgrid::s4u::Exec const& exec)
{

    //Get the task name to process the cache 
    vector<string> result;
    boost::split(result, exec.get_name(), boost::is_any_of("-"));
    std::string task_name =result[2];

    if ( (task_name.compare("ini")!=0 &&  task_name.compare("end")!=0  ))
    {
        this->hosts_cpuavailability[exec.get_host()->get_name()]= this->hosts_cpuavailability[exec.get_host()->get_name()]+1;
    }

    if (exec.get_successors().size() ==0)
    {
        this->finish_request(exec.get_name());
    }

    std::string request_name = result[0]+"-"+ result[1];
    if(! (task_name.compare("ini")==0 || task_name.compare("end")==0))
    {
        task_cache[task_name] = exec.get_host()->get_name();
        task_time_cache[task_name] = int (simgrid::s4u::Engine::get_clock()/5);
    }

    if( task_name.compare("ini")==0 )
    {
        requests_map[request_name]->request_received();
    }
}

void DAGManager::init()
{

    // Used to access the parameters of the deploy file
    int arg_index = 1;

    // Uses the parameter info to define which scheduling heuristic will be used
    SCHEDULING_ALGORITHM  = std::stoi(argsClass[arg_index]);

    double solar_panels_area = std::stod(argsClass[++arg_index]);
    double solar_panels_efficiency = std::stod(argsClass[++arg_index]);
    double battery_capacity = std::stod(argsClass[++arg_index]);
    double battery_dod = std::stod(argsClass[++arg_index]);
    double battery_charge_efficiency = std::stod(argsClass[++arg_index]);
    double battery_discharge_efficiency = std::stod(argsClass[++arg_index]);

    // When the simulation starts, all hosts are available to receive tasks
    for (auto& host : simgrid::s4u::Engine::get_instance()->get_all_hosts())
    {
        // Initially, the host "is responsible for itself", that is, 
        // since it is on, there is no need to redirect the workload to another host
        hosts_manager_map[host->get_name()] = host->get_name();
        // In the beginning of the simulations, all the cores are available, since it didnt started executing
        hosts_cpuavailability[host->get_name()] = host->get_core_count();
        // We create the PV panels and batteries using information from the parameters
        hosts_pvpanels[host->get_name()] = new PVPanel(argsClass[++arg_index],solar_panels_efficiency,solar_panels_area);
        hosts_batteries[host->get_name()] = new LithiumIonBattery(battery_capacity, battery_dod,battery_charge_efficiency,battery_discharge_efficiency);        
        // We init the auxilary map with the information of the energy consumption
        hosts_energy_consumption[host->get_name()] = 0.0;


        // We init the host info for caching
        //hosts_info[host->get_name()] = new EdgeHost();

    }

    if (SCHEDULING_ALGORITHM == SCHEDULING_FIXED_HOST)
    {
        fixed_host = simgrid::s4u::Host::by_name(argsClass[++arg_index]);
    }
    // Log when a tasks finishes
    simgrid::s4u::Exec::on_start_cb([this](simgrid::s4u::Exec const& exec) 
    {
        XBT_INFO("#COMECOU EXEC DA TASK;%s;%f;%f;%s", exec.get_cname(), exec.get_start_time(), exec.get_finish_time(),exec.get_host()->get_cname());
    });    

    // Log when a tasks finishes
    simgrid::s4u::Exec::on_completion_cb([this](simgrid::s4u::Exec const& exec) 
    {
        XBT_INFO("#FE;%s;%f;%f;%s", exec.get_cname(), exec.get_start_time(), exec.get_finish_time(),exec.get_host()->get_cname());
        if(exec.get_finish_time()!= -1.0)
        {
            this->handle_task_finished(exec); 
        }
        
    });    
    // Log when a communication finishes
    simgrid::s4u::Comm::on_completion_cb([this](simgrid::s4u::Comm const& comm) {
        XBT_INFO("#FC;%s;%f;%f;%s;%s", comm.get_cname(), comm.get_start_time(), comm.get_finish_time(),comm.get_source()->get_cname(),comm.get_destination()->get_cname());   
    });
}

void DAGManager::turn_host_off(simgrid::s4u::Host *host)
{
    host->set_pstate(0);
}


void DAGManager::turn_host_on(simgrid::s4u::Host *host)
{
    host->set_pstate(1);
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
    requests_map[dag->get_name()] = dag;
}

/**
 * Starts executing the computational task of the request.
 */
static void execute(simgrid::s4u::ExecPtr exec)
{
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
  /* XBT_INFO("CAMINHO ENTRE %s E %s",src_host->get_cname(),dest_host->get_cname());
    for(auto link : links)
    {
        XBT_INFO("%s",link->get_cname());
    }                 
*/
    comm->set_name(id);
    comm->add_successor(task);
    comm->set_source(src_host);
    comm->set_destination(dest_host);
    comm->wait();
    
}

static void communicate_different_best_host(shared_ptr<SegmentTask>  segment, simgrid::s4u::Host* src_host,simgrid::s4u::Host* dest_host)
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
vector<shared_ptr<SegmentTask>> DAGManager::get_ready_tasks_from_requests()
{
    vector<shared_ptr<SegmentTask>> ready_tasks; 
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
 * Returns the list of tasks from all the requests that have their dependencies solved and can be executed.
 */
vector<shared_ptr<SegmentTask>> DAGManager::get_ready_tasks_cache()
{
    vector<shared_ptr<SegmentTask>> ready_tasks; 
    for(auto& request : requests)
    {
        vector<shared_ptr<SegmentTask>> request_ready_tasks = request->get_ready_tasks_cache(task_cache,task_time_cache);

        for(auto& task : request_ready_tasks)
        {
            //task->get_exec()->remove_successor();
            ready_tasks.push_back(task);
        }
    }
    return ready_tasks;

}

/**
 * Allocates the tasks to the hosts, respecting the servers computational capacity and the selected scheduling policy.
 */
void DAGManager::perform_schedule()
{   
    //Lists of tasks that are ready to be scheduled in the current instant of time
    vector<shared_ptr<SegmentTask>> ready_tasks;
    if (SCHEDULING_ALGORITHM == SCHEDULING_BASELINE_CACHE)
    {
        ready_tasks = get_ready_tasks_cache(); 
    }    
    else
    {
        ready_tasks = get_ready_tasks_from_requests(); 
    }
    for(auto& segment : ready_tasks)
    {
        // Flag used to validate if there is need to perform communications to get its required data,
        // for example, if the parent of a task was executed in a different host, or if the tasks will
        // be executed in a host that does not have all its necessary data
        bool no_comm_needed = true;

        auto task = segment->get_exec();
   //     XBT_INFO("processing task %s",task->get_cname());
        simgrid::s4u::Host* candidate_host = find_host(segment);
        if (candidate_host == nullptr)
        {
         //   XBT_INFO("no host found for task %s",task->get_cname());
            continue;
        }
        double host_renewable_power = get_host_available_renewable_energy(candidate_host);
        
        // First we validate if all the parent tasks (if any) have been executed in different hosts than the canidate host
        if (no_comm_needed)
        {            
            for(auto& parent :segment->get_parents() )
            {
                simgrid::s4u::Host* src_host = parent->get_exec()->get_host();
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
            //XBT_INFO("hosts diffs %s vs %s",candidate_host->get_cname(),segment->get_pref_host()->get_cname());
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
        // No comunincation was needed for this task (due to parent or being allocated to diferent host)
        // or all the necessary communication has been completed. We can start executing it.
        if(no_comm_needed)
        {
            task->set_host(candidate_host);   
           /* auto it = hosts_info[candidate_host->get_name()]->cache.find(task->get_name());
            if (it != hosts_info[candidate_host->get_name()]->cache.end() && task->get_state() != simgrid::s4u::Activity::State::STARTED )
            {
                task->set_flops_amount(0.0);
            }*/
            XBT_INFO("#START TASK;%s;%d;%s",task->get_cname(), hosts_cpuavailability[candidate_host->get_name()],candidate_host->get_cname());
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

/***
 * Finds the hosts for the task that has avaialble CPU capacity and given a scheduling policy.
*/
simgrid::s4u::Host* DAGManager::find_host(shared_ptr<SegmentTask> ready_task)
{  
    simgrid::s4u::Host*  candidate_host =   nullptr;
    // Validates if there is no host allocated yet to the task
    if (ready_task->get_allocated_host()==nullptr)
    {
        if(SCHEDULING_ALGORITHM == SCHEDULING_BASELINE || SCHEDULING_ALGORITHM == SCHEDULING_BASELINE_ON_OFF || SCHEDULING_ALGORITHM == SCHEDULING_BASELINE_CACHE)
        {
            candidate_host =  find_host_baseline(ready_task);
        }
        else if (SCHEDULING_ALGORITHM == SCHEDULING_BESTFIT)
        {
            candidate_host =  find_host_bestfit(ready_task);
        }    
        else if (SCHEDULING_ALGORITHM == SCHEDULING_FIRSTFITSOLAR)
        {
            candidate_host = find_host_renewable_energy(ready_task);
        }
        else if (SCHEDULING_ALGORITHM == SCHEDULING_HEFT)
        {
            candidate_host = find_host_HEFT(ready_task,false);
        }      
        else if (SCHEDULING_ALGORITHM == SCHEDULING_GEFT)
        {
            candidate_host = find_host_HEFT(ready_task,true);
        }
        else if (SCHEDULING_ALGORITHM == SCHEDULING_FIXED_HOST)
        {
            if (hosts_cpuavailability[fixed_host->get_name()] >0 )
            {
                candidate_host = fixed_host;
            }
        }

        if (candidate_host == nullptr) return candidate_host;
        
        XBT_INFO("#SCHEDULE;%s;%d;%s;%f",ready_task->get_exec()->get_cname(), hosts_cpuavailability[candidate_host->get_name()],candidate_host->get_cname(),simgrid::s4u::Engine::get_clock());
        ready_task->set_allocated_host(candidate_host);
        

        vector<string> result;
        boost::split(result, ready_task->get_exec()->get_name(), boost::is_any_of("-"));
        std::string task_name = result[2];
        // These ini and end tasks are used to simulate the user 
        // sending the request and receiving the result
        if ( (task_name.compare("ini")!=0 &&  task_name.compare("end")!=0  ))
        {
            hosts_cpuavailability[candidate_host->get_name()] -=1;
        }
    }
    else
    {
        candidate_host = ready_task->get_allocated_host();
    }

    return candidate_host;
}

simgrid::s4u::Host* DAGManager::find_host_baseline(shared_ptr<SegmentTask> ready_task)
{  
    simgrid::s4u::Host* host =   ready_task->get_pref_host();
    

    if (SCHEDULING_ALGORITHM == SCHEDULING_BASELINE_ON_OFF)
    {
        number_of_tasks_allocated[host->get_name()] +=1;
        host = simgrid::s4u::Host::by_name(hosts_manager_map[host->get_name()]);                
        // In the case the host is off, another host shoud be handling it
        // so we schedule to this other host
        while (host->get_pstate()==PSTATE_OFF)
        {
            host = simgrid::s4u::Host::by_name(hosts_manager_map[host->get_name()]);
        }
    }
    if(  hosts_cpuavailability[host->get_name()]>0)
    {
        return host;
    }
    return nullptr;
}


simgrid::s4u::Host* DAGManager::find_host_renewable_energy(shared_ptr<SegmentTask> ready_task)
{
    for(auto host : simgrid::s4u::Engine::get_instance()->get_all_hosts())
    {
        if(hosts_renewable_energy[host->get_name()] > 0  && hosts_cpuavailability[host->get_name()]>0)
                return host;
    }
    return nullptr;
}

/**
 * Scheduling policy inspired by the EARLIEST FINISH TIME algorithm.
 * The finish times considers both the computation time and the communication time.
 * @param ready_task the task to be allocated
 * @param renewable_energy_required whether the host must have renewable energy to run the task
*/
simgrid::s4u::Host* DAGManager::find_host_HEFT(shared_ptr<SegmentTask> ready_task, bool renewable_energy_required)
{
    simgrid::s4u::Host* selected_host = nullptr;
    double min_finish_time = 999999999.9;

    for(auto candidate_host : simgrid::s4u::Engine::get_instance()->get_all_hosts())
    {
        if(hosts_cpuavailability[candidate_host->get_name()]==0) continue;

        if(renewable_energy_required)
        {
            if( get_host_available_renewable_energy(candidate_host)==0) continue;
        }

        double compute_time = ready_task->get_exec()->get_remaining() / candidate_host->get_speed();
        double comm_time = 0;

        //First, the communication from the parents
        double max_parent_comms =0;
        for(auto& parent :ready_task->get_parents() )
        {
            simgrid::s4u::Host* src_host = parent->get_exec()->get_host();
            std::vector<simgrid::s4u::Link *> links;                            
            src_host->route_to(candidate_host,links,nullptr);    
            double parent_latency = 0.0;    

            for(auto link : links)
            {
                parent_latency+=link->get_latency();
            }                 
            if (parent_latency > max_parent_comms)
            {
                max_parent_comms = parent_latency;
            }
         
        }
        // Since the communications occurs in parallel, it will use the value from the parent that has
        // the longest communication delay
        comm_time += max_parent_comms;


        // Now, we validate if the host does not has the local data
        if(candidate_host != ready_task->get_pref_host())
        {
            std::vector<simgrid::s4u::Link *> links;                            
            ready_task->get_pref_host()->route_to(candidate_host,links,nullptr);

            double latency = 0.0;    
            for(auto link : links)
            {
                latency+=link->get_latency();
            }                 
            comm_time+=2*latency;

        }
        // Validates if the finish time (computation and communication) is the smallest 
        // given all the hosts
        if ( (comm_time + compute_time)  < min_finish_time)
        {
            selected_host = candidate_host;
            min_finish_time = comm_time + compute_time;
        }
    }

    return selected_host;
}

/**
 * Starts executing the computational task of the request.
 */
simgrid::s4u::Host* DAGManager::find_host_bestfit(shared_ptr<SegmentTask> ready_task)
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

/*  Returns the available renewable energy of the hosts during the time slot duration (for example, during 1 min).
 *  The renewable energy comes from the solar panels production and from the batteries.
 *  @param host the selected host
 *  @return the renewable energy (joules) that the host has during the current time slot
*/
double DAGManager::get_host_available_renewable_energy(simgrid::s4u::Host* host)
{
    // First the energy from the solar panels
    double available_renewable_power = hosts_renewable_energy[host->get_name()];
    // Then we add the energy from the batteries
    available_renewable_power += convert_wh_to_joules( hosts_batteries[host->get_name()]->getUsableWattsHour());    
    // We also need to remove the power consumed by the host, to update the available renewable energy info
    double host_consumed_energy = sg_host_get_consumed_energy(host) - hosts_energy_consumption[host->get_name()];
    double power_per_core = 1.2;
    double idle_power = 2.5;
    double run_time = 0.1;
    if (host->get_name().compare("cloud_cluster")==0)
    {
        idle_power = 117.0;
        power_per_core = 2.21875;
        run_time = 0.05;
    }
    int cores_used =  host->get_core_count() - hosts_cpuavailability[host->get_name()] ;
    cores_used += 1 ; // to represent that we will allocate a task to this host
    double dynamic_energy = cores_used*power_per_core;
    host_consumed_energy += ((dynamic_energy+idle_power)*run_time);
    available_renewable_power-= host_consumed_energy;
    if (available_renewable_power >= 0 )
    {
        return available_renewable_power;
    }
    return 0;
}

void DAGManager::update_battery_state(simgrid::s4u::Host* host)
{

    double host_energy_consumption =   convert_joules_to_wh(sg_host_get_consumed_energy(host) - hosts_energy_consumption[host->get_name()]);
    double brown_energy_wh   = 0.0; 
    double energy_discharged = 0.0; 
    double renewable_power_prod = convert_joules_to_wh(hosts_renewable_energy[host->get_name()]);

    // Validate if there was a overproduction of solar power (more than consumption), in this case 
    // no energy was discharged from the battery and we can charge the excess PV energy in the battery    
    if (renewable_power_prod >= host_energy_consumption)
    {
        hosts_batteries[host->get_name()]->charge(renewable_power_prod-host_energy_consumption);
    }

    // If the solar power was not enough, we need to compute the amount of energy discharged from the batteries
    else
    {
        // First we compute how much energy was consumed that did not originate from the PV panels
        brown_energy_wh = host_energy_consumption - renewable_power_prod;

        // If the batteries had more energy than the brown energy consumed, we assume that 
        // this value was discharged from the batteries
        if (hosts_batteries[host->get_name()]->getUsableWattsHour()>=brown_energy_wh)
        {
            energy_discharged = hosts_batteries[host->get_name()]->discharge(brown_energy_wh);
            brown_energy_wh =0;
        }

        // Otherwise, we only discharge what was possible
        else
        {
            energy_discharged = hosts_batteries[host->get_name()]->discharge( hosts_batteries[host->get_name()] ->getUsableWattsHour() );
            brown_energy_wh-=hosts_batteries[host->get_name()] ->getUsableWattsHour();

        }
    }
    XBT_INFO("#ENERGY;%f;%s;%f;%f;%f;%f",simgrid::s4u::Engine::get_clock(),host->get_cname(),host_energy_consumption,renewable_power_prod, brown_energy_wh,hosts_batteries[host->get_name()]->getUsableWattsHour());
}

/**
 * Updates the information about renewable energy availability for all hosts,
 * considering both the PV panels and the batteries
*/
void DAGManager::update_hosts_energy_information()
{
    for (auto& host : simgrid::s4u::Engine::get_instance()->get_all_hosts())
    { 
        //First, update the energy in the batteries. For example, charge if there is overprduction during the previous time slot.
        update_battery_state(host);

        //Then, update the information about the solar panel energy production
        hosts_renewable_energy[host->get_name()] =  convert_wh_to_joules(hosts_pvpanels[host->get_name()]->get_current_green_power_production(simgrid::s4u::Engine::get_clock()));
        
        // Finally, we update the information of the host energy consumed
        hosts_energy_consumption[host->get_name()] =  sg_host_get_consumed_energy(host);
    }

    next_time_to_update += timeslot_duration;
}


void DAGManager::evaluate_turn_on_or_off()
{


    for (auto& host : simgrid::s4u::Engine::get_instance()->get_all_hosts())
    {
        if (host->get_pstate()==PSTATE_ON){
            if(number_of_tasks_allocated[host->get_name()]==0)
            {
                XBT_INFO("DESLIGOU %s",host->get_cname());
                
                simgrid::s4u::Host* new_manager = get_nearest_neighbour_host(host);
                if (new_manager!=nullptr)                                                                                                                                
                {
                    hosts_manager_map[host->get_name()] = new_manager->get_name();
                    turn_host_off(host);
                }
                

            }
        }
        else if (host->get_pstate()==PSTATE_OFF)
        {
            if(number_of_tasks_allocated[host->get_name()] >0 )
            {
                XBT_INFO("LIGOU %s",host->get_cname());
                turn_host_on(host);
                hosts_manager_map[host->get_name()] = host->get_name();
            }   
        }

        // Reinitialize the variable that registers the amount of tasks 
        // during the simulation
        number_of_tasks_allocated[host->get_name()] = 0;
    }    
}


/**
 * Returns the host that is closest to a given host in terms of network latency.
*/
simgrid::s4u::Host* DAGManager::get_nearest_neighbour_host(simgrid::s4u::Host* host)
{
 
    simgrid::s4u::Host* closest = nullptr;
    double min_latency =  1000.0; 

    for (auto& candidate_host : simgrid::s4u::Engine::get_instance()->get_all_hosts())
    {        
        if(candidate_host!=host && candidate_host->get_pstate() == PSTATE_ON)
        {            
            std::vector<simgrid::s4u::Link *> links;
            double latency = 0.0;
            host->route_to(candidate_host,links,nullptr);        
            for(auto link : links)
            {
                latency += link->get_latency();
            }
            if (latency<min_latency)
            {
                closest = candidate_host;
                min_latency = latency;
            }
            links.clear();
        }
    }

   return closest;
   
}
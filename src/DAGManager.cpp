#include <algorithm>
#include "simgrid/plugins/live_migration.h"
#include <map>
#include "DAGManager.hpp"
#include <bits/stdc++.h>
#include <boost/algorithm/string.hpp>
#include "Util.hpp"
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
        if(int(simgrid::s4u::Engine::get_clock()) == next_time_to_update)
        {
            update_hosts_energy_information();
            if (SCHEDULING_ALGORITHM == SCHEDULING_BASELINE_ON_OFF)
            {
                evaluate_turn_on_or_off();
            }
        }
        update_valid_requests();
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
        //XBT_INFO("COLOCOU A TAREFA %s da request %s em cache na maquina %s",task_name.c_str(),exec.get_cname(),exec.get_host()->get_cname());
        task_cache[task_name] = exec.get_host()->get_name();
        task_time_cache[task_name] = int (simgrid::s4u::Engine::get_clock()/cache_duration);
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
    std::string city_solar_traces = argsClass[++arg_index];


    for (auto& host : simgrid::s4u::Engine::get_instance()->get_all_hosts())
    {
        const std::unordered_map<std::string, std::string> * host_properties = host-> get_properties();

        if(host_properties->find("host_type")!=host_properties->end())
        {       
            std::string host_type = host->get_property("host_type");
            if (host_type.compare("bus_stop")==0)
            {        
                // We create the PV panels and batteries using information from the parameters        
                hosts_pvpanels[host->get_name()] = new PVPanel(city_solar_traces,solar_panels_efficiency,solar_panels_area);
                hosts_batteries[host->get_name()] = new LithiumIonBattery(battery_capacity, battery_dod,battery_charge_efficiency,battery_discharge_efficiency);        

            }
        }

        // Initially, the host "is responsible for itself", that is, 
        // since it is on, there is no need to redirect the workload to another host
        hosts_manager_map[host->get_name()] = host->get_name();
        // In the beginning of the simulations, all the cores are available, since it didnt started executing
        hosts_cpuavailability[host->get_name()] = host->get_core_count();

        // We init the auxilary map with the information of the energy consumption
        hosts_energy_consumption[host->get_name()] = 0.0;


        // We init the host info for caching
        host_cache_mem_used[host->get_name()] = 0;
        



    }


    if (SCHEDULING_ALGORITHM == SCHEDULING_BASELINE)
    {
        schedulingstrategy = new SchedulingBaseline(&hosts_cpuavailability);
    }
    else if(  SCHEDULING_ALGORITHM == SCHEDULING_HEFT)
    {
        schedulingstrategy = new SchedulingHEFT(&hosts_cpuavailability,&task_cache);
    }
    else if(  SCHEDULING_ALGORITHM == SCHEDULING_GEFT)
    {
        schedulingstrategy = new SchedulingGEFT(&hosts_cpuavailability,&hosts_renewable_energy,&hosts_energy_consumption,&hosts_batteries,&task_cache);
    }
    else if(  SCHEDULING_ALGORITHM == SCHEDULING_BESTFIT)
    {
        schedulingstrategy = new SchedulingBestFit(&hosts_cpuavailability);
    }


    // If baseline with cache, we get the selected cache duration from the parameter
    cache_duration = std::stoi(argsClass[++arg_index]);


    if(cache_duration>-1)
    {
        use_cache = true;
    }



    output_dir = argsClass[++arg_index];

    tasks_output = new WriteBuffer(output_dir + "tasks.csv");
    requests_output = new WriteBuffer(output_dir + "requests.csv");
    energy_output = new WriteBuffer(output_dir + "energy.csv");
    co2_output = new WriteBuffer(output_dir + "co2.csv");

    max_request_run_time = std::stod(argsClass[++arg_index]);

    pv_panel_power_co2 = std::stod(argsClass[++arg_index]);
    battery_power_co2 = std::stod(argsClass[++arg_index]);
    local_grid_power_co2 = new ElectricityCO2eq(argsClass[++arg_index]);
    cloud_dc_power_co2 = new ElectricityCO2eq(argsClass[++arg_index]);
    
    if( SCHEDULING_ALGORITHM == SCHEDULING_CO2)
    {
        schedulingstrategy = new SchedulingBestCO2(&hosts_cpuavailability, local_grid_power_co2, cloud_dc_power_co2,  pv_panel_power_co2,  battery_power_co2,  cloud_cluster,&hosts_renewable_energy,&hosts_batteries, &hosts_energy_consumption);
    }
    if( SCHEDULING_ALGORITHM == SCHEDULING_CO2VOLUME)
    {
        schedulingstrategy = new SchedulingLCAHEFT(&hosts_cpuavailability, local_grid_power_co2, cloud_dc_power_co2,  pv_panel_power_co2,  battery_power_co2,  cloud_cluster,&hosts_renewable_energy,&hosts_batteries, &hosts_energy_consumption);
    }
    if( SCHEDULING_ALGORITHM == SCHEDULING_CO2_NEIGHBOUR)
    {
        schedulingstrategy = new SchedulingBestCO2Neighbours(&hosts_cpuavailability, local_grid_power_co2, cloud_dc_power_co2,  pv_panel_power_co2,  battery_power_co2,  cloud_cluster,&hosts_renewable_energy,&hosts_batteries, &hosts_energy_consumption);
    }

    // If fixed host, we get the selected host from the parameter
    if (SCHEDULING_ALGORITHM == SCHEDULING_FIXED_HOST_TYPE)
    {
        std::string selected_host_type = argsClass[++arg_index];
        std::vector<simgrid::s4u::Host *> selected_host_type_array ;

         // Validate if the host is the selected type
        for (auto& host : simgrid::s4u::Engine::get_instance()->get_all_hosts())
        {
            const std::unordered_map<std::string, std::string> * host_properties = host-> get_properties();

            if(host_properties->find("host_type")!=host_properties->end())
            {       
                std::string host_type = host->get_property("host_type");
                if (host_type.compare(selected_host_type)==0)
                {       
                    selected_host_type_array.push_back(host);
                }
            }
        }
        
        schedulingstrategy = new SchedulingHostType(&hosts_cpuavailability,selected_host_type_array );
    }

    if (use_cache)
    {
        int host_cache_memory = std::stoi(argsClass[++arg_index]);
        for (auto& host : simgrid::s4u::Engine::get_instance()->get_all_hosts())
        {
            hosts_info[host->get_name()] = new EdgeHost(host->get_name(),host_cache_memory);
        }
    }

    // Log when a tasks starts
    simgrid::s4u::Exec::on_start_cb([this](simgrid::s4u::Exec const& exec) 
    {
        std::string state = "";

        if (exec.get_state()== simgrid::s4u::Activity::State::INITED) state = "INITED";
        if (exec.get_state()== simgrid::s4u::Activity::State::STARTING) state = "STARTING";
        if (exec.get_state()== simgrid::s4u::Activity::State::STARTED) state = "STARTED";
        if (exec.get_state()== simgrid::s4u::Activity::State::FAILED) state = "FAILED";
        if (exec.get_state()== simgrid::s4u::Activity::State::CANCELED) state = "CANCELED";
        if (exec.get_state()== simgrid::s4u::Activity::State::FINISHED) state = "FINISHED";


        //XBT_INFO("#COMECOU EXEC DA TASK;%s;%f;%f;%s;%s", exec.get_cname(), exec.get_start_time(), exec.get_finish_time(),exec.get_host()->get_cname(),state.c_str());
        
    });    

    // Log when a tasks finishes
    simgrid::s4u::Exec::on_completion_cb([this](simgrid::s4u::Exec const& exec) 
    {
        std::string state = "";

        if (exec.get_state()== simgrid::s4u::Activity::State::INITED) state = "INITED";
        if (exec.get_state()== simgrid::s4u::Activity::State::STARTING) state = "STARTING";
        if (exec.get_state()== simgrid::s4u::Activity::State::STARTED) state = "STARTED";
        if (exec.get_state()== simgrid::s4u::Activity::State::FAILED) state = "FAILED";
        if (exec.get_state()== simgrid::s4u::Activity::State::CANCELED) state = "CANCELED";
        if (exec.get_state()== simgrid::s4u::Activity::State::FINISHED) state = "FINISHED";


        const int buf_size = 256;
        int nb_printed;
        (void) nb_printed; // Avoids a warning if assertions are ignored
        char * buf = static_cast<char*>(malloc(sizeof(char) * buf_size));
        snprintf(buf, buf_size,"%s,%f,%f,%s,%s\n", exec.get_cname(), exec.get_start_time(), exec.get_finish_time(),exec.get_host()->get_cname(),state.c_str());
        tasks_output->append_text(buf);
        free(buf);

        //XBT_INFO("#FE;%s;%f;%f;%s;%s", exec.get_cname(), exec.get_start_time(), exec.get_finish_time(),exec.get_host()->get_cname(),state.c_str());
        if(exec.get_finish_time()!= -1.0)
        {
          //  XBT_INFO("HANDLE TASK FINISHED;%s;%f;%f;%s;%s", exec.get_cname(), exec.get_start_time(), exec.get_finish_time(),exec.get_host()->get_cname(),state.c_str());

            this->handle_task_finished(exec); 
        }
        
    });    

    // Log when a communication finishes
    simgrid::s4u::Comm::on_completion_cb([this](simgrid::s4u::Comm const& comm) {
  //      XBT_INFO("#FC;%s;%f;%f;%s;%s\n", comm.get_cname(), comm.get_start_time(), comm.get_finish_time(),comm.get_source()->get_cname(),comm.get_destination()->get_cname());   
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
            case MESSAGE_DAG_SUBMISSION:
                handle_request_submission(static_cast<DAGOfTasks*>(message->data));        
                break;      
            case MESSAGE_STOP:
                requests_output->flush_buffer();
                tasks_output->flush_buffer();
                energy_output->flush_buffer();
                co2_output->flush_buffer();
                /*requests_output->close_buffer();
                tasks_output->close_buffer();
                energy_output->close_buffer();
                */

                break;      
            default:
                break;
        }
        message = nullptr;
    }
}

void DAGManager::handle_request_submission(DAGOfTasks* dag)
{
    requests.push_back(dag);
    requests_map[dag->get_name()] = dag;
    dag->set_submission_time(simgrid::s4u::Engine::get_clock());
    if (SCHEDULING_ALGORITHM == SCHEDULING_HEFT || SCHEDULING_ALGORITHM == SCHEDULING_GEFT)
    {
        SchedulingHEFT *casted = dynamic_cast<SchedulingHEFT*>(schedulingstrategy);
        std::vector<std::shared_ptr<SegmentTask>>  tasks = dag->get_DAG();    
        casted->create_tasks_ranking(tasks);
    }
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

    comm->set_name(id);
    comm->add_successor(task);
    comm->set_source(src_host);
    comm->set_destination(dest_host);

    // Create one additional task to simulate power consumption in the destination node 
    // for communicating
   // simgrid::s4u::ExecPtr comm_load = simgrid::s4u::this_actor::exec_init(9999000000.0 * 100000); // huge flop amount to ensure that it will run during all the migration 
    //comm_load->set_host(dest_host);
    //comm_load->start();

    comm->wait();
    // Finishes the communication task
   // comm_load->cancel();


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

    // Create one additional task to simulate power consumption in the destination node 
    // for communicating
   // simgrid::s4u::ExecPtr comm_load = simgrid::s4u::this_actor::exec_init(9999000000.0 * 100000); // huge flop amount to ensure that it will run during all the migration 
    //comm_load->set_host(src_host);
    //comm_load->start();

    comm1->wait();
    comm2->wait();
    // Create one additional task to simulate power consumption in the destination node 
    // for communicating    
    // comm_load->cancel();

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
        vector<shared_ptr<SegmentTask>> request_ready_tasks = request->get_ready_tasks_cache(task_cache,task_time_cache,cache_duration);
        for(auto& task : request_ready_tasks)
        {
            ready_tasks.push_back(task);
        }        
    }
    return ready_tasks;

}


bool sort_by_priority(shared_ptr<SegmentTask> a, shared_ptr<SegmentTask> b)
{
    return a->get_priority() > b->get_priority();
}
/**
 * Allocates the tasks to the hosts, respecting the servers computational capacity and the selected scheduling policy.
 */
void DAGManager::perform_schedule()
{   
    //Lists of tasks that are ready to be scheduled in the current instant of time
    vector<shared_ptr<SegmentTask>> ready_tasks;
    if (use_cache)
    {
        ready_tasks = get_ready_tasks_cache(); 
    }    
    else
    {
        ready_tasks = get_ready_tasks_from_requests(); 
    }
    if(SCHEDULING_ALGORITHM == SCHEDULING_HEFT || SCHEDULING_ALGORITHM == SCHEDULING_GEFT)
    {
        std::stable_sort(ready_tasks.begin(),ready_tasks.end(),sort_by_priority);
    }

    for(auto& segment : ready_tasks)
    {
        // Flag used to validate if there is need to perform communications to get its required data,
        // for example, if the parent of a task was executed in a different host, or if the tasks will
        // be executed in a host that does not have all its necessary data
        bool no_comm_needed = true;

        auto task = segment->get_exec();
        //XBT_INFO("processing task %s",task->get_cname());
        simgrid::s4u::Host* candidate_host = find_host(segment);
        if (candidate_host == nullptr)
        {
            //XBT_INFO("no host found for task %s q queria o bus stop %s",task->get_cname(),segment->get_pref_host()->get_cname());
            continue;
        }
        
        // First we validate if all the parent tasks (if any) have been executed in different hosts than the canidate host
        if (no_comm_needed)
        {            
            //XBT_INFO("VALIDATING IF NEED TO SEND DATA FROM 1 COMM FROM PARENTS");
            for(auto& parent :segment->get_parents() )
            {
                simgrid::s4u::Host* src_host;
                
                if (parent.second->get_exec()->is_assigned()) 
                {
                    src_host = parent.second->get_exec()->get_host();
                }
                else
                {

                    src_host = simgrid::s4u::Host::by_name(task_cache[parent.second->get_task_data()]);
                    

                }

                if(src_host!=candidate_host)
                {
                    //XBT_INFO("Task %s Needs to communicate from this parent %s",task->get_cname(),parent.first.c_str());
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
           /* auto it = hosts_info[candidate_host->get_name()]->cache.find(task->get_name());
            if (it != hosts_info[candidate_host->get_name()]->cache.end() && task->get_state() != simgrid::s4u::Activity::State::STARTED )
            {
                task->set_flops_amount(0.0);
            }*/
            for(auto& parent: segment->get_parents_in_cache())
            {

                std::string parent_state = "";
                if (parent.second->get_exec()->get_state()== simgrid::s4u::Activity::State::INITED) parent_state = "INITED";
                if (parent.second->get_exec()->get_state()== simgrid::s4u::Activity::State::STARTING) parent_state = "STARTING";
                if (parent.second->get_exec()->get_state()== simgrid::s4u::Activity::State::STARTED) parent_state = "STARTED";
                if (parent.second->get_exec()->get_state()== simgrid::s4u::Activity::State::FAILED) parent_state = "FAILED";
                if (parent.second->get_exec()->get_state()== simgrid::s4u::Activity::State::CANCELED) parent_state = "CANCELED";
                if (parent.second->get_exec()->get_state()== simgrid::s4u::Activity::State::FINISHED) parent_state = "FINISHED";
                //XBT_INFO("TAREFA PAI %s ta em cache e tem o state %s, entao vamos completar ela pq vai ser usada pra tarefa %s ",parent.first.c_str(),parent_state.c_str(),task->get_cname());
                
                if (parent.second->get_exec()->get_state()==simgrid::s4u::Activity::State::FINISHED || parent.second->completed())
                {
                  //  XBT_INFO("OPA A TAREFA JA TA COMPLETADA  %s ta em cache, entao vamos ignonrar ela pq vai ser usada pra tarefa %s",parent.first.c_str(),task->get_cname());
                    continue;
                }
                if(!parent.second->get_exec()->is_assigned() )
                {                
                   parent.second->get_exec()->set_host(simgrid::s4u::Host::by_name(task_cache[parent.second->get_task_data()]));
                }                

                parent.second->complete();      
                parent.second->get_exec()->unset_host();
            }     

            //XBT_INFO("#START TASK;%s;%d;%s\n",task->get_cname(), hosts_cpuavailability[candidate_host->get_name()],candidate_host->get_cname());
            task->set_host(candidate_host);   
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
            const int buf_size = 256;
            int nb_printed;
            (void) nb_printed; // Avoids a warning if assertions are ignored
            char * buf = static_cast<char*>(malloc(sizeof(char) * buf_size));
            snprintf(buf, buf_size,"%s,%f,%f,1\n", request->get_name().c_str(), request->get_submission_time(), simgrid::s4u::Engine::get_clock());
            requests_output->append_text(buf);
            free(buf);
            
            //XBT_INFO("#FR;%s;%f\n", request->get_name().c_str(), simgrid::s4u::Engine::get_clock());
            this->requests.erase(std::remove(this->requests.begin(), this->requests.end(), request), this->requests.end());
            break;
        }
    }
}   

/**
 * Finds the hosts for the task that has avaialble CPU capacity and given a scheduling policy.
*/
simgrid::s4u::Host* DAGManager::find_host(shared_ptr<SegmentTask> ready_task)
{  
    simgrid::s4u::Host*  candidate_host =   nullptr;
    // Validates if there is no host allocated yet to the task
    if (ready_task->get_allocated_host()==nullptr)
    {   

        candidate_host =    schedulingstrategy->find_host(ready_task);

        if (candidate_host == nullptr) return candidate_host;

        //XBT_INFO("#SCHEDULE;%s;%d;%s;%f\n",ready_task->get_exec()->get_cname(), hosts_cpuavailability[candidate_host->get_name()],candidate_host->get_cname(),simgrid::s4u::Engine::get_clock());
        ready_task->set_allocated_host(candidate_host);

        // These ini and end tasks are used to simulate the user 
        // sending the request and receiving the result
        if ( (ready_task->get_task_data().compare("ini")!=0 &&  ready_task->get_task_data().compare("end")!=0  ))
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


void DAGManager::update_battery_state(simgrid::s4u::Host* host)
{
    double host_energy_consumption =   Util::convert_joules_to_wh(sg_host_get_consumed_energy(host) - hosts_energy_consumption[host->get_name()]);
    double brown_energy_wh   = host_energy_consumption; 
    double energy_discharged = 0.0; 
    double renewable_power_prod =0;
    double renewable_energy_used = 0;
    double grid_energy_used =0;
    double battery_capacity = 0;
    double grid_co2 = 0;
    double battery_co2 = 0;
    double solar_co2 = 0;
    if (hosts_pvpanels.find(host->get_name())!=hosts_pvpanels.end())
    {
        renewable_power_prod = Util::convert_joules_to_wh(hosts_renewable_energy[host->get_name()]);
        // Validate if there was a overproduction of solar power (more than consumption), in this case 
        // no energy was discharged from the battery and we can charge the excess PV energy in the battery    
        if (renewable_power_prod >= host_energy_consumption)
        {
            hosts_batteries[host->get_name()]->charge(renewable_power_prod-host_energy_consumption);    
            brown_energy_wh   = 0;
            renewable_energy_used = host_energy_consumption;
        }

        // If the solar power was not enough, we need to compute the amount of energy discharged from the batteries
        else
        {
            // First we compute how much energy was consumed that did not originate from the PV panels
            brown_energy_wh = host_energy_consumption - renewable_power_prod;

            renewable_energy_used = renewable_power_prod;
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
                brown_energy_wh-=energy_discharged;
            }
            

        }

        battery_capacity = hosts_batteries[host->get_name()]->getUsableWattsHour();

    }

    grid_energy_used = brown_energy_wh;
    //XBT_INFO("#ENERGY;%f;%s;%f;%f;%f;%f",simgrid::s4u::Engine::get_clock(),host->get_cname(),host_energy_consumption,renewable_power_prod, brown_energy_wh);        
    const int buf_size = 256;
    int nb_printed;
    (void) nb_printed; // Avoids a warning if assertions are ignored
    char * buf = static_cast<char*>(malloc(sizeof(char) * buf_size));
    snprintf(buf, buf_size,"%f,%s,%f,%f,%f,%f\n",simgrid::s4u::Engine::get_clock(),host->get_cname(),host_energy_consumption,renewable_power_prod, brown_energy_wh,battery_capacity);
    energy_output->append_text(buf);
    free(buf);

    const std::unordered_map<std::string, std::string> * host_properties = host-> get_properties();
    if(host_properties->find("host_type")!=host_properties->end())
    {       
        std::string host_type = host->get_property("host_type");
        if (host_type.compare("cloud_host")==0)
        {
            grid_co2 = grid_energy_used * cloud_dc_power_co2->get_current_co2_eq(simgrid::s4u::Engine::get_clock()) *1/1000; 
        }
        else
        {
            grid_co2 = grid_energy_used * local_grid_power_co2->get_current_co2_eq(simgrid::s4u::Engine::get_clock())*1/1000;
        }
    }
    
    battery_co2 = energy_discharged * battery_power_co2*1/1000;
    solar_co2 = renewable_energy_used * pv_panel_power_co2*1/1000;

    buf = static_cast<char*>(malloc(sizeof(char) * buf_size));
    //XBT_INFO("#CO2;%f;%s;%f;%f;%f;%f",simgrid::s4u::Engine::get_clock(),host->get_cname(),(grid_co2+battery_co2+solar_co2),grid_co2,battery_co2,solar_co2);

    snprintf(buf, buf_size,"%f;%s;%f;%f;%f;%f\n",simgrid::s4u::Engine::get_clock(),host->get_cname(),(grid_co2+battery_co2+solar_co2),grid_co2,battery_co2,solar_co2);
    co2_output->append_text(buf);
    free(buf);
 
}

/**
 * Updates the information about renewable energy availability for all hosts,
 * considering both the PV panels and the batteries
*/
void DAGManager::update_hosts_energy_information()
{
    sg_host_energy_update_all();
    for (auto& host : simgrid::s4u::Engine::get_instance()->get_all_hosts())
    { 
        //First, update the energy in the batteries. For example, charge if there is overprduction during the previous time slot.
        update_battery_state(host);

        //Then, update the information about the solar panel energy production
        if (hosts_pvpanels.find(host->get_name())!=hosts_pvpanels.end())
        {
            hosts_renewable_energy[host->get_name()] =  Util::convert_wh_to_joules(hosts_pvpanels[host->get_name()]->get_current_green_power_production(simgrid::s4u::Engine::get_clock()));
        }
        
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

void DAGManager::update_valid_requests(){
    std::vector<DAGOfTasks*>::iterator it = requests.begin();

    while(it != requests.end()) {

        if( ( simgrid::s4u::Engine::get_clock() -   (*it)->get_submission_time()   )  > max_request_run_time) {

            const int buf_size = 256;
            int nb_printed;
            (void) nb_printed; // Avoids a warning if assertions are ignored
            char * buf = static_cast<char*>(malloc(sizeof(char) * buf_size));
            snprintf(buf, buf_size,"%s,%f,%f,0\n", (*it)->get_name().c_str(), (*it)->get_submission_time(), simgrid::s4u::Engine::get_clock());
            requests_output->append_text(buf);
            free(buf);
            it = requests.erase(it);

        }
        else ++it;
    }
}

void DAGManager::add_task_in_cache(std::string & task_name, const std::string& host_name)
{
    simgrid::s4u::Host * host = simgrid::s4u::Host::by_name(host_name);
    int timeslot = int (simgrid::s4u::Engine::get_clock()/cache_duration);
    bool host_has_free_memory = hosts_info[host_name]->has_free_memory_in_cache();

    if (!host_has_free_memory)
    {
        hosts_info[host_name]->expire_tasks(timeslot);
        host_has_free_memory = hosts_info[host_name]->has_free_memory_in_cache();
    }

    if (host_has_free_memory)
    {
        task_cache[task_name] = host_name;
        task_time_cache[task_name] = timeslot;
        host_cache_mem_used[host_name] = 1 + host_cache_mem_used[host_name];        
        hosts_info[host_name]->add_task_in_cache(task_name, timeslot);
        cache_of_tasks[task_name].push_back(host_name);
    }    
}

#include "SchedulingHEFT.hpp"
#include <bits/stdc++.h>
#include <boost/algorithm/string.hpp>
#include <memory>

XBT_LOG_NEW_DEFAULT_CATEGORY(SchedulingHEFT, "SchedulingHEFT category");

double SchedulingHEFT::getNetworkLatencyVivaldi(double x1, double y1, double z1, double x2, double y2, double z2)
{
    return sqrt( pow(x1-x2,2) + pow( y1-y2,2))  + z1 + z2;
    
}


SchedulingHEFT::SchedulingHEFT(map<string, int> *hosts_cpu_availability,std::unordered_map<std::string, string> * cache)
{
    this->hosts_cpuavailability = hosts_cpu_availability;
    this->task_cache = cache;
}



simgrid::s4u::Host* SchedulingHEFT::find_host(shared_ptr<SegmentTask> ready_task,std::vector<simgrid::s4u::Host *> hosts)
{
    simgrid::s4u::Host* selected_host = nullptr;
    double min_finish_time = 999999999.9;

    for(auto candidate_host :hosts )
    {        
        if((*hosts_cpuavailability)[candidate_host->get_name()]==0) continue;
        //XBT_INFO("ANALIZANDO O HOST %s pra tarefa %s",candidate_host->get_cname(),ready_task->get_exec()->get_cname());

        double compute_time = ready_task->get_exec()->get_remaining() / candidate_host->get_speed();
        double comm_time = 0;

        //First, the communication from the parents
        double max_parent_comms =0;
        for(auto& parent :ready_task->get_parents() )
        {
            simgrid::s4u::Host* src_host = nullptr;;
            if (parent.second->get_exec()->is_assigned()) 
            {
                src_host = parent.second->get_exec()->get_host();
            }
            else
            {
                vector<string> result;
                boost::split(result, parent.first, boost::is_any_of("-"));
                std::string parent_name = result[2];
                src_host = simgrid::s4u::Host::by_name((*task_cache)[parent_name]);
               
            }


            std::string task_state = "";

            if (ready_task->get_exec()->get_state()== simgrid::s4u::Activity::State::INITED) task_state = "INITED";
            if (ready_task->get_exec()->get_state()== simgrid::s4u::Activity::State::STARTING) task_state = "STARTING";
            if (ready_task->get_exec()->get_state()== simgrid::s4u::Activity::State::STARTED) task_state = "STARTED";
            if (ready_task->get_exec()->get_state()== simgrid::s4u::Activity::State::FAILED) task_state = "FAILED";
            if (ready_task->get_exec()->get_state()== simgrid::s4u::Activity::State::CANCELED) task_state = "CANCELED";
            if (ready_task->get_exec()->get_state()== simgrid::s4u::Activity::State::FINISHED) task_state = "FINISHED";

            std::string parent_task_state = "";
            if (parent.second->get_exec()->get_state()== simgrid::s4u::Activity::State::INITED) parent_task_state = "INITED";
            if (parent.second->get_exec()->get_state()== simgrid::s4u::Activity::State::STARTING) parent_task_state = "STARTING";
            if (parent.second->get_exec()->get_state()== simgrid::s4u::Activity::State::STARTED) parent_task_state = "STARTED";
            if (parent.second->get_exec()->get_state()== simgrid::s4u::Activity::State::FAILED) parent_task_state = "FAILED";
            if (parent.second->get_exec()->get_state()== simgrid::s4u::Activity::State::CANCELED) parent_task_state = "CANCELED";
            if (parent.second->get_exec()->get_state()== simgrid::s4u::Activity::State::FINISHED) parent_task_state = "FINISHED";

         
                        
            double parent_latency = 0.0;    

            const std::unordered_map<std::string, std::string> * host_properties = src_host-> get_properties();

            if(host_properties->find("lat")!=host_properties->end())
            {       
                double parent_lat =  std::stod(src_host->get_property("lat"));
                double parent_long = std::stod(src_host->get_property("long"));
                double parent_z_coord = std::stod(src_host->get_property("z_coord"));
                                
                double cand_host_lat = std::stod(candidate_host->get_property("lat"));
                double cand_host_long = std::stod(candidate_host ->get_property("long"));
                double cand_host_z_coord = std::stod(candidate_host->get_property("z_coord"));

                max_parent_comms = getNetworkLatencyVivaldi(parent_long,parent_lat,parent_z_coord,cand_host_long,cand_host_lat,cand_host_z_coord);

            }
            else
            {
                std::vector<simgrid::s4u::Link *> links;                            
                src_host->route_to(candidate_host,links,nullptr);    

                for(auto link : links)
                {
                    parent_latency+=link->get_latency();
                }                 

                if (parent_latency > max_parent_comms)
                {
                    max_parent_comms = parent_latency;
                }                
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


simgrid::s4u::Host* SchedulingHEFT::find_host(shared_ptr<SegmentTask> ready_task)
{
    std::vector<simgrid::s4u::Host *> hosts = simgrid::s4u::Engine::get_instance()->get_all_hosts();
    return this->find_host(ready_task,hosts);
}

void SchedulingHEFT::create_task_ranking_recursive(simgrid::s4u::ActivityPtr task,std::map<std::string,int>& rank)
{
    
    // XBT_INFO("processing task %s",task->get_cname());

    if (task->get_successors().empty()){
        rank[task->get_name()] = 1;
        return;
    }

    int max_rank = 0;
    for (auto sucessor : task->get_successors())
    {
        int temp = 0;
        create_task_ranking_recursive(sucessor,rank);
        temp = 1 + rank[sucessor->get_name()];
        if (temp > max_rank)
        {
            max_rank = temp;
        }
    }
    rank[task->get_name()] = max_rank;
}

void SchedulingHEFT::create_tasks_ranking(std::vector<std::shared_ptr<SegmentTask>> &tasks)
{
    std::map<std::string,int> task_rank;
    create_task_ranking_recursive( (*tasks.begin())->get_exec(),task_rank);
    for (auto task: tasks)
    {
        task->set_priority(task_rank[task->get_exec()->get_name()]);
    }    
}
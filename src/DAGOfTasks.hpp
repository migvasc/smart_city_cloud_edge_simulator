
#ifndef DAGOFTASKS_HPP
#define DAGOFTASKS_HPP

#include <string>
#include <unordered_map>
#include "robin_hood.h"
#include "simgrid/s4u.hpp"
#include <nlohmann/json.hpp>
#include <sstream>
#include <fstream>
#include "SegmentTask.hpp"

using namespace std; 


class DAGOfTasks {


private:
    std::string name;
    simgrid::s4u::ExecPtr last_exec;
    std::vector<shared_ptr<SegmentTask>> dag;
    void create_DAG_from_JSON(const std::string& filename);
    // To control whether or not we can start the computations of the request
    // that is the path of the route
    bool ini_task_finished = false;

public:
    explicit DAGOfTasks(std::string& input_file);
    std::vector<shared_ptr<SegmentTask>> get_DAG();
    std::vector<shared_ptr<SegmentTask>>   get_ready_tasks();
    std::vector<shared_ptr<SegmentTask>>   get_ready_tasks_cache( unordered_map<string,string>& cache,unordered_map<string,int>& time_cache, int cache_duration);
    
    simgrid::s4u::ExecPtr get_last_exec();
    std::string get_name();
    void request_received();
    bool can_start_computations();
};
#endif
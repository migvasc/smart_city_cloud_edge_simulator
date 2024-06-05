
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
    std::vector<SegmentTask> dag;
    void create_DAG_from_JSON(const std::string& filename);

public:
    explicit DAGOfTasks(std::string& input_file);
    std::vector<SegmentTask> get_DAG();
    std::vector<SegmentTask*>   get_ready_tasks();
    SegmentTask*   get_one_ready_task();
    simgrid::s4u::ExecPtr get_last_exec();
    std::string get_name();
};
#endif
#ifndef DAGDISPATCHER_HPP
#define DAGDISPATCHER_HPP

#include <simgrid/s4u.hpp>
#include "xbt/replay.hpp"
#include "xbt/str.h"

class DAGDispatcher {

public:    

    explicit DAGDispatcher(std::vector<std::string> args);
    void operator()();
    static void send_DAG_of_tasks(simgrid::xbt::ReplayAction& action);
    static void stop(simgrid::xbt::ReplayAction& action);
    static void wait_for(double time);
    
};

#endif
#ifndef SCHEDULINGSTRATEGY_HPP
#define SCHEDULINGSTRATEGY_HPP

#include <simgrid/s4u.hpp>
#include <memory>
#include <stdlib.h>
#include "SegmentTask.hpp"
using namespace std;
class SchedulingStrategy{

public:
    virtual simgrid::s4u::Host* find_host(shared_ptr<SegmentTask> ready_task){return nullptr;};

};
#endif
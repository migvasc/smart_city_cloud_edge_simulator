#ifndef SEGMENTTASK_HPP
#define SEGMENTTASK_HPP
#include <iostream>
#include <vector>
#include <memory>
#include <simgrid/s4u.hpp>

class SegmentTask
{


  private:
    simgrid::s4u::ExecPtr ptr;
    simgrid::s4u::Host* pref_host=nullptr;   
    simgrid::s4u::Host* allocated_host=nullptr;
    bool data_in_cache = false;
    std::vector<std::weak_ptr < SegmentTask>> parents;
    std::vector<std::weak_ptr < SegmentTask>> parents_with_cache;


  public:
    bool is_assigned();
    simgrid::s4u::Host* get_pref_host();
    void start();
    simgrid::s4u::ExecPtr get_exec();
    void set_exec(simgrid::s4u::ExecPtr exec);
    void set_pref_host(simgrid::s4u::Host* host);
    void add_parent(std::shared_ptr<SegmentTask> parent);
    std::vector<std::shared_ptr<SegmentTask>> get_parents() const;
    void clear_parents();
    std::vector<simgrid::s4u::CommPtr> parent_coms;
    void set_allocated_host(simgrid::s4u::Host* host);
    simgrid::s4u::Host* get_allocated_host();
    bool has_cache();
    void set_cache();
    bool is_ready_for_execution(std::unordered_map<std::string,std::string>& cache,std::unordered_map<std::string,int>& time_cache );

};

#endif
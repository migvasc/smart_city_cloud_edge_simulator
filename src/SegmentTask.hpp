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
    std::map<std::string,std::weak_ptr<SegmentTask>> parents;
    std::map<std::string,std::weak_ptr<SegmentTask>> parents_in_cache;
    bool task_completed = false;
    int priority =0;
    std::string task_data;

  public:
    bool is_assigned();
    simgrid::s4u::Host* get_pref_host();
    void start();
    simgrid::s4u::ExecPtr get_exec();
    void set_exec(simgrid::s4u::ExecPtr exec);
    void set_pref_host(simgrid::s4u::Host* host);
    void add_parent(const std::shared_ptr<SegmentTask> parent);
    void add_parent_in_cache(const std::shared_ptr<SegmentTask> parent);
    std::map<std::string,std::shared_ptr<SegmentTask>> get_parents() const;
    std::map<std::string,std::shared_ptr<SegmentTask>> get_parents_in_cache() const;
    void clear_parents_in_cache();
    void clear_parents();
    std::vector<simgrid::s4u::CommPtr> parent_coms;
    void set_allocated_host(simgrid::s4u::Host* host);
    simgrid::s4u::Host* get_allocated_host();
    bool has_cache();
    void set_cache();
    bool is_ready_for_execution( );
    bool is_parent_in_cache(std::string parent_id);
    bool completed();
    void complete();
    void set_priority(int value);
    int get_priority();
    bool operator < (const SegmentTask& str) const
    {
        return (priority < str.priority);
    }
    bool operator > (const SegmentTask& str) const
    {
        return (priority > str.priority);
    }
    const std::string get_task_data();
    void set_task_data(const std::string & value);

};

#endif
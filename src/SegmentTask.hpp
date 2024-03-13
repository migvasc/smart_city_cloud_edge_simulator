#ifndef SEGMENTTASK_HPP
#define SEGMENTTASK_HPP

#include <simgrid/s4u.hpp>


class SegmentTask
{


  private:
    simgrid::s4u::ExecPtr ptr;
    simgrid::s4u::Host* pref_host=nullptr;
    std::vector<simgrid::s4u::ExecPtr> parents;

  public:
    bool dependencies_solved();
    bool is_assigned();
    simgrid::s4u::Host* get_pref_host();
    void start();
    simgrid::s4u::ExecPtr get_exec();
    void set_exec(simgrid::s4u::ExecPtr exec);
    void set_pref_host(simgrid::s4u::Host* host);
    void add_parent(simgrid::s4u::ExecPtr parent);
    std::vector<simgrid::s4u::ExecPtr> get_parents();
    void clear_parents();
    std::vector<simgrid::s4u::CommPtr> parent_coms;

};

#endif
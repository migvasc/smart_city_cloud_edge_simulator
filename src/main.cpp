/* Copyright (c) 2010-2020. The SimGrid Team. All rights reserved.          */
/* This program is free software; you can redistribute it and/or modify it
 * under the terms of the license (GNU LGPL) which comes with this package. */

#include <simgrid/s4u.hpp>
#include "simgrid/plugins/energy.h"
#include "xbt/replay.hpp"
#include "xbt/str.h"
#include <boost/algorithm/string/join.hpp>
#include "DAGDispatcher.hpp"
#include "DAGManager.hpp"
#include "simgrid/plugins/live_migration.h"
#include <chrono>
#include <memory>


XBT_LOG_NEW_DEFAULT_CATEGORY(simulationMain, "Main Logger");

int main(int argc, char* argv[])
{
  auto start = std::chrono::high_resolution_clock::now();
  /*Inits the energy plugin*/   
  sg_vm_live_migration_plugin_init();
  sg_host_energy_plugin_init();

  simgrid::s4u::Engine e(&argc, argv);
  /* Register the classes representing the actors */
  //e.register_actor<VMDispatcher>("vmdispatcher");/* Submits VMs */
  e.register_actor<DAGDispatcher>("dagdispatcher");/* Submits DAGs */
  e.register_actor<DAGManager>("dagmanager");/* Handles VM requests and schedules them */
  /* Load the platform description and how it will be deployed */
  e.load_platform(argv[1]);
  e.load_deployment(argv[2]);
 /* argv[3] is the trace file */
  if (argv[3]) {
    /* loads the trace file */
    xbt_replay_set_tracefile(argv[3]);
    //simgrid::xbt::action_fs = new std::ifstream(argv[3], std::ifstream::in);
  }  
  XBT_INFO("#Simulation will be executed using platform: %s; deployment file: %s; and trace file: %s",argv[1],argv[2],argv[3]);    

  /* Register the class and methods that will read the trace file */
  xbt_replay_action_register("send_DAG", DAGDispatcher::send_DAG_of_tasks);
  xbt_replay_action_register("stop", DAGDispatcher::stop);
  xbt_replay_action_register("create_checkpoint", DAGDispatcher::create_checkpoint);
  e.set_config("network/latency-factor:1");
  /* Run the simulation */
  e.run();


  auto end = std::chrono::high_resolution_clock::now();
  auto time = std::chrono::duration_cast<std::chrono::milliseconds>( end - start ).count();
  XBT_INFO("Simulation is over");    
  XBT_INFO("#TOTAL;%ld",time);
  return 0;
}
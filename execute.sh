#!/bin/bash
#rm -rf output/output_baseline_min
#mkdir output/output_baseline_min
#clear && clear && echo 'running baseline min...' && build/src/simulationmain input/platform/vieille-toulouse_cloud.xml input/deployment/deploy_baseline.xml input/workload/baseline_makespan.txt --log=root.app:file:output/output_baseline_min/logfile.log
#python3 util/process_log.py input/workload/baseline_makespan.txt output/output_baseline_min/logfile.log output/output_baseline_min

#rm -rf output/output_baseline_30k
#mkdir output/output_baseline_30k
#clear && clear && echo 'running baseline cloud with idle...' && build/src/simulationmain input/platform/vieille-toulouse_cloud.xml input/deployment/deploy_baseline.xml input/workload/vieille-toulouse_30k.txt --log=root.app:file:output/output_baseline_30k/logfile.log
#python3 util/process_log.py input/workload/vieille-toulouse_30k.txt output/output_baseline_30k/logfile.log output/output_baseline_30k

#rm -rf output/output_GEFT_30k
#mkdir output/output_GEFT_30k
#clear && clear && echo 'running GEFT cloud with idle...' && build/src/simulationmain input/platform/vieille-toulouse_cloud.xml input/deployment/deploy_GEFT.xml input/workload/vieille-toulouse_30k.txt --log=root.app:file:output/output_GEFT_30k/logfile.log
#python3 util/process_log.py input/workload/vieille-toulouse_30k.txt output/output_GEFT_30k/logfile.log output/output_GEFT_30k

#rm -rf output/output_baseline_30k_no_idle
#mkdir output/output_baseline_30k_no_idle
#clear && clear && echo 'running baseline cloud no idle...' && build/src/simulationmain input/platform/vieille-toulouse_cloud_no_idle_power.xml input/deployment/deploy_baseline.xml input/workload/vieille-toulouse_30k.txt --log=root.app:file:output/output_baseline_30k_no_idle/logfile.log
#python3 util/process_log.py input/workload/vieille-toulouse_30k.txt output/output_baseline_30k_no_idle/logfile.log output/output_baseline_30k_no_idle

#rm -rf output/output_GEFT_30k_no_idle
#mkdir output/output_GEFT_30k_no_idle
#clear && clear && echo 'running GEFT cloud no idle...' && build/src/simulationmain input/platform/vieille-toulouse_cloud_no_idle_power.xml input/deployment/deploy_GEFT.xml input/workload/vieille-toulouse_30k.txt --log=root.app:file:output/output_GEFT_30k_no_idle/logfile.log
#python3 util/process_log.py input/workload/vieille-toulouse_30k.txt output/output_GEFT_30k_no_idle/logfile.log output/output_GEFT_30k_no_idle

#rm -rf output/output_solar
#mkdir output/output_solar
#clear && clear && build/src/simulationmain input/platform/vieille-toulouse_cloud.xml input/deployment/deploy_solar.xml input/workload/vieille-toulouse_30k.txt --log=root.app:file:output/output_solar/logfile.log
#python3 util/process_log.py input/workload/vieille-toulouse_30k.txt output/output_solar/logfile.log output/output_solar

#rm -rf output/output_bestfit
#mkdir output/output_bestfit
#clear && clear && build/src/simulationmain input/platform/vieille-toulouse_cloud.xml input/deployment/deploy_bestfit.xml input/workload/vieille-toulouse_30k.txt --log=root.app:file:output/output_bestfit/logfile.log
#python3 util/process_log.py input/workload/vieille-toulouse_30k.txt output/output_bestfit/logfile.log output/output_bestfit

#rm -rf output/output_HEFT_30k
#mkdir output/output_HEFT_30k
#clear && clear && build/src/simulationmain input/platform/vieille-toulouse_cloud.xml input/deployment/deploy_HEFT.xml input/workload/vieille-toulouse_30k.txt --log=root.app:file:output/output_HEFT_30k/logfile.log




#python3 util/process_log.py input/workload/vieille-toulouse_30k.txt output/output_HEFT_30k/logfile.log output/output_HEFT_30k
#rm -rf output/output_HEFT_30k_no_idle
#mkdir output/output_HEFT_30k_no_idle
#clear && clear && echo 'running HEFT cloud no idle...' && build/src/simulationmain input/platform/vieille-toulouse_cloud_no_idle_power.xml input/deployment/deploy_HEFT.xml input/workload/vieille-toulouse_30k.txt --log=root.app:file:output/output_HEFT_30k_no_idle/logfile.log
#python3 util/process_log.py input/workload/vieille-toulouse_30k.txt output/output_HEFT_30k_no_idle/logfile.log output/output_HEFT_30k_no_idle

#rm -rf output/output_offload_cloud
#mkdir output/output_offload_cloud
#clear && clear && echo 'running all to cloud .. ' && build/src/simulationmain input/platform/vieille-toulouse_cloud.xml input/deployment/deploy_all_to_cloud.xml input/workload/dag-test.txt --log=root.app:file:output/output_offload_cloud/logfile.log
#python3 util/process_log.py input/workload/dag-test.txt output/output_offload_cloud/logfile.log output/output_offload_cloud

#rm -rf output/output_test_brown
#mkdir output/output_test_brown
#clear && clear && echo 'running GEFT cloud no idle...' && build/src/simulationmain input/platform/vieille-toulouse_cloud_no_idle_power.xml input/deployment/deploy_GEFT.xml input/workload/test-energy.txt --log=root.app:file:output/output_test_brown/logfile.log
#python3 util/process_log.py input/workload/test-energy.txt output/output_test_brown/logfile.log output/output_test_brown

#rm -rf output/output_HEFT_min
#mkdir output/output_HEFT_min
#clear && clear && echo 'running HEFT min...' && build/src/simulationmain input/platform/vieille-toulouse_cloud.xml input/deployment/deploy_HEFT.xml input/workload/baseline_makespan.txt --log=root.app:file:output/output_HEFT_min/logfile.log
#python3 util/process_log.py input/workload/baseline_makespan.txt output/output_HEFT_min/logfile.log output/output_HEFT_min

#rm -rf output/output_baseline_06_to_07
#mkdir output/output_baseline_06_to_07
#clear && clear && echo 'running baseline 06 to 07...' && build/src/simulationmain input/platform/vieille-toulouse_cloud.xml input/deployment/deploy_baseline.xml input/workload/vieille-toulouse_06h_to_07h.txt --log=root.app:file:output/output_baseline_06_to_07/logfile.log
#python3 util/process_log.py input/workload/vieille-toulouse_06h_to_07h.txt output/output_baseline_06_to_07/logfile.log output/output_baseline_06_to_07

#rm -rf output/output_GEFT_06_to_07
#mkdir output/output_GEFT_06_to_07
#clear && clear && echo 'running baseline 06 to 07...' && build/src/simulationmain input/platform/vieille-toulouse_cloud.xml input/deployment/deploy_GEFT.xml input/workload/vieille-toulouse_06h_to_07h.txt --log=root.app:file:output/output_GEFT_06_to_07/logfile.log
#python3 util/process_log.py input/workload/vieille-toulouse_06h_to_07h.txt output/output_GEFT_06_to_07/logfile.log output/output_GEFT_06_to_07

#rm -rf output/output_HEFT_30k_cloud_edge
#mkdir output/output_HEFT_30k_cloud_edge
#clear && clear && build/src/simulationmain input/platform/vieille-toulouse_cloud_edge.xml input/deployment/deploy_HEFT.xml input/workload/vieille-toulouse_30k.txt  --log=root.app:file:output/output_HEFT_30k_cloud_edge/logfile.log
#python3 util/process_log.py input/workload/vieille-toulouse_30k.txt output/output_HEFT_30k_cloud_edge/logfile.log output/output_HEFT_30k_cloud_edge

#rm -rf output/output_HEFT_30k_distance
#mkdir output/output_HEFT_30k_distance
#clear && clear && build/src/simulationmain input/platform/platform_latency.xml input/deployment/deploy_HEFT_many_solar_data.xml input/workload/vieille-toulouse_30k.txt  --log=root.app:file:output/output_HEFT_30k_distance/logfile.log
#python3 util/process_log.py input/workload/vieille-toulouse_30k.txt output/output_HEFT_30k_distance/logfile.log output/output_HEFT_30k_distance

#rm -rf output/output_baseline_30k_distance
#mkdir output/output_baseline_30k_distance
#clear && clear && build/src/simulationmain input/platform/platform_latency.xml input/deployment/deploy_baseline_many_solar_data.xml input/workload/vieille-toulouse_30k.txt  --log=root.app:file:output/output_baseline_30k_distance/logfile.log
#python3 util/process_log.py input/workload/vieille-toulouse_30k.txt output/output_baseline_30k_distance/logfile.log output/output_baseline_30k_distance

#rm -rf output/output_baseline_30k_on_off_no_cloud
#mkdir output/output_baseline_30k_on_off_no_cloud
#clear && clear && build/src/simulationmain input/platform/vieille-toulouse_on_off.xml input/deployment/deploy_baseline_on_off.xml input/workload/vieille-toulouse_30k.txt  --log=root.app:file:output/output_baseline_30k_on_off_no_cloud/logfile.log
#python3 util/process_log.py input/workload/vieille-toulouse_30k.txt output/output_baseline_30k_on_off_no_cloud/logfile.log output/output_baseline_30k_on_off_no_cloud

#rm -rf output/output_HEFT_30k_no_cloud_no_lat
#mkdir output/output_HEFT_30k_no_cloud_no_lat
#clear && clear && build/src/simulationmain input/platform/vieille-toulouse_no_lat.xml input/deployment/deploy_HEFT.xml input/workload/vieille-toulouse_06h_to_07h.txt --log=root.app:file:output/output_HEFT_30k_no_cloud_no_lat/logfile.log
#python3 util/process_log.py input/workload/vieille-toulouse_30k.txt output/output_HEFT_30k_no_cloud_no_lat/logfile.log output/output_HEFT_30k_no_cloud_no_lat

#rm -rf output/output_alltoedge_30k_no_lat
#mkdir output/output_alltoedge_30k_no_lat
#clear && clear && build/src/simulationmain input/platform/vieille-toulouse_cloud_edge_no_power_from_edge_no_lat.xml input/deployment/deploy_all_to_edge.xml input/workload/vieille-toulouse_06h_to_07h.txt --log=root.app:file:output/output_alltoedge_30k_no_lat/logfile.log
#python3 util/process_log.py input/workload/vieille-toulouse_30k.txt output/output_alltoedge_30k_no_lat/logfile.log output/output_alltoedge_30k_no_lat


#rm -rf output/output_toulouse_30k_baseline
#mkdir output/output_toulouse_30k_baseline
#clear && clear && build/src/simulationmain input/platform/platform_latency.xml input/deployment/deploy_baseline_many_solar_data.xml input/workload/toulouse_30k.txt --log=root.app:file:output/output_toulouse_30k_baseline/logfile.log
#python3 util/process_log.py input/workload/toulouse_30k.txt output/output_toulouse_30k_baseline/logfile.log output/output_toulouse_30k_baseline

#rm -rf output/output_baseline_30k_no_extra_logs
#mkdir output/output_baseline_30k_no_extra_logs
#clear && clear && echo 'running baseline cloud with idle...' && build/src/simulationmain input/platform/vieille-toulouse_cloud.xml input/deployment/deploy_baseline.xml input/workload/vieille-toulouse_30k.txt --log=root.app:file:output/output_baseline_30k_no_extra_logs/logfile.log
#
##python3 util/process_log.py input/workload/vieille-toulouse_30k.txt output/output_baseline_30k_no_extra_logs/logfile.log output/output_baseline_30k_no_extra_logs
#
#rm -rf output/output_GEFT_30k_no_extra_logs
#mkdir output/output_GEFT_30k_no_extra_logs
#clear && clear && echo 'running GEFT cloud with idle...' && build/src/simulationmain input/platform/vieille-toulouse_cloud.xml input/deployment/deploy_GEFT.xml input/workload/vieille-toulouse_30k.txt --log=root.app:file:output/output_GEFT_30k_no_extra_logs/logfile.log
#
#python3 util/process_log.py input/workload/vieille-toulouse_30k.txt output/output_GEFT_30k_no_extra_logs/logfile.log output/output_GEFT_30k_no_extra_logs
#
#rm -rf output/output_HEFT_30k_no_extra_logs
#mkdir output/output_HEFT_30k_no_extra_logs
#clear && clear && build/src/simulationmain input/platform/vieille-toulouse_cloud.xml input/deployment/deploy_HEFT.xml input/workload/vieille-toulouse_30k.txt --log=root.app:file:output/output_HEFT_30k_no_extra_logs/logfile.log

#rm -rf output/output_toulouse_6_7_baseline
#mkdir output/output_toulouse_6_7_baseline
#clear && clear && build/src/simulationmain input/platform/platform_latency_toulouse_vivaldi.xml input/deployment/deploy_baseline_many_solar_data.xml input/workload/toulouse_06_07.txt --log=root.app:file:output/output_toulouse_6_7_baseline/logfile.log
#python3 util/process_log.py input/workload/toulouse_30k.txt output/output_toulouse_6_7_baseline/logfile.log output/output_toulouse_6_7_baseline

#rm -rf output/output_baseline_30k_vivaldi
#mkdir output/output_baseline_30k_vivaldi
#clear && clear && echo 'running baseline cloud with idle...' && build/src/simulationmain input/platform/platform_latency_vivaldi.xml input/deployment/deploy_baseline.xml input/workload/vieille-toulouse_30k.txt --log=root.app:file:output/output_baseline_30k_vivaldi/logfile.log
#python3 util/process_log.py input/workload/vieille-toulouse_30k.txt output/output_baseline_30k_vivaldi/logfile.log output/output_baseline_30k_vivaldi

#rm -rf output/output_baseline_vieille-toulouse_vivaldi
#mkdir output/output_baseline_vieille-toulouse_vivaldi
#clear && clear && echo 'running baseline cloud with idle...' && build/src/simulationmain input/platform/platform_latency_vieille-toulouse_vivaldi_no_edge.xml input/deployment/deploy_baseline.xml input/workload/vieille-toulouse_30k.txt --log=root.app:file:output/output_baseline_vieille-toulouse_vivaldi/logfile.log
#python3 util/process_log.py input/workload/vieille-toulouse_30k.txt output/output_baseline_vieille-toulouse_vivaldi/logfile.log output/output_baseline_vieille-toulouse_vivaldi
#
#rm -rf output/output_alltoedge_30k_vivaldi
#mkdir output/output_alltoedge_30k_vivaldi
#clear && clear && build/src/simulationmain input/platform/platform_latency_vieille-toulouse_vivaldi.xml input/deployment/deploy_all_to_edge.xml input/workload/vieille-toulouse_30k.txt --log=root.app:file:output/output_alltoedge_30k_vivaldi/logfile.log
#python3 util/process_log.py input/workload/vieille-toulouse_30k.txt output/output_alltoedge_30k_vivaldi/logfile.log output/output_alltoedge_30k_vivaldi

#rm -rf output/output_baseline_30k_1_task_per_req
#mkdir output/output_baseline_30k_1_task_per_req
#clear && clear && echo 'running baseline 1 task per req...' && build/src/simulationmain input/platform/vieille-toulouse_cloud.xml input/deployment/deploy_baseline.xml input/workload/vieille-toulouse_30k.txt --log=root.app:file:output/output_baseline_30k_1_task_per_req/logfile.log
#python3 util/process_log.py input/workload/vieille-toulouse_30k.txt output/output_baseline_30k_1_task_per_req/logfile.log output/output_baseline_30k_1_task_per_req

#rm -rf output/output_alltoedge_30k_1_to_1
#mkdir output/output_alltoedge_30k_1_to_1
#clear && clear && build/src/simulationmain input/platform/vieille-toulouse_cloud_edge_no_power_from_edge_1_to_1.xml input/deployment/deploy_all_to_edge.xml input/workload/vieille-toulouse_30k.txt --log=root.app:file:output/output_alltoedge_30k_1_to_1/logfile.log
#python3 util/process_log.py input/workload/vieille-toulouse_30k.txt output/output_alltoedge_30k_1_to_1/logfile.log output/output_alltoedge_30k_1_to_1
#
#rm -rf output/output_HEFT_30k_vivaldi
#mkdir output/output_HEFT_30k_vivaldi
#clear && clear && build/src/simulationmain input/platform/platform_latency_vieille-toulouse_vivaldi_cloud.xml input/deployment/deploy_HEFT.xml input/workload/vieille-toulouse_30k.txt --log=root.app:file:output/output_HEFT_30k_vivaldi/logfile.log
#python3 util/process_log.py input/workload/vieille-toulouse_30k.txt output/output_HEFT_30k_vivaldi/logfile.log output/output_HEFT_30k_vivaldi
#
#rm -rf output/output_GEFT_30k_vivaldi
#mkdir output/output_GEFT_30k_vivaldi
#clear && clear && echo 'running GEFT cloud with idle...' && build/src/simulationmain input/platform/platform_latency_vieille-toulouse_vivaldi_cloud input/deployment/deploy_GEFT.xml input/workload/vieille-toulouse_30k.txt --log=root.app:file:output/output_GEFT_30k_vivaldi/logfile.log
#python3 util/process_log.py input/workload/vieille-toulouse_30k.txt output/output_GEFT_30k_vivaldi/logfile.log output/output_GEFT_30k_vivaldi

#$rm -rf output/output_baseline_vieille-toulouse_vivaldi_cache
#$mkdir  output/output_baseline_vieille-toulouse_vivaldi_cache
#clear && clear && echo 'running baseline vivaldi with cache...' && build/src/simulationmain input/platform/platform_latency_vieille-toulouse_vivaldi_no_edge.xml input/deployment/deploy_baseline_cache.xml input/workload/vieille-toulouse_12h_to_13h.txt --log=root.app:file:output/output_baseline_vieille-toulouse_vivaldi_cache/logfile.log
#$python3 util/process_log.py input/workload/vieille-toulouse_30k.txt output/output_baseline_vieille-toulouse_vivaldi_cache/logfile.log output/output_baseline_vieille-toulouse_vivaldi_cache

#clear && clear && echo 'results of comparing requests from both algorithms:' &&  python3 util/compare_requests.py

#rm -rf output/output_baseline_vieille-toulouse_vivaldi_cache_map_no_idle
#mkdir  output/output_baseline_vieille-toulouse_vivaldi_cache_map_no_idle
#clear && clear && echo 'running baseline vivaldi with cache map structure...' && build/src/simulationmain input/platform/platform_latency_vieille-toulouse_vivaldi_no_edge_no_idle.xml input/deployment/deploy_baseline_cache.xml input/workload/vieille-toulouse_30k.txt --log=root.app:file:output/output_baseline_vieille-toulouse_vivaldi_cache_map_no_idle/logfile.log
#python3 util/process_log.py input/workload/vieille-toulouse_30k.txt output/output_baseline_vieille-toulouse_vivaldi_cache_map_no_idle/logfile.log output/output_baseline_vieille-toulouse_vivaldi_cache_map_no_idle

#rm -rf output/output_baseline_vieille-toulouse_vivaldi_no_cache_map_no_idle
#mkdir output/output_baseline_vieille-toulouse_vivaldi_no_cache_map_no_idle
#clear && clear && echo 'running baseline vivaldi no cache...' && build/src/simulationmain input/platform/platform_latency_vieille-toulouse_vivaldi_no_edge_no_idle.xml input/deployment/deploy_baseline.xml input/workload/vieille-toulouse_30k.txt --log=root.app:file:output/output_baseline_vieille-toulouse_vivaldi_no_cache_map_no_idle/logfile.log
#python3 util/process_log.py input/workload/vieille-toulouse_30k.txt output/output_baseline_vieille-toulouse_vivaldi_no_cache_map_no_idle/logfile.log output/output_baseline_vieille-toulouse_vivaldi_no_cache_map_no_idle

#clear && clear && echo 'results of comparing requests from both algorithms:' &&  python3 util/compare_requests.py
#rm -rf output/output_baseline_vieille-toulouse_vivaldi_cache_map_no_idle_param
#mkdir  output/output_baseline_vieille-toulouse_vivaldi_cache_map_no_idle_param
#clear && clear && echo 'running baseline vivaldi with cache map structure...' && build/src/simulationmain input/platform/platform_latency_vieille-toulouse_vivaldi_no_edge_no_idle.xml input/deployment/deploy_baseline_cache.xml input/workload/vieille-toulouse_30k.txt --log=root.app:file:output/output_baseline_vieille-toulouse_vivaldi_cache_map_no_idle_param/logfile.log
#python3 util/process_log.py input/workload/vieille-toulouse_30k.txt output/output_baseline_vieille-toulouse_vivaldi_cache_map_no_idle_param/logfile.log output/output_baseline_vieille-toulouse_vivaldi_cache_map_no_idle_param

#rm -rf output/output_HEFT_vieille-toulouse_vivaldi_cache_map_no_idle_param_no_cloud
#mkdir  output/output_HEFT_vieille-toulouse_vivaldi_cache_map_no_idle_param_no_cloud
#clear && clear && echo 'running HEFT vivaldi with cache map structure no cloud...' && build/src/simulationmain input/platform/platform_latency_vieille-toulouse_vivaldi_no_coud.xml input/deployment/deploy_HEFT_cache.xml input/workload/vieille-toulouse_12h_to_13h.txt --log=root.app:file:output/output_HEFT_vieille-toulouse_vivaldi_cache_map_no_idle_param_no_cloud/logfile.log
#python3 util/process_log.py input/workload/vieille-toulouse_12h_to_13h.txt output/output_HEFT_vieille-toulouse_vivaldi_cache_map_no_idle_param_no_cloud/logfile.log output/output_HEFT_vieille-toulouse_vivaldi_cache_map_no_idle_param_no_cloud

#rm -rf output/output_HEFT_vieille-toulouse_vivaldi_cache_map_no_idle_param
#mkdir  output/output_HEFT_vieille-toulouse_vivaldi_cache_map_no_idle_param
#clear && clear && echo 'running HEFT vivaldi with cache map structure...' && build/src/simulationmain input/platform/platform_latency_vieille-toulouse_vivaldi_no_edge_no_idle.xml input/deployment/deploy_HEFT_cache.xml input/workload/vieille-toulouse_30k.txt --log=root.app:file:output/output_HEFT_vieille-toulouse_vivaldi_cache_map_no_idle_param/logfile.log
#python3 util/process_log.py input/workload/vieille-toulouse_30k.txt output/output_HEFT_vieille-toulouse_vivaldi_cache_map_no_idle_param/logfile.log output/output_HEFT_vieille-toulouse_vivaldi_cache_map_no_idle_param

#rm -rf output/output_GEFT_vieille-toulouse_vivaldi_cache_map_no_idle_param
#mkdir  output/output_GEFT_vieille-toulouse_vivaldi_cache_map_no_idle_param
#clear && clear && echo 'running HEFT vivaldi with cache map structure...' && build/src/simulationmain input/platform/platform_latency_vieille-toulouse_vivaldi_no_edge_no_idle.xml input/deployment/deploy_GEFT_cache.xml input/workload/vieille-toulouse_30k.txt --log=root.app:file:output/output_GEFT_vieille-toulouse_vivaldi_cache_map_no_idle_param/logfile.log
#python3 util/process_log.py input/workload/vieille-toulouse_30k.txt output/output_GEFT_vieille-toulouse_vivaldi_cache_map_no_idle_param/logfile.log output/output_GEFT_vieille-toulouse_vivaldi_cache_map_no_idle_param

#rm -rf output/output_baseline_vieille-toulouse_vivaldi
#mkdir  output/output_baseline_vieille-toulouse_vivaldi
#clear && clear && echo 'running baseline vivaldi...' && build/src/simulationmain input/platform/platform_latency_vieille-toulouse_vivaldi_no_edge_no_idle.xml input/deployment/deploy_baseline.xml input/workload/vieille-toulouse_30k.txt --log=root.app:file:output/output_baseline_vieille-toulouse_vivaldi/logfile.log
#python3 util/process_log.py input/workload/vieille-toulouse_30k.txt output/output_baseline_vieille-toulouse_vivaldi/logfile.log output/output_baseline_vieille-toulouse_vivaldi


#rm -rf output/output_baseline_vieille-toulouse_vivaldi_interface
#mkdir  output/output_baseline_vieille-toulouse_vivaldi_interface
#clear && clear && echo 'running baseline vivaldi...' && build/src/simulationmain input/platform/platform_latency_vieille-toulouse_vivaldi_no_edge_no_idle.xml input/deployment/deploy_baseline.xml input/workload/vieille-toulouse_30k.txt --log=root.app:file:output/output_baseline_vieille-toulouse_vivaldi_interface/logfile.log
#python3 util/process_log.py input/workload/vieille-toulouse_30k.txt output/output_baseline_vieille-toulouse_vivaldi_interface/logfile.log output/output_baseline_vieille-toulouse_vivaldi_interface

#rm -rf output/output_HEFT_vieille-toulouse_vivaldi_interface
#mkdir  output/output_HEFT_vieille-toulouse_vivaldi_interface
#clear && clear && echo 'running baseline vivaldi...' && build/src/simulationmain input/platform/platform_latency_vieille-toulouse_vivaldi_cloud.xml input/deployment/deploy_HEFT_cache.xml input/workload/vieille-toulouse_30k.txt --log=root.app:file:output/output_HEFT_vieille-toulouse_vivaldi_interface/logfile.log
#python3 util/process_log.py input/workload/vieille-toulouse_30k.txt output/output_HEFT_vieille-toulouse_vivaldi_interface/logfile.log output/output_HEFT_vieille-toulouse_vivaldi_interface
#

#rm -rf output/output_HEFT_vieille-toulouse_vivaldi
#mkdir  output/output_HEFT_vieille-toulouse_vivaldi
#clear && clear && echo 'running baseline vivaldi...' && build/src/simulationmain input/platform/platform_latency_vieille-toulouse_vivaldi_cloud.xml input/deployment/deploy_HEFT.xml input/workload/vieille-toulouse_30k.txt --log=root.app:file:output/output_HEFT_vieille-toulouse_vivaldi/logfile.log
#python3 util/process_log.py input/workload/vieille-toulouse_30k.txt output/output_HEFT_vieille-toulouse_vivaldi/logfile.log output/output_HEFT_vieille-toulouse_vivaldi


#rm -rf output/output_GEFT_30k
#mkdir output/output_GEFT_30k
#clear && clear && echo 'running GEFT cloud ...' && build/src/simulationmain input/platform/platform_latency_vieille-toulouse_vivaldi_cloud.xml input/deployment/deploy_GEFT.xml input/workload/vieille-toulouse_30k.txt --log=root.app:file:output/output_GEFT_30k/logfile.log
#python3 util/process_log.py input/workload/vieille-toulouse_30k.txt output/output_GEFT_30k/logfile.log output/output_GEFT_30k

#rm -rf output/output_GEFT_30k_interface
#mkdir output/output_GEFT_30k_interface
#clear && clear && echo 'running GEFT cloud interface...' && build/src/simulationmain input/platform/platform_latency_vieille-toulouse_vivaldi_cloud.xml input/deployment/deploy_GEFT_cache.xml input/workload/vieille-toulouse_30k.txt --log=root.app:file:output/output_GEFT_30k_interface/logfile.log
#python3 util/process_log.py input/workload/vieille-toulouse_30k.txt output/output_GEFT_30k_interface/logfile.log output/output_GEFT_30k_interface
#
#


rm -rf output/output_baseline_30k_cache
mkdir  output/output_baseline_30k_cache
clear && clear && echo 'running baseline vivaldi cache...' && build/src/simulationmain input/platform/platform_latency_vieille-toulouse_vivaldi_cloud.xml input/deployment/deploy_baseline_cache.xml input/workload/vieille-toulouse_30k.txt  --log=root.app:file:output/output_baseline_30k_cache/logfile.log
python3 util/process_log.py input/workload/vieille-toulouse_30k.txt output/output_baseline_30k_cache/logfile.log output/output_baseline_30k_cache

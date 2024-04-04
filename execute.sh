#!/bin/bash

#rm -rf output_baseline_min
#mkdir output_baseline_min
#clear && clear && echo 'running baseline min...' && build/src/simulationmain input/platform/vieille-toulouse_cloud.xml input/deployment/deploy_baseline.xml input/workload/baseline_makespan.txt --log=root.app:file:output_baseline_min/logfile.log
#python3 util/process_log.py input/workload/baseline_makespan.txt output_baseline_min/logfile.log output_baseline_min
#
#
#
#rm -rf output_baseline_30k
#mkdir output_baseline_30k
#clear && clear && echo 'running baseline cloud with idle...' && build/src/simulationmain input/platform/vieille-toulouse_cloud.xml input/deployment/deploy_baseline.xml input/workload/vieille-toulouse_30k.txt --log=root.app:file:output_baseline_30k/logfile.log
#python3 util/process_log.py input/workload/vieille-toulouse_30k.txt output_baseline_30k/logfile.log output_baseline_30k
#
#rm -rf output_GEFT_30k
#mkdir output_GEFT_30k
#clear && clear && echo 'running GEFT cloud with idle...' && build/src/simulationmain input/platform/vieille-toulouse_cloud.xml input/deployment/deploy_GEFT.xml input/workload/vieille-toulouse_30k.txt --log=root.app:file:output_GEFT_30k/logfile.log
#python3 util/process_log.py input/workload/vieille-toulouse_30k.txt output_GEFT_30k/logfile.log output_GEFT_30k
#
#rm -rf output_baseline_30k_no_idle
#mkdir output_baseline_30k_no_idle
#clear && clear && echo 'running baseline cloud no idle...' && build/src/simulationmain input/platform/vieille-toulouse_cloud_no_idle_power.xml input/deployment/deploy_baseline.xml input/workload/vieille-toulouse_30k.txt --log=root.app:file:output_baseline_30k_no_idle/logfile.log
#python3 util/process_log.py input/workload/vieille-toulouse_30k.txt output_baseline_30k_no_idle/logfile.log output_baseline_30k_no_idle
#
#rm -rf output_GEFT_30k_no_idle
#mkdir output_GEFT_30k_no_idle
#clear && clear && echo 'running GEFT cloud no idle...' && build/src/simulationmain input/platform/vieille-toulouse_cloud_no_idle_power.xml input/deployment/deploy_GEFT.xml input/workload/vieille-toulouse_30k.txt --log=root.app:file:output_GEFT_30k_no_idle/logfile.log
#python3 util/process_log.py input/workload/vieille-toulouse_30k.txt output_GEFT_30k_no_idle/logfile.log output_GEFT_30k_no_idle


#rm -rf output_solar
#mkdir output_solar
#clear && clear && build/src/simulationmain input/platform/vieille-toulouse_cloud.xml input/deployment/deploy_solar.xml input/workload/vieille-toulouse_30k.txt --log=root.app:file:output_solar/logfile.log
#python3 util/process_log.py input/workload/vieille-toulouse_30k.txt output_solar/logfile.log output_solar



#rm -rf output_bestfit
#mkdir output_bestfit
#clear && clear && build/src/simulationmain input/platform/vieille-toulouse_cloud.xml input/deployment/deploy_bestfit.xml input/workload/vieille-toulouse_30k.txt --log=root.app:file:output_bestfit/logfile.log
#python3 util/process_log.py input/workload/vieille-toulouse_30k.txt output_bestfit/logfile.log output_bestfit
##
##
##
#rm -rf output_HEFT
#mkdir output_HEFT
#clear && clear && build/src/simulationmain input/platform/vieille-toulouse_cloud.xml input/deployment/deploy_HEFT.xml input/workload/vieille-toulouse_30k.txt --log=root.app:file:output_HEFT/logfile.log
#python3 util/process_log.py input/workload/vieille-toulouse_30k.txt output_HEFT/logfile.log output_HEFT
#



#rm -rf output_offload_cloud
#mkdir output_offload_cloud
#clear && clear && echo 'running all to cloud .. ' && build/src/simulationmain input/platform/vieille-toulouse_cloud.xml input/deployment/deploy_all_to_cloud.xml input/workload/dag-test.txt --log=root.app:file:output_offload_cloud/logfile.log
#python3 util/process_log.py input/workload/dag-test.txt output_offload_cloud/logfile.log output_offload_cloud


#rm -rf output_test_brown
#mkdir output_test_brown
#clear && clear && echo 'running GEFT cloud no idle...' && build/src/simulationmain input/platform/vieille-toulouse_cloud_no_idle_power.xml input/deployment/deploy_GEFT.xml input/workload/test-energy.txt --log=root.app:file:output_test_brown/logfile.log
#python3 util/process_log.py input/workload/test-energy.txt output_test_brown/logfile.log output_test_brown



rm -rf output_HEFT_min
mkdir output_HEFT_min
clear && clear && echo 'running HEFT min...' && build/src/simulationmain input/platform/vieille-toulouse_cloud.xml input/deployment/deploy_HEFT.xml input/workload/baseline_makespan.txt --log=root.app:file:output_HEFT_min/logfile.log
python3 util/process_log.py input/workload/baseline_makespan.txt output_HEFT_min/logfile.log output_HEFT_min

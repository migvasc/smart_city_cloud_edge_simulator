#!/bin/bash

rm -rf output_baseline
mkdir output_baseline
clear && clear && build/src/simulationmain input/platform/vieille-toulouse.xml input/deployment/deploy_baseline.xml input/workload/dag-very-small.txt --log=root.app:file:output_baseline/logfile.log
python3 util/process_log.py input/workload/dag-very-small.txt output_baseline/logfile.log output_baseline

rm -rf output_bestfit
mkdir output_bestfit

clear && clear && build/src/simulationmain input/platform/vieille-toulouse.xml input/deployment/deploy_bestfit.xml input/workload/dag-very-small.txt --log=root.app:file:output_bestfit/logfile.log
python3 util/process_log.py input/workload/dag-very-small.txt output_bestfit/logfile.log output_bestfit

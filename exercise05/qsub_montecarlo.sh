#!/bin/bash

#$ -q std.q
#$ -cwd
#$ -o output/output_montecarlo.dat
#$ -e output/error_montecarlo.dat

#$ -pe openmp 8

bash ./compile_montecarlo.sh
bash ./run_montecarlo_pi.sh

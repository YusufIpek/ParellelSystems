#!/bin/bash

#$ -q std.q
#$ -cwd
#$ -o output_omp_queens.dat
#$ -e error_omp_queens.dat

#$ -pe openmp 8

bash ./compile_queens.sh
bash ./run_queens.sh

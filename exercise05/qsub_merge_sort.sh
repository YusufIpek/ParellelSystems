#!/bin/bash

#$ -q std.q
#$ -cwd
#$ -o output/output_mergesort.dat
#$ -e output/error_mergesort.dat

#$ -pe openmp 8

bash ./compile_mergesort.sh
bash ./run_merge_sort.sh

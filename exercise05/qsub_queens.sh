#!/bin/bash

#$ -q std.q
#$ -cwd
#$ -o output/output_queens.dat
#$ -e output/error_queens.dat

#$ -pe openmp 8

bash ./compile_queens.sh
bash ./run_queens.sh

#!/bin/bash

#$ -q std.q
#$ -cwd

for i in 1 2 4 8;
do
  #$ -o output/output_jacobi_$i.dat
  #$ -e output/error_jacobi_$i.dat
  #$ -pe openmp $i
  bash run_jacobi.sh
done

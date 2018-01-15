#!/bin/bash

gcc *.c -O3 -o real -lm -g3
gcc *.c -O3 -o real_omp -lm -g3 -fopenmp

separator="--------------------------------------"

echo "Sequential"
{ time ./real; } 2>&1
echo $separator


for i in 1 2 4 8
do	
	export OMP_NUM_THREADS=$i
	echo "Parallel "$i
	{ time ./real_omp; } 2>&1
	echo $separator
done

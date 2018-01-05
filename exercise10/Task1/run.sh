#!/bin/bash

module load openmpi/1.8.5

mpiCC -std=c++11 -Wall ./matrix_mul_mpi.cpp -o matrix_mul_mpi -O3 -DGATHER=1
mpiCC -std=c++11 -Wall ./matrix_mul_mpi.cpp -o matrix_mul_mpi_v2 -O3 -DGATHER=0

for i in 512 1024
do
	for j in 1 2 4 8 16 32 64
	do
		echo "GATHER| Processes: " $j " Matrix size: " $i 
		{ time mpirun -np $j ./matrix_mul_mpi $i; } 2>&1
		echo
		echo "GATHERV| Processes: " $j " Matrix size: " $i
		{ time mpirun -np $j ./matrix_mul_mpi_v2 $i; } 2>&1
		echo
	done
done

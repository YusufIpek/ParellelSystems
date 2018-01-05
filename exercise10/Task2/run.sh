#!/bin/bash

module load openmpi/1.8.5

mpiCC -std=c++11 -march=native nqueens_mpi.cpp -o nqueens_mpi -O3

for i in 11 12 13 14 15
do
	for j in 1 2 4 8
	do
		echo "Processes: " $j " Board size: " $i
		{ time mpirun -np $j ./nqueens_mpi $i; } 2>&1
	done
done

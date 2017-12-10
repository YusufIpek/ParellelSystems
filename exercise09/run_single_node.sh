#!/bin/bash

for i in 1 2 4 8
do
echo "Size: 512| Processes: " $i
{ time mpirun -np $i ./jacobi_2D_512_mpi 1.0 0.5 0.0 -0.5; } 2>&1
echo "Size: 768| Processes: " $i
{ time mpirun -np $i ./jacobi_2D_768_mpi 1.0 0.5 0.0 -0.5; } 2>&1
done

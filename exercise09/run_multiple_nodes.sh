#!/bin/bash


echo "Size: 512| Processes: " $1
{ time mpirun -np $1 ./jacobi_2D_512_mpi 1.0 0.5 0.0 -0.5; } 2>&1
echo "Size: 768| Processes: " $1
{ time mpirun -np $1 ./jacobi_2D_768_mpi 1.0 0.5 0.0 -0.5; } 2>&1
done

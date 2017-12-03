#!/bin/bash



for i in 10000000 100000000 1000000000 10000000000
do
	echo
	printf "Problem size: %'.d\n" $i
	./montecarlo_pi $i
	echo
	mpiexec -n 4 ./montecarlo_pi_mpi $i
	echo
done
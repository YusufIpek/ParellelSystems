#!/bin/bash

for i in 100000 1000000 10000000 100000000 1000000000
do
	LC_NUMERIC=en_US printf "Problem size: %'.f\n" $i
	printf "Seq "
	./merge_sort $i
	for j in 1 2 4 8
	do
		export OMP_NUM_THREADS=$j
		printf "Number of Threads: " $j
		./merge_sort_par $i
	done
	printf "\n"
done
#!/bin/bash

export opt=3


for i in {100000,1000000,10000000,100000000}
do
	LC_NUMERIC=en_US printf "Problem size: %'.f\n" $i
	printf "Seq "
	./montecarlo_pi $i
	for j in {1,2,4,8}
	do
		printf "Par "$j" "
		./montecarlo_pi_par_$j $i
	done
	printf "\n"
done
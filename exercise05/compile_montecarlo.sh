#!/bin/bash

if ! g++ montecarlo_pi.cpp -o montecarlo_pi -Wall -O3 -std=c++11;
then
  echo COMPILATION of montecarlo_pi FAILED!
  return 1
fi

for i in 1 2 4 8;
do
  if ! g++ montecarlo_pi_par.cpp -o montecarlo_pi_par_$i -Wall -O3 -std=c++11 -fopenmp -DOUR_NUMBER_OF_THREADS=$i;
  then
    echo COMPILATION of montecarlo_pi_par_$i FAILED!
    return 1
  fi
done

echo COMPILATION SUCCESSFUL

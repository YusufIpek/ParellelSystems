#!/bin/bash

g++ montecarlo_pi.cpp -o montecarlo_pi -Wall -O3 -std=c++11
g++ montecarlo_pi_par.cpp -o montecarlo_pi_par -Wall -O3 -std=c++11 -fopenmp


for i in 10000 100000 1000000 10000000 100000000;
do
  echo "N = "$i
  ./montecarlo_pi $i
  ./montecarlo_pi_par $i
  echo ""
done
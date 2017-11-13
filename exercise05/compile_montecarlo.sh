#!/bin/bash

if ! g++ montecarlo_pi.cpp -o montecarlo_pi -Wall -O3 -std=c++11;
then
  echo COMPILATION of montecarlo_pi FAILED!
  return 1
fi


if ! g++ montecarlo_pi_par.cpp -o montecarlo_pi_par -Wall -O3 -std=c++11 -fopenmp;
then
  echo COMPILATION of montecarlo_pi_par FAILED!
  return 1
fi

echo COMPILATION SUCCESSFUL

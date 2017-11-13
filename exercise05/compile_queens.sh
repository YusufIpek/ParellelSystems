#!/bin/bash

if ! g++ n_queens.cpp -o n_queens -Wall -O3 -std=c++11;
then
  echo COMPILATION of n_queens FAILED!
  return 1
fi


if ! g++ n_queens_par.cpp -o n_queens_par -Wall -O3 -std=c++11 -fopenmp;
then
  echo COMPILATION of n_queens_par FAILED!
  return 1
fi


echo COMPILATION SUCCESSFUL

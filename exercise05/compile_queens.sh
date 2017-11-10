#!/bin/bash

if ! g++ n_queens.cpp -o n_queens -Wall -O3 -std=c++11;
then
  echo COMPILATION of n_queens FAILED!
  return 1
fi

for i in 1 2 4 8;
do
  if ! g++ n_queens_par.cpp -o n_queens_par_$i -Wall -O3 -std=c++11 -fopenmp -DOMP_NUM_THREADS=$i;
  then
    echo COMPILATION of n_queens_par_$i FAILED!
    return 1
  fi
done

echo COMPILATION SUCCESSFUL

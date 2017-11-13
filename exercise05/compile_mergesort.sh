#!/bin/bash

if ! g++ merge_sort.cpp -o merge_sort -Wall -O3 -std=c++11;
then
  echo COMPILATION of merge_sort FAILED!
  return 1
fi


if ! g++ merge_sort_par.cpp -o merge_sort_par -Wall -O3 -std=c++11 -fopenmp;
then
  echo COMPILATION of merge_sort_par FAILED!
  return 1
fi


echo COMPILATION SUCCESSFUL

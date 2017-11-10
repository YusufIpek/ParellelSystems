#!/bin/bash

if ! g++ merge_sort.cpp -o merge_sort -Wall -O3 -std=c++11;
then
  echo COMPILATION of merge_sort FAILED!
  return 1
fi

for i in 1 2 4 8;
do
  if ! g++ merge_sort_par.cpp -o merge_sort_par_$i -Wall -O3 -std=c++11 -fopenmp -DOUR_NUMBER_OF_THREADS=$i;
  then
    echo COMPILATION of merge_sort_par_$i FAILED!
    return 1
  fi
done

echo COMPILATION SUCCESSFUL

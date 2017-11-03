#!/bin/bash

g++ n_queens_par.cpp -o n_queens_par -Wall -O3 -std=c++11 -fopenmp

# https://de.wikipedia.org/wiki/Damenproblem
SOLUTIONS=(1 1 0 0 2 10 4 40 92 352 724 2680 14200)

for i in {1..12};
do
  result="$(./n_queens_par $i)"
  echo result: ${result}
  #sol=$SOLUTIONS[]
  echo $SOLUTIONS[${i}]
  
  if [ $result -ne ${SOLUTIONS[$i]} ]
  then
     echo "Test FAILED"
  fi
done
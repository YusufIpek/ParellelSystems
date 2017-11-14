#!/bin/bash

# https://de.wikipedia.org/wiki/Damenproblem
# solutions from 0 to 20 queens
SOLUTIONS=(1 1 0 0 2 10 4 40 92 352 724 2680 14200 73712 365596 2279184 14772512 95815104 6660909624 4968057848 39029188884)

for i in {8..15};
do
  printf "\n\nWiki-solutions for n="$i":  ${SOLUTIONS["${i}"]}\n"
  echo "seqential"
  time ./n_queens $i
  
  for j in 1 2 4 8;
  do
	export OMP_NUM_THREADS=$j
    echo "parallel: number of threads = "$j
    { time ./n_queens_par $i; } 2>&1
  done
done

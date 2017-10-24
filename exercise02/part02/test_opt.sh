#!/bin/bash

export opt=3
export repeats=5
export input_file="mmul_opt.cpp"
export filename="log_opt_test.txt"
export seperator="--------------------------------------------------------------------------------------------------------"

g++ $input_file -Wall -std=c++11 -O$opt -o mmul_nes -DNESTED_VECTOR
g++ $input_file -Wall -std=c++11 -O$opt -o mmul_conmul -DCONTIGUOUS_WITH_MULTIPLICATION
g++ $input_file -Wall -std=c++11 -O$opt -o mmul_conin -DCONTIGUOUS_WITH_INDIRECTION

for i in {0..2};
do
  for MSIZE in 256 512 1024 2048;
  do
    perf stat -d -e task-clock,cycles,instructions,cache-references,cache-misses,branch,branch-misses,page-faults -r$repeats ./mmul_nes $MSIZE >> $filename 2>&1
    echo $seperator >>$filename
    perf stat -d -e task-clock,cycles,instructions,cache-references,cache-misses,branch,branch-misses,page-faults -r$repeats ./mmul_conmul $MSIZE >>$filename 2>&1
    echo $seperator >>$filename
    perf stat -d -e task-clock,cycles,instructions,cache-references,cache-misses,branch,branch-misses,page-faults -r$repeats ./mmul_conin $MSIZE >>$filename 2>&1
    echo $seperator >>$filename
  done
done

#!/bin/bash

export opt=3
export repeats=5
export seperator="--------------------------------------------------------------------------------------------------------"

g++ mmul.cpp -Wall -std=c++11 -O$opt -o mmul_nes -DNESTED_VECTOR
g++ mmul.cpp -Wall -std=c++11 -O$opt -o mmul_conmul -DCONTIGUOUS_WITH_MULTIPLICATION
g++ mmul.cpp -Wall -std=c++11 -O$opt -o mmul_conin -DCONTIGUOUS_WITH_INDIRECTION

for i in {0..2};
do
  for MSIZE in 256 512 1024 2048;
  do
    perf stat -d -r$repeats ./mmul_nes $MSIZE >>log_test.txt 2>&1
    echo $seperator >>log_test.txt
    perf stat -d -r$repeats ./mmul_conmul $MSIZE >>log_test.txt 2>&1
    echo $seperator >>log_test.txt
    perf stat -d -r$repeats ./mmul_conin $MSIZE >>log_test.txt 2>&1
    echo $seperator >>log_test.txt
  done
done

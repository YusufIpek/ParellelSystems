#!/bin/bash

export opt=3

g++ mmul.cpp -Wall -std=c++11 -O$opt -o mmul_nes -DNESTED_VECTOR
g++ mmul.cpp -Wall -std=c++11 -O$opt -o mmul_conmul -DCONTIGUOUS_WITH_MULTIPLICATION
g++ mmul.cpp -Wall -std=c++11 -O$opt -o mmul_conin -DCONTIGUOUS_WITH_INDIRECTION

for i in {0..1};
do
  for MSIZE in 256 512 1024 2048 4096;
  do
    perf stat ./mmul_nes $MSIZE &> log_nes_run-${i}_size-$MSIZE.txt
    perf stat ./mmul_conmul $MSIZE &> log_conmul_run-${i}_size-$MSIZE.txt
    perf stat ./mmul_conin $MSIZE &> log_conin_run-${i}_size-$MSIZE.txt
  done
done  
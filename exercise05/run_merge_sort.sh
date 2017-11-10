#!/bin/bash

export opt=3
export filename_seq="merge_sort"
export filename_par="merge_sort_par"


for i in {100000,1000000,10000000,100000000}
do
LC_NUMERIC=en_US printf "Problem size: %'.f\n" $i
printf "Seq "
./$filename_seq $i
printf "Par "
./$filename_par $i
printf "\n"
done
#!/bin/bash
#script for running one benchmark c file with a singular input.

export ITERATION=10

output_file="/tmp/time.txt"
> $output_file 

for (( c=1; c<=$ITERATION; c++ ))
do
  start=$(($(date +%s)*1000 + $(date +%N | cut -c1-3)))
  $@  
  end=$(($(date +%s)*1000 + $(date +%N | cut -c1-3)))
  runtime=$((end - start))
  echo $runtime >> $output_file 
done

awk '{s+=$1; y+=$1*$1} END {print "Avg runtime: " s/NR " ms, Std: " sqrt(y/NR - (s/NR)^2) " ms"}' $output_file

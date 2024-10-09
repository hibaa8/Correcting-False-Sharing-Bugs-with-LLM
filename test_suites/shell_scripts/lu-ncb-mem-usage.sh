#!/bin/bash

commands=(
  "../lu_ncb/lu -p4 -n2048 -b16"
  "../lu_ncb/lu -p4 -n2048 -b100"
  "../lu_ncb/lu -p4 -n2048 -b256"
  "../lu_ncb/lu-manual -p4 -n2048 -b16"
  "../lu_ncb/lu-manual -p4 -n2048 -b100"
  "../lu_ncb/lu-manual -p4 -n2048 -b256"
  "../lu_ncb/lu-llm -p4 -n2048 -b16"
  "../lu_ncb/lu-llm -p4 -n2048 -b100"
  "../lu_ncb/lu-llm -p4 -n2048 -b256"
)

for cmd in "${commands[@]}"; do
    echo "Running command: $cmd"
    read -r -a cmd_array <<< "$cmd"
    memory_usage=$(./run_memory_usage.sh "${cmd_array[@]}")
    echo "$memory_usage"
    echo "-------------------------------------" 
done
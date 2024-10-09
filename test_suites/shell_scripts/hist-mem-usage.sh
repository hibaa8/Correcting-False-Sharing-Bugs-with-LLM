#!/bin/bash


commands=(
    "../histogram/hist-pthread histogram/histogram_datafiles/small.bmp"
    "../histogram/hist-pthread histogram/histogram_datafiles/med.bmp"
    "../histogram/hist-pthread histogram/histogram_datafiles/large.bmp"
    "../histogram/hist-pthread-manual histogram/histogram_datafiles/small.bmp"
    "../histogram/hist-pthread-manual histogram/histogram_datafiles/med.bmp"
    "../histogram/hist-pthread-manual histogram/histogram_datafiles/large.bmp"
    "../histogram/hist-pthread-llm histogram/histogram_datafiles/small.bmp"
    "../histogram/hist-pthread-llm histogram/histogram_datafiles/med.bmp"
    "../histogram/hist-pthread-llm histogram/histogram_datafiles/large.bmp"
)

for cmd in "${commands[@]}"; do
    echo "Running command: $cmd"
    read -r -a cmd_array <<< "$cmd"

    memory_usage=$(./run_memory_usage.sh "${cmd_array[@]}")
    
    echo "$memory_usage"
    echo "-------------------------------------" 
done

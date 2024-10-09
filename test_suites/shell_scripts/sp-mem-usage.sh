#!/bin/bash

commands=(
    "../boost_test/sp 1000"
    "../boost_test/sp 10000"
    "../boost_test/sp 15000"
    "../boost_test/sp-manual 1000"
    "../boost_test/sp-manual 10000"
    "../boost_test/sp-manual 1500"
    "../boost_test/sp-llm 1000"
    "../boost_test/sp-llm 10000"
    "../boost_test/sp-llm 15000"
)

for cmd in "${commands[@]}"; do
    echo "Running command: $cmd"
    read -r -a cmd_array <<< "$cmd"

    memory_usage=$(./run_memory_usage.sh "${cmd_array[@]}")
    
    echo "$memory_usage"
    echo "-------------------------------------"  
done

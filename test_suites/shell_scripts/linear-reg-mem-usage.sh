#!/bin/bash

commands=(
    "../linear_regression/lreg-pthread linear_regression/linear_regression_datafiles/key_file_50MB.txt"
    "../linear_regression/lreg-pthread linear_regression/linear_regression_datafiles/key_file_100MB.txt"
    "../linear_regression/lreg-pthread linear_regression/linear_regression_datafiles/key_file_500MB.txt"
    "../linear_regression/lreg-pthread-manual linear_regression/linear_regression_datafiles/key_file_50MB.txt"
    "../linear_regression/lreg-pthread-manual linear_regression/linear_regression_datafiles/key_file_100MB.txt"
    "../linear_regression/lreg-pthread-manual linear_regression/linear_regression_datafiles/key_file_500MB.txt"
    "../linear_regression/lreg-pthread-llm linear_regression/linear_regression_datafiles/key_file_50MB.txt"
    "../linear_regression/lreg-pthread-llm linear_regression/linear_regression_datafiles/key_file_100MB.txt"
    "../linear_regression/lreg-pthread-llm linear_regression/linear_regression_datafiles/key_file_500MB.txt"
)

for cmd in "${commands[@]}"; do
    echo "Running command: $cmd"
    read -r -a cmd_array <<< "$cmd"
    memory_usage=$(./run_memory_usage.sh "${cmd_array[@]}")
    echo "$memory_usage"
    echo "-------------------------------------"
done

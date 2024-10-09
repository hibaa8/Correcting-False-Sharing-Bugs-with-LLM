#!/bin/bash

commands=(
    "../string_match/string_match_pthreads string_match/string_match_datafiles/key_file_50MB.txt"
    "../string_match/string_match_pthreads string_match/string_match_datafiles/key_file_100MB.txt"
    "../string_match/string_match_pthreads string_match/string_match_datafiles/key_file_500MB.txt"
    "../string_match/string_match_pthreads_manual string_match/string_match_datafiles/key_file_50MB.txt"
    "../string_match/string_match_pthreads_manual string_match/string_match_datafiles/key_file_100MB.txt"
    "../string_match/string_match_pthreads_manual string_match/string_match_datafiles/key_file_500MB.txt"
    "../string_match/string_match_pthreads_llm string_match/string_match_datafiles/key_file_50MB.txt"
    "../string_match/string_match_pthreads_llm string_match/string_match_datafiles/key_file_100MB.txt"
    "../string_match/string_match_pthreads_llm string_match/string_match_datafiles/key_file_500MB.txt"
)

for cmd in "${commands[@]}"; do
    echo "Running command: $cmd"
    read -r -a cmd_array <<< "$cmd"
    memory_usage=$(./run_memory_usage.sh "${cmd_array[@]}")
    
    echo "$memory_usage"
    echo "-------------------------------------"  
done

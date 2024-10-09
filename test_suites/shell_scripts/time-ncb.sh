#!/bin/bash

# Array of block sizes
block_sizes=(16 100 256)

# Array of program variants
programs=("lu" "lu-manual" "lu-llm")

# Number of processors and problem size
processors=4
problem_size=2048

# Temporary file to store runtime information
output_file="/tmp/lu_times.txt"

# Function to calculate average and standard deviation
calculate_stats() {
    times=("$@")
    total=0
    sum_sq=0
    n=${#times[@]}

    # Calculate sum of times
    for time in "${times[@]}"; do
        total=$((total + time))
    done

    # Calculate the average
    avg=$((total / n))

    # Calculate sum of squares for standard deviation
    for time in "${times[@]}"; do
        diff=$((time - avg))
        sum_sq=$((sum_sq + diff * diff))
    done

    # Calculate standard deviation
    std=$(echo "sqrt($sum_sq / $n)" | bc -l)

    echo "Average time: $avg ms, Standard deviation: $std ms"
}

# Loop through each program variant
for program in "${programs[@]}"
do
  # Loop through each block size
  for block_size in "${block_sizes[@]}"
  do
    echo "Running $program with block size $block_size"
    
    times=()  # Array to store times for 3 runs

    # Run the command 3 times and capture the times
    for (( i=1; i<=3; i++ ))
    do
      # Run the program and capture output
      output=$(./$program -p$processors -n$problem_size -b$block_size)

      # Extract the total time without initialization using grep and awk
      time=$(echo "$output" | grep "Total time without initialization" | awk '{print $6}')
      
      # Store the time in the array
      times+=($time)

      echo "Run $i for $program with block size $block_size: $time ms"
    done

    # Calculate and display the average and standard deviation
    calculate_stats "${times[@]}"
  done
done


#!/bin/bash
#script for running multiple benchmark c files with a multiple input sizes.

export ITERATION=10

programs=( "$1" "$2" "$3" )
input_files=( "$4" "$5" "$6" )
output_file="/tmp/time.txt"

for program in "${programs[@]}"
do
  for input_file in "${input_files[@]}"
  do
    echo "Running $program with $input_file"
    > $output_file
    for (( c=1; c<=$ITERATION; c++ ))
    do
      start=$(($(date +%s)*1000 + $(date +%N | cut -c1-3)))
      $program $input_file   
      end=$(($(date +%s)*1000 + $(date +%N | cut -c1-3)))
      runtime=$((end - start))
      echo $runtime >> $output_file  
    done

    awk -v prog="$program" -v input="$input_file" \
    '{s+=$1; y+=$1*$1} END {print prog, input, "Avg runtime: " s/NR " ms, Std deviation: " sqrt(y/NR - (s/NR)^2) " ms"}' $output_file
  done
done

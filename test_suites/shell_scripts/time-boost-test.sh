#!/bin/bash

iterations=3

calc_avg_std() {
    local times=("$@")
    local sum=0
    local avg=0
    local variance=0
    local std=0

    for time in "${times[@]}"; do
        sum=$(echo "$sum + $time" | bc)
    done

    avg=$(echo "scale=2; $sum / ${#times[@]}" | bc)

    for time in "${times[@]}"; do
        variance=$(echo "$variance + ($time - $avg)^2" | bc)
    done

    std=$(echo "scale=2; sqrt($variance / ${#times[@]})" | bc)

    echo "Average: $avg ms"
    echo "Standard Deviation: $std ms"
}

time_command() {
    local cmd="$1"
    local arg="$2"
    local times=()

    for ((i=1; i<=$iterations; i++)); do
        time_in_ms=$( { /usr/bin/time -f "%e" "$cmd" "$arg" 2>&1 1>/dev/null; } )
        time_in_ms=$(echo "$time_in_ms * 1000" | bc)
        times+=("$time_in_ms")
        echo "Run $i for $cmd $arg: $time_in_ms ms"
    done

    calc_avg_std "${times[@]}"
    echo ""
}

commands=("./sp" "./sp-manual" "./sp-llm")
args=(1000 10000 100000)

for cmd in "${commands[@]}"; do
    for arg in "${args[@]}"; do
        echo "Timing $cmd $arg..."
        time_command "$cmd" "$arg"
    done
done

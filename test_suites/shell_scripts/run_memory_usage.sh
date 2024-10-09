#!/bin/bash

"$@" &
pid=$!

prev_rss=$(ps -o rss= -p $pid)
prev_vsz=$(ps -o vsz= -p $pid)

while true; do
    if ! ps -p $pid > /dev/null; then
        break;
    fi

    rss=$(ps -o rss= -p $pid)
    vsz=$(ps -o vsz= -p $pid)

    if [[ $rss -ne $prev_rss || $vsz -ne $prev_vsz ]]; then
        echo "RSS: $rss, VSZ: $vsz" >> log.out
        prev_rss=$rss
        prev_vsz=$vsz
    fi

    sleep 0.1  
done

awk 'BEGIN { maxvsz=0; maxrss=0; sumrss=0; } \
{ if ($1 == "RSS:") { maxvsz=$2; sumrss+=$2; } if ($1 == "VSZ:") { maxrss=$3; } } \
END { print "rss=" maxvsz " vsz=" sumrss/NR }' log.out

rm log.out

#!/bin/sh

for i in `seq 1 1000`; do
    echo $i
    sudo /bin/sh -c "echo $i > /sys/kernel/debug/tracing/trace_marker"
    ./cpuclock
    if [ $? -ne 0 ]; then
	break;
    fi
done
#!/bin/sh

for i in `seq 1 1000`; do
    echo $i
    ./cpuclock2
    if [ $? -ne 0 ]; then
	break;
    fi
done
#!/bin/sh

mem=1m
loop=1000
last_node=0

for node in `seq 0 $last_node`; do
    for i in `seq 1 63`; do
	memhog -r$loop $mem membind $node &
    done
done

wait




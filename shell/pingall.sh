#!/bin/bash
cat /etc/hosts | grep -v '^#' | while read LINE
do
    ADDR=`awk '{print $1}' `  #do not known what's the meaming
    for machine in $ADDR
    do
        ping -s -c 1 $machine
    done
done

#!/bin/sh

exec 3>/tmp/lock_test
flock -x 3
while :
do
    sleep 1
    echo "hello"
done
exec 3>&-

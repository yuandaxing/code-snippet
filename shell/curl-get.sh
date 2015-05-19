#!/bin/sh

set1='curl -X GET http://tf12dg.prod.mediav.com:53001/param_repository/input_dimension_size\?input_dimension_size\=100'
set2='curl -X GET http://tf12dg.prod.mediav.com:53001/param_repository/input_dimension_size\?input_dimension_size\=101'
get='curl http://tf12dg.prod.mediav.com:53001/input_dimensions'

for i in {1..1000};
do
    if [ "`expr $i % 2`" -eq "1" ]
    then
        eval $set1
    else
        eval $set2
    fi
    sleep 1
    eval $get > "$i.txt"
    sleep 10
done

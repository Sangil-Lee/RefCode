#!/bin/sh

a=`cat $PWD/data2.txt`

for i in $a
do

    caput ECH_WRE04_INP $i

    sleep 5

#    caget ECH_WRD13_CALC >> result.txt

done

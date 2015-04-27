#!/bin/sh

a=`cat $PWD/data.txt`

for i in $a
do

    caput ECH_WRE02_INP $i

    sleep 3

#    caget ECH_WRD13_CALC >> result.txt

done

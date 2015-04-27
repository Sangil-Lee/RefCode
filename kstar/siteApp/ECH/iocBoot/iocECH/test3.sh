#!/bin/sh

a=`cat $PWD/data3.txt`

for i in $a
do

    caput ECH_WRE05 $i
    caput ECH_WRE06 $i
    caput ECH_WRE07 $i

    sleep 5

#    caget ECH_WRD13_CALC >> result.txt

done

#!/bin/bash

if [ $# -eq 2 ];then
    values_n=$1;
    procesors_n=$2;
else
    echo "Usage: test.sh values_count processors_n";
    exit 1;
fi;

dd if=/dev/random bs=1 count=$values_n of=numbers

mpic++ --prefix /usr/local/share/OpenMPI -lm -o mss mss.cpp

mpirun --prefix /usr/local/share/OpenMPI -np $procesors_n mss $values_n

rm -f mss numbers


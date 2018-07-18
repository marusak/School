#!/bin/bash

if [ $# -eq 1 ];then
    vers=$1;
else
    echo "Usage: test.sh VERTICES";
    exit 1;
fi;

mpic++ --prefix /usr/local/share/OpenMPI -lm -o pro pro.cpp

len=${#vers}
procs=$((2*(len-1)))

mpirun --prefix /usr/local/share/OpenMPI -n $procs pro $vers

rm -f pro


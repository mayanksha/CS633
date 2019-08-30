#!/bin/bash

# Remove the older data files
make clean
./create_hostfile.sh 1
if [ $? -ne 0 ]
then
    echo "Some problem with create the hostfile! Exiting!"
    exit -1
fi

# Compile the src.c file
make
if [ $? -ne 0 ]
then
    echo "Some problem with make command! Exiting!"
    exit -1
fi

# Truncate the data file
printf "" > data.txt
for i in `seq 5`;
do
    mpiexec -f mpich.hostfile ./src.x >> data.txt
    if [ $? -ne 0 ]
    then
        echo "mpiexec command failed for iteration = ${i}! Exiting!"
        exit -1
    fi
done

python3 ./plot.py
if [ $? -ne 0 ]
then
    echo "Failed while plotting! Exiting!"
    exit -1
fi

exit 0

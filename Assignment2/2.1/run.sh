#!/bin/bash

set -x
data_sizes=(65536 524288 2097152)
# data_sizes=(65536)
# Remove the older data files
make clean 2>&1 > /dev/null

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
for i in "${data_sizes[@]}"
do
    time_now=$(date +%R@%F)
    filtered_name="./data/filtered-data-${i}B"
    unfiltered_name="./data/unfiltered-data-${i}B"

    printf "" > $filtered_name
    printf "" > $unfiltered_name

    export ENABLE_FILTER=1
    mpiexec -f ./mpich.hostfile -ppn 1 ./src.x ${i} >> "$filtered_name"
    if [ $? -ne 0 ]
    then
        echo "mpiexec command failed!"
        exit -1
    fi

    unset ENABLE_FILTER
    mpiexec -f mpich.hostfile -ppn 1 ./src.x ${i} >> "$unfiltered_name"
    if [ $? -ne 0 ]
    then
        echo "mpiexec command failed!"
        exit -1
    fi
    python ./plot.py ${filtered_name} "plot-${i}.png"
    python ./plot.py ${unfiltered_name} "plot-${i}-non-zeroed.png"
done
exit 0

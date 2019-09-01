#!/bin/bash

# Remove the older data files
make clean 2>&1 > /dev/null
num_nodes=(2 4 8)
for n in "${num_nodes[@]}"
do
    ./create_hostfile.sh ${n} 4
    if [ $? -ne 0 ]
    then
        echo "Some problem with create the hostfile! Exiting!"
        exit -1
    fi
    cat ./mpich.hostfile
    # Compile the src.c file
    make
    if [ $? -ne 0 ]
    then
        echo "Some problem with make command! Exiting!"
        exit -1
    fi

    # Truncate the data file
    for i in `seq 5`;
    do
        filename="data-${n}-${i}.txt"
        printf "" > $filename
        echo "Now running for ${n} nodes (iteration #${i})"
        mpiexec -f mpich.hostfile ./src.x >> $filename
        if [ $? -ne 0 ]
        then
            echo "mpiexec command failed for iteration = ${i}! Exiting!"
            exit -1
        fi
    done
done
python3 plot.py
exit 0

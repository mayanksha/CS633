#!/bin/bash

set -x
proc_sizes=(8 16 32)
ppn=(2 4 8)
data_sizes=(65536 262144 524288 2097152 4194304)
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

for i in "${proc_sizes[@]}"
do
    # time_now=$(date +%R@%F)
    filename="./data/data-NP-${i}"
    printf "" > $filename
    for j in "${ppn[@]}"
    do
        for k in "${data_sizes[@]}"
        do
            for l in `seq 10`
            do
                out=$(mpiexec -f ./mpich.hostfile -ppn ${j} -np ${i} ./src.x ${k})
                printf "${k},${j},${out},${i}\n" >> ${filename}
                if [ $? -ne 0 ]
                then
                    echo "mpiexec command failed!"
                    exit -1
                fi
            done
        done
    done
    python plot.py ${filename}
    echo "Plotted graphs for P=${i}"
done
exit 0

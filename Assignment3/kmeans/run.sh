#!/bin/bash

# set -x
# data_sizes=(65536)
# Remove the older data files
make clean 2>&1 > /dev/null

./create_hostfile.sh 12
if [ $? -ne 0 ]
then
    echo "Some problem with create the hostfile! Exiting!"
    exit -1
fi

# Compile the src.c file
make -f "Makefile"
if [ $? -ne 0 ]
then
    echo "Some problem with make command! Exiting!"
    exit -1
fi

mkdir - cse
dataset_1_path="./data1/"
dataset_2_path="./data2/"

n_procs=(1 2 3 4 5 6 7 8 9 10)
n_hosts=(1 2 3)
# Truncate the data file
for i in "${n_hosts[@]}"
do
    for j in "${n_procs[@]}"
    do
        total_proc=$(( $i * $j ))\n
        mpiexec -f mpich.hostfile -np "${total_proc}" -ppn "${j}" ./src.x > "cse/output_${total_proc}"
    done
done
exit 0

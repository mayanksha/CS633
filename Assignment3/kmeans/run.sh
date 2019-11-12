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

mkdir -p cse
dataset_1_files=`ls ./data1/file*`
dataset_2_files=`ls ./data2/file*`

n_procs=(1 2 3 4 5 6 7 8 9 10)
n_hosts=(1 2 3)
KVal="10"
# Truncate the data file
for i in "${n_hosts[@]}"
do
    for j in "${n_procs[@]}"
    do
        total_proc=$(( $i * $j ))\n
        out_file="cse/output_${total_proc}.txt"
        printf "" > "$out_file"
        for k in "${dataset_1_files[@]}"
        do
            mpiexec -f mpich.hostfile -np "${total_proc}" -ppn "${j}" ./src.x "${k}" ${KVal} \
                >> "${out_file}"
        done
    done
done
exit 0

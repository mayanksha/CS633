#!/bin/bash

set -x
# data_sizes=(65536)
# Remove the older data files
make clean 2>&1 > /dev/null

# Compile the src.c file
make -f "Makefile"
if [ $? -ne 0 ]
then
    echo "Some problem with make command! Exiting!"
    exit -1
fi

mkdir -p cse
rm -rf cse/*
dataset_1_files=($(ls ./data1/file*))
dataset_2_files=($(ls ./data2/file*))

n_procs=(1 2 3 4 5 6 7 8 9 10)
n_hosts=(1 2 3)
KVal="90"
# Truncate the data file
for i in "${n_hosts[@]}"
do
    for j in "${n_procs[@]}"
    do
        total_proc=$(( $i + $j ))
        temp_out_file="cse/temp_output_${total_proc}.txt"
        out_file="cse/output_${total_proc}.txt"
        printf "" > "$temp_out_file"
        printf "" > "$out_file"

        for k in "${dataset_1_files[@]}"
        do
            echo "${k}"
            mpiexec -f mpich.hostfile -np "${total_proc}" -ppn "${j}" ./src.x "${k}" ${KVal} >> "${temp_out_file}"
        done

        head -n1 "${temp_out_file}" >> "${out_file}"
        sed -n '5~5p' "${temp_out_file}" >> "${out_file}"
        total_times=($(sed -n '2~5p' ${temp_out_file} | grep -oP ' \K.*'))
        total_avg=$(python -c  'import sys; print(sum([float(i) for i in sys.argv[1:]])/len(sys.argv[1:]))' "${total_times[@]}")
        proc_times=($(sed -n '3~5p' ${temp_out_file} | grep -oP ' \K.*'))
        proc_avg=$(python -c  'import sys; print(sum([float(i) for i in sys.argv[1:]])/len(sys.argv[1:]))' "${proc_times[@]}")
        pre_proc_times=($(sed -n '4~5p' ${temp_out_file} | grep -oP ' \K.*'))
        pre_proc_avg=$(python -c  'import sys; print(sum([float(i) for i in sys.argv[1:]])/len(sys.argv[1:]))' "${pre_proc_times[@]}")

        echo "Total Time: ${total_avg}">> "${out_file}"
        echo "Processing Time: ${proc_avg}">> "${out_file}"
        echo "Pre-Processing Time: ${pre_proc_avg}">> "${out_file}"
    done
done
exit 0

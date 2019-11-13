#!/bin/bash

set -x
set -e

./create_hostfile.sh 8
if [ $? -ne 0 ]
then
    echo "Some problem with create the hostfile! Exiting!"
    exit -1
fi

# data_sizes=(65536)
# Remove the older data files
make -f "Makefile.cse" clean 2>&1 > /dev/null

# Compile the src.c file
make -f "Makefile.cse"
if [ $? -ne 0 ]
then
    echo "Some problem with make command! Exiting!"
    exit -1
fi

rm -rf cse/*
data_folders=("data1" "data2")

n_hosts=(1 2 4 8 16 32)
KVal="10"
# Truncate the data file

for data_folder in "${data_folders[@]}"
do
    KVal=""
    if [ "${data_folder}" = "data1" ]
    then
        KVal="90"
    else
        KVal="11"
    fi

    dataset_files=($(ls ./${data_folder}/file*))
    for i in "${n_hosts[@]}"
    do
        total_proc=$(( $i ))
        mkdir -p "./cse/${data_folder}/"

        for n_iter in `seq 5`
        do
            temp_out_file="./cse/${data_folder}/temp_output_${total_proc}_${n_iter}"
            out_file="./cse/${data_folder}/output_${total_proc}_${n_iter}"
            printf "" > "$temp_out_file"
            printf "" > "$out_file"
            echo "$temp_out_file"
            echo "$out_file"


            for k in "${dataset_files[@]}"
            do
                mpiexec -f mpich.hostfile -np "${total_proc}" -ppn "8" ./src.x "${k}" ${KVal} >> "${temp_out_file}"
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
done
python3 plot.py cse/
exit 0

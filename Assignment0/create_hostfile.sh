#!/bin/bash
if [ $# -ne 1 ]
then
	echo "This script takes a single argument - the number of processes to run on a single node."
	exit -1
fi

n_proc=$1
hostfile_name="mpich.hostfile"

check_host () {
	local run=$1
	ping -c1 csews${1} 2>&1 > /dev/null
	if [ $? -eq 0 ]
	then
		echo "csews${1}:$n_proc" | tee -a ${hostfile_name} 
	fi
}

echo "# This is an automatically created hostfile using create_hostfile.sh script" > ${hostfile_name}
for i in `seq 27 39`; do check_host ${i} & done


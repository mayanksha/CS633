#!/bin/bash
if [ $# -ne 1 ]
then
	echo "This script takes a single argument - the number of processes to run on a single node."
	exit -1
fi

n_proc=$1
hostfile="mpich.hostfile"
hostnum=$(echo ${HOSTNAME} | grep -Po '\d+')
hosts_bucket=$(seq 27 39)
actual_hosts=()

for i in $hosts_bucket
do
    if [ "${hostnum}" -ne "${i}" ]
    then
        actual_hosts+=(${i})
    fi
done

check_host () {
	local run=$1
	ping -c1 csews${1} 2>&1 > /dev/null
	if [ $? -eq 0 ]
	then
		# echo "csews${1}:$n_proc" | tee -a ${hostfile}
		echo "csews${1}:$n_proc" >> ${hostfile}
	fi
}

echo "# This is an automatically created hostfile using create_hostfile.sh script" > ${hostfile}
for i in ${actual_hosts[@]}
do
    check_host ${i} &
done

for job in `jobs -p`
do
    wait $job
done

echo `tail -n1 ${hostfile}` > ${hostfile}
echo "${HOSTNAME}:${n_proc}" >> ${hostfile}

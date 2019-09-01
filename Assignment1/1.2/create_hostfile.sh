#!/bin/bash
if [ $# -ne 2 ]
then
	echo "This script takes two positional arguments - The number of nodes and the number of processes to run on a single node, respectively."
	exit -1
fi

n_proc=$2
n_nodes=$1
hostfile="mpich.hostfile"
hostnum=$(echo ${HOSTNAME} | grep -Po '\d+')
hosts_bucket=$(seq 20 39)
actual_hosts=()

for i in $hosts_bucket
do
    if [[ ( "${hostnum}" -ne "${i}" ) && ( "29" -ne "${i}" ) ]]
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
		printf "csews${1}:$n_proc\n" >> ${hostfile}
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

hosts_to_write=$(tail -n $((${n_nodes} - 1)) ${hostfile})
echo "# This is an automatically created hostfile using create_hostfile.sh script" > ${hostfile}
for i in ${hosts_to_write[@]}
do
    echo "${i}" >> ${hostfile}
done
echo "${HOSTNAME}:${n_proc}" >> ${hostfile}

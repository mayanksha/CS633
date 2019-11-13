#!/bin/bash
if [ $# -ne 1 ]
then
	echo "This script takes a single argument - the number of processes to run on a single node."
	exit -1
fi

n_proc=$1
hostfile="mpich.hostfile"
hostnum=$(echo ${HOSTNAME} | grep -Po '\d+')
hosts_bucket=$(cat ../hosts)
actual_hosts=()

#for i in $hosts_bucket
#do
#    if [ "${hostnum}" -ne "${i}" ]
#    then
#        actual_hosts+=(${i})
#    fi
#done

check_host () {
	ping -c1 ${1} 2>&1 > /dev/null
	if [ $? -eq 0 ]
	then
		# echo "csews${1}:$n_proc" | tee -a ${hostfile}
		echo "${i}:$n_proc" >> ${hostfile}
	fi
}

echo "# This is an automatically created hostfile using create_hostfile.sh script" > ${hostfile}
for i in ${hosts_bucket[@]}
do
    check_host ${i} &
done

for job in `jobs -p`
do
    wait $job
done

hosts=$(tail -n15 ${hostfile})
printf "" > ${hostfile}
for i in ${hosts[@]}
do
    echo ${i} >> ${hostfile}
done

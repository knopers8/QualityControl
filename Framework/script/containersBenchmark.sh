#!/usr/bin/env bash
#set -e ;# exit on error
set -u ;# exit when using undeclared variable
#set -x ;# debugging

containers=(map tmap tobjarray thashlist)
mo_counts=(1 3 10 33 100)
time=$1

if [ -d tcontainers_results.txt ] ; then
  rm tcontainers_results.txt
fi

for container in ${containers[*]}
do
  for mo_count in ${mo_counts[*]}
  do
    echo ${container}' '${mo_count}
    qcRunContainerBenchmark -q -b --type ${container} --mo-count ${mo_count} --time ${time}
  done
done

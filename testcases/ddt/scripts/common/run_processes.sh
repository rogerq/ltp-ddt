#!/bin/sh
# Execute multiple processes in parallel
# returns 1 if any process returns non-zero value
# returns 0 otherwise
# usage: run_process.sh "#-separated commands" [num instances] 
#                        [cpu affinity mask] [inter-process start delay]

pids=''
RET=0
OFS=$IFS
IFS="#"
tmp_dir=`mktemp -d`
i=0

# Check for number of instances
p_instances=1
if [ "x$2" != "x" ]
then
  p_instances=$2
fi

# Check for CPU affinity mask
p_mask='0xFFFFFFFF'
if [ "x$3" != "x" ]
then
  p_mask=$3
fi

# Check for inter-process start delay
p_delay=1
if [ "x$4" != "x" ]
then
  p_delay=$4
fi

# Start processes
for w in $1 
do
  for (( j=0 ; j < $p_instances ; j++ ))
  do 
    echo "starting $w instance $j" 
    sleep $p_delay
    c="taskset $p_mask $w"
    eval "$c" > $tmp_dir/log$i.$j.tmp 2>&1 &
    pids="$pids:$!"
  done
  i=`expr $i + 1`
done
cnt=$i

IFS=':'

# Wait for all process to complete and check return value of process
for p in $pids
do
  if [ "x$p" != "x" ]
  then
    wait ${p}
    rc=$?
    if [ "$rc" -ne "0" ]
    then
      RET=1
      break
    fi
  fi
done

IFS=$OFS

# Print logs in console
i=0
while [ $i -lt $cnt ]
do
  for (( j=0 ; j < $p_instances ;  j++ ))
  do
    echo "*************  $tmp_dir/log$i.$j.tmp    ***************"
    cat $tmp_dir/log$i.$j.tmp
    echo "*************end of $tmp_dir/log$i.$j.tmp**************"
    echo ""
  done
  i=`expr $i + 1`
done

exit $RET


#!/bin/sh
# Execute multiple run_processes.sh in parallel
# returns 1 if any process returns non-zero value
# returns 0 otherwise
# usage: multi_run_process.sh <process list> [<process list2> ...]"
#    where process_list= "#-separated commands" <num instances> 
#                        <cpu affinity mask> <inter-process start delay>

OFS=$IFS
RET=0
pids=''
i=0

# Start processes
for (( idx=1 ; $#-idx >= 3 ; idx+=4 ))
do
  eval p1=\$$idx
  eval p2=\$$((idx+1))
  eval p3=\$$((idx+2))
  eval p4=\$$((idx+3))
  run_processes.sh "$p1" $p2 $p3 $p4 & pids="$pids:$!"
done

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
      echo "************************************************"
      echo "Process $p exit with non-zero value"
      echo "************************************************"
      break
    fi
  fi
done

IFS=$OFS

exit $RET


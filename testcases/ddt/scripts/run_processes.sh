#!/bin/sh
# Execute multiple processes in parallel
# returns 1 if any process returns non-zero value
# returns 0 otherwise

pids=''
RET=0
i=0
OFS=$IFS
IFS="#"

# Start processes
for w in $1 
do
  eval "$w" > ./log$i.tmp 2>&1 &
  pids="$pids:$!"
  i=`expr $i + 1`
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
      break
    fi
  fi
done

IFS=$OFS
exit $RET


#!/bin/sh
# Execute multiple processes in parallel
# returns 1 if any process returns non-zero value
# returns 0 otherwise

pids=''
RET=0
OFS=$IFS
IFS="#"

# Start processes
i=0
for w in $1 
do
  sleep 1
  eval "$w" > ./log$i.tmp 2>&1 &
  pids="$pids:$!"
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
j=0
while [ $j -lt $cnt ]
do
  echo "*************  log$j.tmp    ***************"
  cat ./log$j.tmp
  echo "*************end of log$j.tmp**************"
  echo ""
  j=`expr $j + 1`
done

exit $RET


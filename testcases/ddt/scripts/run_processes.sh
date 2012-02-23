#!/bin/sh
# Execute multiple processes in parallel
# returns 1 if any process returns non-zero value
# returns 0 otherwise

pids=''
RET=0
i=0
OFS=$IFS
IFS="#"

echo "0" > ./$$  # Delete previous content
export retFile="$$"

# Start processes
for w in $1 
do
  `eval "$w" > ./log$i.tmp; echo "$?" >> ./$retFile` &
  pids="$pids:$!"
  i=`expr $i + 1`
done

echo "Debug: pids=$pids"

IFS=':'

# Wait for all process to complete
for p in $pids
do
  if [ "x$p" != "x" ]
  then
    wait $p
  fi
done

# Check return value
while read line
do
  if [ "$line" != "0" ]
  then
    RET=1     
    break
  fi
done < ./"$$"

# Cleanup
IFS=$OFS
rm ./$$
exit $RET


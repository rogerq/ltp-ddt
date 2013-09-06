#!/bin/sh
# Execute multiple run_processes.sh in parallel
# returns 1 if any process returns non-zero value
# returns 0 otherwise
# usage: 
# multi_run_processes.sh <process list> [<process list2> ...] -v
#   where        -v is an optional flag and when present indicates that priority
#                 of two processes has to be verified using time taken
#                 for process execution
#
#                 process_list= -s "#-separated commands" -l "-n <num_instances> 
#                        -a <cpu affinity mask> -d <inter-process start delay>
#                        -p <priority>" 

OFS=$IFS
RET=0
pids=''
i=0
COMMANDS_SET=0
PARAMS_SET=0

while getopts ":s:l:" opt; do                                            
  echo "OPT is $opt"
  case $opt in                                                                 
   s)                                                                          
      echo "-s was triggered, Parameter: $OPTARG" >&2                  
      COMMANDS_SET=1                                                           
      s_command=$OPTARG                                                 
      s_commands+=( "$s_command" )
   ;;                                                        
   l)                                                        
      echo "-l was triggered, Parameter: $OPTARG" >&2
      l_command=$OPTARG                                        
      l_commands+=( "$l_command" )
    ;;                  
  esac
 done
 j=0
 for i in "${s_commands[@]}"
 do
   :
   cmd=${l_commands[$j]}
   run_processes.sh -c "$i" ${cmd[@]} & pids="$pids:$!"
   j=$j+1
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
      echo "Process $p exit with non-zero value at time " `date`
      echo "************************************************"
      break
    fi
  fi
done

IFS=$OFS
echo "Return is "$RET
exit $RET


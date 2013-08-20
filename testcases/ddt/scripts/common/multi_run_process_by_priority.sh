#!/bin/sh
# Execute multiple run_process_by_priority.sh in parallel
# returns 1 if any process returns non-zero value
# returns 0 otherwise
# usage: 
# multi_run_process_by_priority.sh <process list> [<process list2> ...] -v
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
VERIFY_SET=0
verify_flag=0

while getopts ":s:l:v" opt; do                                            
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
   v)                                                        
      echo "-v was triggered, Parameter: $OPTARG" >&2
      if [[ $VERIFY_SET = 0 ]]; then
        VERIFY_SET=1
        verify_flag=1                                      
      else                                                      
        echo "Option -commmands already was used."              
        exit 1                                                  
      fi        
    ;;                  
  esac
 done
 j=0
 for i in "${s_commands[@]}"
 do
   :
   cmd=${l_commands[$j]}
   val=`run_process_by_priority.sh -c "$i" ${cmd[@]} & pids="$pids:$!"`
   if [ $verify_flag = 1 ] 
   then
   # store return in an array
     echo "ENTERED verify_priority"
     val_array+=( "$val" )
     if [ ${#val_array[@]} = 2 ] # time to compare the two entries
     then
      OFS=$IFS  
      IFS=' ' 
      for (( j=0 ; j < 2 ; j++ ))
      do
        i=0
        for p in ${val_array[j]}  
        do            
          if [ $i = 0 ] 
          then
            priority[j]=$p 
          else
            system_time[j]=$p
          fi
          i+=1
        done
      done
      IFS=$OFS
      res=`expr ${system_time[0]} \> ${system_time[1]}`
      if [  ${priority[0]} -gt ${priority[1]} ]  
      then
        echo "${priority[0]} is > ${priority[1]}"
        if [ $res = 1 ]
        then
          echo "Success ${system_time[0]} is > ${system_time[1]}"
          exit 0
        else
          echo "Failure ${system_time[1]} is > ${system_time[0]}"
          exit 1
        fi
      else 
        echo "${priority[1]} is > ${priority[0]}"
        if [ $res = 1 ]
        then
          echo "Failure ${system_time[0]} is < ${system_time[1]}"
          exit 1
        else
          echo "Success ${system_time[1]} is > ${system_time[0]}"
          exit 0
        fi
      fi
     fi
   fi
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
echo "Return is "$rc
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


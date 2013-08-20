#!/bin/sh
# Execute multiple processes in parallel
# returns 1 if any process returns non-zero value
# returns 0 otherwise
## usage: run_process_by_priority.sh "#-separated commands" [num instances] 
##                        [cpu affinity mask] [inter-process start delay]
# usage: run_process_by_priority.sh -c <"#-separated commands"> -n <num_of_instances> 
#                                                  -a <cpu_affinity_mask>
#                                                  -d <delay_in_sec> 
#                                                  -p <priority> 
pids=''
RET=0
OFS=$IFS
IFS="#"
tmp_dir=`mktemp -d`
i=0

COMMANDS_SET=0
INSTANCE_SET=0
AFFINITY_SET=0
DELAY_SET=0
PRIORITY_SET=0


p_instances=1
p_mask='0xFFFFFFFF'
p_delay=1
p_priority=0
while getopts ":c:n:a:d:p:" opt; do
  #echo "Opt is "$opt
  case $opt in
   c)
    if [[ $COMMANDS_SET = 0 ]]; then
      #echo "-commmands was triggered, Parameter: $OPTARG" >&2
      COMMANDS_SET=1
      p_commands=$OPTARG
    else
      echo "Option -commmands already was used."
      exit 1
    fi
   ;;
   n)
    if [[ $INSTANCE_SET = 0 ]]; then
      #echo "-num_instances was triggered, Parameter: $OPTARG" >&2
      INSTANCE_SET=1
      p_instances=$OPTARG
    else
      echo "Option -num_instances already was used."
      exit 1
    fi
    ;;
   a)
    if [[ $AFFINITY_SET = 0 ]]; then
      #echo "-cpu_affinity_mask was triggered, Parameter: $OPTARG" >&2
      AFFINITY_SET=1
      p_mask=$OPTARG
    else
      echo "Option -cpu_affinity_mask was already used."
      exit 1
    fi
    ;;
   d)
    if [[ $DELAY_SET = 0 ]]; then
      #echo "-inter_process_start_delay was triggered, Parameter: $OPTARG" >&2
      DELAY_SET=1
      p_delay=$OPTARG
    else
      echo "Option -inter_process_start_delay was already used."
      exit 1
    fi
    ;;
   p)
    if [[ $PRIORITY_SET = 0 ]]; then
      #echo "-task_priority was triggered, Parameter: $OPTARG" >&2
      PRIORITY_SET=1
      p_priority=$OPTARG
    else
      echo "Option -task_priority was already used."
      exit 1
    fi
    ;;
   \?)
    echo "Invalid option: -$OPTARG" >&2
    echo "Syntax is -c <#-separated commands> -n <num_of_instances> -a <cpu_affinity_mask> -d <inter_process_start_delay in sec> -p <task_priority>"
    ;;
  esac
done

# Start processes
for w in $p_commands 
do
  for (( j=0 ; j < $p_instances ; j++ ))
  do 
    sleep $p_delay
    c="taskset $p_mask $w"
    eval "$c" > $tmp_dir/log$i.$j.tmp 2>&1 &
    process_id=$!
    typeset hash_${process_id}=$p_priority
    #echo "$(eval echo \$hash_${process_id})"
#    eval "$c" > $tmp_dir/log$process_id.tmp 2>&1 &
    #echo "PROCESS ID is $process_id"
    #echo "PRIORITY for $process_id before "`awk '{print $18}' /proc/$process_id/stat`  
    d="renice $p_priority -p $process_id"
    output_str=`eval "$d"` 
    #echo "RENICE DONE PPPPPPPPPPPPPPPPPPPPPROCESS ID" $process_id
    #echo "PRIORITY for $process_id after "`awk '{print $18}' /proc/$process_id/stat`  
    pids="$pids:$!"
    pid_table+=( "$process_id" )
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
         echo "*****************From run_processes.sh***********************"
      echo "Process $p exit with non-zero value at time " `date`                    
      echo ""
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
    process_id=${pid_table[$j]}
    #echo $process_id
    #echo "*************  $tmp_dir/log$i.$j.tmp    ***************"
    #echo "$(eval echo \$hash_${process_id})"
    match_string=`cat $tmp_dir/log$i.$j.tmp|grep "sys*m*s"` 
    if [ ${#match_string} != 0 ]
    # this command has system time being printed in output, so collect it for calling script
    then
    new_string=`echo $match_string | cut -c5-`
    ret_val[0]="$(eval echo \$hash_${process_id})"
    ret_val[1]="$new_string"
    echo ${ret_val[@]}
    fi
  done
  i=`expr $i + 1`
done
exit $RET

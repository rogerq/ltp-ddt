#!/bin/sh
# verify_priority.sh 
# compares system time for processes with respect to priority 
#      assigned to those processes
# takes log as input - log is multi-line string output of multi_run_processes.sh
# returns 0 if execution time is according to priority set
# exits with failure message when fail condition is encountered
# failure could be regarding system time not taking into account
# priorities
# failure will be reported if processes did not start with unique priorities
# or result in non-unique execution time
# usage:  verify_priority.sh <log>

source "common.sh"  # Import do_cmd(), die() and other functions
log=$1
result=1
comparison_done=0

# Read log to create the priorities and sys time available and
#    create arrays for each
while read line 
do
  if echo "$line"|grep "Task priority"; then
    echo "Found priority line"
    priority=$( echo "$line"|awk -F'Task priority is ' '{print $2;}' )
    priority_table+=("$priority") 
  elif echo "$line"|grep "sys "; then
    echo "Found time line"
    time=$( echo "$line"|awk -F'sys ' '{print $2;}' )
    time_table+=("$time")
  fi
done <<< "$log"
echo "Priority Table is ${priority_table[@]}"
echo "System Time Table is ${time_table[@]}"

# Check that number of priority strings in log is equal to sys time strings
if [ ${#priority_table[@]} != ${#time_table[@]} ]
then
  die "Mismatch in log file - number of priority tasks did not match system time output entries"
fi

# Check that there are no duplicates in priority or sys time entries
for (( i=0;i < ${#time_table[@]};i++ ))                           
do                                                                
  for (( j=0;j < ${#time_table[@]};j++ ))        
  do                                             
   if [ ${priority_table[$i]} == ${priority_table[$j]} ] || [ ${time_table[$i]} == ${time_table[$j]} ]
   then                                                                        
    if [ $i == $j ]                                                            
    then                                                                       
      break                                                                    
    fi                                                                         
    die "Duplicate priority or system time found - double-check test parameters for unique priorities or re-run test"                                                   
    exit                                                                        
   fi                                                                           
  done                                                                          
done


while [ $comparison_done != 1 ]
do
#  echo "COMPARISON_DONE is $comparison_done"
#  echo "Table is ${priority_table[@]}"
#  echo "Table is ${time_table[@]}"

# Sorting the priority table
  min=-20 max=-20
  index=0
  min_index=0 max_index=0
  for i in ${priority_table[@]}; do
    (( $i > max || max == -20)) && max=$i && max_index=$index
    (( $i < min || min == -20)) && min=$i && min_index=$index
    index=`expr $index + 1`
  done
#  echo "min=$min, min_index=$min_index
#  max=$max, max_index=$max_index"

# Sorting the system time table which has floating point elements
  index=0
  min_time=0.0
  max_time=0.0
  min_time_index=0
  max_time_index=0
  for i in ${time_table[@]}; do
    var1=`echo "$i > $max_time" | bc`
    var2=`echo "$max_time == 0.0" | bc`
    (( $var1 == 1 || $var2 == 1)) && max_time=$i && max_time_index=$index
    var1=`echo "$i < $min_time" | bc`
    var2=`echo "$min_time == 0.0" | bc`
    (( $var1 == 1 || $var2 == 1)) && min_time=$i && min_time_index=$index
    index=`expr $index + 1`
  done
#  echo "min_time=$min_time, min_time_index=$min_time_index
#  max_time=$max_time, max_time_index=$max_time_index"
  if [ $min_index == $min_time_index ] && [ $max_index == $max_time_index ] 
  then
 #So far so good and can continue with rest of the elements in the array
 # make the array smaller or if total array length is less than or equal to three, result=0
 #   echo "LENGTH is ${#priority_table[@]} Entered so far so good"

 # nothing more to be done since we are either left with no more elements
#    or only one more, so nothing more to compare
    if [ ${#priority_table[@]} -le 3 ]
    then
      result=0
      comparison_done=1
 #     echo "Entered here also since table is only 2 or 3 elements long now"
      break
    else
 #    removing the min and max found above and continuing same operation on rest of the array
      priority_table=(${priority_table[@]:0:$max_index} ${priority_table[@]:$(($max_index + 1))})
      priority_table=(${priority_table[@]:0:$min_index} ${priority_table[@]:$(($min_index + 1))})
      time_table=(${time_table[@]:0:$max_time_index} ${time_table[@]:$(($max_time_index + 1))})
      time_table=(${time_table[@]:0:$min_time_index} ${time_table[@]:$(($min_time_index + 1))})
 #     echo ${priority_table[@]}
 #     echo ${time_table[@]}
    fi
  else
    result=1
    comparison_done=1
    die "Priority check failed"
    break
  fi
done
echo $result 

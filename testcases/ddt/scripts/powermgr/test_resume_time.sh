#! /bin/sh
# 
# Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/
#  
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License as 
# published by the Free Software Foundation version 2.
# 
# This program is distributed "as is" WITHOUT ANY WARRANTY of any
# kind, whether express or implied; without even the implied warranty
# of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
#
# This script is to use to test if the resume time is consistent 

source "functions.sh"

############################# Functions #######################################
usage()
{
cat <<-EOF >&2
        usage: ./${0##*/} [-p power_state] [-t max_stime] [-i test_iteration] 
        -p power_state  power state like 'mem' or 'standby'
        -t max_stime    maximum suspend or standby time; the suspend time will be a random number
        -i t_iteration  iterations to suspend/resume
        -h help         print this usage
EOF
exit 0
}

############################### CLI Params ###################################
while getopts  :p:t:i:h arg
do case $arg in
        p)  power_state="$OPTARG";;
        t)  max_stime="$OPTARG";;
        i)  t_iteration="$OPTARG";;
        h)  usage;;
        :)  test_print_trc "$0: Must supply an argument to -$OPTARG." >&2
            exit 1
            ;;
        \?) test_print_trc "Invalid Option -$OPTARG ignored." >&2
            usage
            exit 1
            ;;
esac
done

############################ DEFAULT Params #######################
: ${t_iteration:='2'}
: ${power_state:='mem'}
: ${max_stime:='10'}
test_print_trc "power_state: $power_state"
test_print_trc "max_stime: $max_stime"
test_print_trc "t_iteration: $t_iteration"

############# Do the work ###########################################
expect="PM: resume of devices complete after"

# The diff between different resume time should not exceed $diff_threshold 
diff_threshold="10"

# The max resume time in ms the test can accept.
# The resume time may be different for different kernel and it also be
# different for different power state 'mem' or 'standby', etc. The resume time
# threshold we set here is not accurate; but by using this, we can at least
# to pick out the big failure.
r_time_threshold="500"

i=0
while [ $i -lt $t_iteration ]; do
  test_print_trc "===test_resume_time iteration $i==="
  dmesg -c > /dev/null
  suspend -p "$power_state" -t "$max_stime" -i 1
  r_time=`dmesg |grep -i "$expect" |cut -d" " -f8`
  if (( $(echo "$r_time > $r_time_threshold" | bc -l) )); then
    die "It takes too long to resume" 
  fi
 
  if [ $i -eq 0 ]; then
    old_time="$r_time"
  else 
    new_time="$r_time"
    diff=`echo "$new_time - $old_time" |bc`
    test_print_trc "The resume time diff is $diff"
    diff_abs=`echo ${diff#-}`
    test_print_trc "The absolute value of resume time diff is $diff_abs"
    if (( $(echo "$diff_threshold > $diff" | bc -l) )); then
      test_print_trc "iteration $i: pass"
    else
      die "The difference between pre resume time $old_time and new resume time $new_time is too big and the resume time is not consistent"
    fi
    old_time=$new_time
  fi
  i=`expr $i + 1`
done


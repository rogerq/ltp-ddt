#!/bin/bash
#
# PM-QA validation test suite for the power management on Linux
#
# Copyright (C) 2011, Linaro Limited.
# Copyright (C) 2013 Texas Instruments Incorporated - http://www.ti.com/
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
#
# Contributors:
#     Daniel Lezcano <daniel.lezcano@linaro.org> (IBM Corporation)
#       - initial API and implementation
#     Carlos Hernandez <ceh@ti.com>
#       - Add new functions
#

source "common.sh"     # include ltp-ddt common functions

CPU_PATH="/sys/devices/system/cpu"
TEST_NAME=$(basename ${0%.sh})
PREFIX=$TEST_NAME
INC=0
CPU=

if [ -f /sys/power/wake_lock ]; then
    use_wakelock=1
else
    use_wakelock=0
fi

log_begin() {
    printf "%-76s" "$TEST_NAME.$INC$CPU: $@... "
    INC=$(($INC+1))
}

log_end() {
    printf "$*\n"
}

log_skip() {
    log_begin "$@"
    log_end "skip"
}

check() {

    local descr=$1
    local func=$2
    shift 2;

    log_begin "checking $descr"

    $func $@
    if [ $? != 0 ]; then
    log_end "fail"
    return 1
    fi

    log_end "pass"

    return 0
}

check_file() {
    local file=$1
    local dir=$2

    check "'$file' exists" "test -f" $dir/$file
}


for_each_cpu() {

    local func=$1
    shift 1

    cpus=$(ls $CPU_PATH | grep "cpu[0-9].*")

    for cpu in $cpus; do
	INC=0
	CPU=/$cpu
	$func $cpu $@
    done

    return 0
}

get_num_cpus() {
    cpus=$(ls $CPU_PATH | grep "cpu[0-9].*")
    echo ${#cpus[@]}
}

for_each_governor() {

    local cpu=$1
    local func=$2
    local dirpath=$CPU_PATH/$cpu/cpufreq
    local governors=$(cat $dirpath/scaling_available_governors)
    shift 2

    for governor in $governors; do
	$func $cpu $governor $@
    done

    return 0
}

for_each_frequency() {

    local cpu=$1
    local func=$2
    local dirpath=$CPU_PATH/$cpu/cpufreq
    local frequencies=$(cat $dirpath/scaling_available_frequencies)
    shift 2

    for frequency in $frequencies; do
	$func $cpu $frequency $@
    done

    return 0
}

set_governor() {

    local cpu=$1
    local dirpath=$CPU_PATH/$cpu/cpufreq/scaling_governor
    local newgov=$2

    echo $newgov > $dirpath
}

get_governor() {

    local cpu=$1
    local dirpath=$CPU_PATH/$cpu/cpufreq/scaling_governor

    cat $dirpath
}

wait_latency() {
    local cpu=$1
    local dirpath=$CPU_PATH/$cpu/cpufreq
    local latency=
    local nrfreq=

    latency=$(cat $dirpath/cpuinfo_transition_latency)
    if [ $? != 0 ]; then
	return 1
    fi

    nrfreq=$(cat $dirpath/scaling_available_frequencies | wc -w)
    if [ $? != 0 ]; then
	return 1
    fi

    nrfreq=$((nrfreq + 1))
    ../utils/nanosleep $(($nrfreq * $latency))
}

frequnit() {
    local freq=$1
    local ghz=$(echo "scale=1;($freq / 1000000)" | bc -l)
    local mhz=$(echo "scale=1;($freq / 1000)" | bc -l)

    res=$(echo "($ghz > 1.0)" | bc -l)
    if [ "$res" = "1" ]; then
	echo $ghz GHz
	return 0
    fi

    res=$(echo "($mhz > 1.0)" | bc -l)
    if [ "$res" = "1" ];then
	echo $mhz MHz
	return 0
    fi

    echo $freq KHz
}

set_frequency() {

    local cpu=$1
    local dirpath=$CPU_PATH/$cpu/cpufreq
    local newfreq=$2
    local setfreqpath=$dirpath/scaling_setspeed

    echo $newfreq > $setfreqpath
    wait_latency $cpu
}

get_frequency() {
    local cpu=$1
    local dirpath=$CPU_PATH/$cpu/cpufreq/scaling_cur_freq
    cat $dirpath
}

get_max_frequency() {
    local cpu=$1
    local dirpath=$CPU_PATH/$cpu/cpufreq/scaling_max_freq
    cat $dirpath
}

get_min_frequency() {
    local cpu=$1
    local dirpath=$CPU_PATH/$cpu/cpufreq/scaling_min_freq
    cat $dirpath
}

set_online() {
    local cpu=$1
    local dirpath=$CPU_PATH/$cpu
    echo 1 > $dirpath/online
    report "$cpu online"
}

set_offline() {
    local cpu=$1
    local dirpath=$CPU_PATH/$cpu
    echo 0 > $dirpath/online
    report "$cpu offline"
}

get_online() {
    local cpu=$1
    local dirpath=$CPU_PATH/$cpu

    cat $dirpath/online
}

# Online/offline CPU1 or higher - mess with governor
cpu_online_random()
{
    local num_cpu=`get_num_cpus`
    local random_cpu=cpu`random_ne0 $num_cpu`
    local k=`random 1`
    if [ -f $CPU_PATH/$random_cpu/online -a $k -eq 1 ]; then
        set_online $random_cpu
    fi
}

# IF WE HAVE A BUG CREATION LOGIC, TRIGGER IT
bug_random()
{
    if [ -f $DEBUGFS_LOCATION/pm_debug/bug ]; then
        k=`random 1`
        echo -n "$k"> $DEBUGFS_LOCATION/pm_debug/bug
        report "BUG : $k"
    fi
}

# Do off or not
offmode_random()
{
    k=`random 1`
    echo -n "$k"> $DEBUGFS_LOCATION/pm_debug/enable_off_mode
    report "enable_off_mode : $k"
}

# automated waker.. dont want hitting keyboards..
wakeup_time_random()
{
    # add this variable to have bigger wakeup time
    max_wtime=$1
    if [ -z $max_wtime ]; then
      max_wtime=10
    fi
    k=`random_ne0 $max_wtime`
    sec=`expr $k % 1000`
    msec=`expr $k / 1000`
    if [ -e $DEBUGFS_LOCATION/pm_debug/wakeup_timer_seconds ]; then
      echo $sec > $DEBUGFS_LOCATION/pm_debug/wakeup_timer_seconds
      echo $msec > $DEBUGFS_LOCATION/pm_debug/wakeup_timer_milliseconds
    fi
    report "wakeup - $sec sec $msec msec"
}

# cleanup cpuloadgen
remove_cpuloadgen()
{
    report "remove cpuloadgen"
    if [ `which cpuloadgen` ]; then
        sleep 5
        killall cpuloadgen 2>/dev/null
    else
        report "cpuloadgen is not installed"
    fi
}

# start up cpuloadgen
cpu_load_random()
{
    if [ `which cpuloadgen` ]; then
        local cpus_load=''
        local num_cpu=`get_num_cpus`
        i=0
        while [ $i -lt $num_cpu ]; do
            cpus_load="$cpus_load "`random_ne0 100`
        done
        time=`random_ne0 600`
        report "cpuloadgen $cpus_load $time"
        time cpuloadgen $cpus_load $time &
    else
        report "cpuloadgen is not installed"
    fi
}

#Run memtest
# $1: use memory percentage
# $2: number of iterations
run_memtest()
{
    export m1=`free|cut -d ":" -f2|sed -e "s/^\s\s*//g"|head -2|tail -1|cut -d ' ' -f1`
    export m2=M
    export m=`expr $m1 \* $1 / 100 / 1024`

    report "Testing $m$m2 of memory $2 times"
    memtester $m$m2 $2
}

#Start memtest
# $1: use memory percentage
start_memtest()
{
    # Step 1- start up memtest
    export m1=`free|cut -d ":" -f2|sed -e "s/^\s\s*//g"|head -2|tail -1|cut -d ' ' -f1`
    export m2=M
    export m=`expr $m1 \* $1 / 100 / 1024`

    report "Testing memory for $m$m2"
    memtester $m$m2 &

}
# pause memtester
pause_memtest()
{
    MEMTESTERPID=`ps | grep memtester | grep -v grep | cut -c 0-5`
    kill -STOP $MEMTESTERPID
    report "pause memtest"
}

# resume memtester
# $1: use memory percentage
resume_memtest()
{
    MEMTESTERPID=`ps | grep memtester | grep -v grep | cut -c 0-5`
    if [ -z "$MEMTESTERPID" ]; then
        start_memtest $1
    else
        kill -CONT $MEMTESTERPID
    fi
    report "resume memtest"
}

# kill memtester
kill_memtest()
{
    MEMTESTERPID=`ps | grep memtester | grep -v grep | cut -c 0-5`
    kill -TERM $MEMTESTERPID
    killall memtester 2>/dev/null
    report "killed memtest"
}

# give me some idle time
idle_random()
{
    time=`random 10`
    report "smallidle: $time seconds"
    sleep $time
}

# give me some idle time
idlebig_random()
{
    time=`random_ne0 300`
    report "bigidle: $time seconds"
    report "Processes running:"
    ps 
    report "Load running:"
    top -n1 -b
    report "cpu1 status:"
    cat /sys/devices/system/cpu/cpu1/online
    sleep $time
}

# dont suspend
no_suspend()
{
    if [ $use_wakelock -ne 0 ]; then
        echo "$PSID" >/sys/power/wake_lock
        report "wakelock $PSID"
    fi
}

# suspend / standby me
# input
#   -p power_state  optional; power state like 'mem' or 'standby'; default to 'mem'
#   -t max_stime    optional; maximum suspend or standby time; default to 10s; the suspend time will be a random number
#   -i iterations   optional; iterations to suspend/resume; default to 1
suspend()
{
    while getopts :p:t:i: arg
    do case $arg in
      p)  power_state="$OPTARG";;
      t)  max_stime="$OPTARG";;
      i)  iterations="$OPTARG";;
      \?)  test_print_trc "Invalid Option -$OPTARG ignored." >&2
      exit 1
      ;;
    esac
    done

    # for backward compatible
    : ${power_state:='mem'}
    : ${max_stime:='10'}
    : ${iterations:='1'}

    test_print_trc "suspend function: power_state: $power_state"
    test_print_trc "suspend function: max_stime: $max_stime"
    test_print_trc "suspend function: iterations: $iterations"

    if [ $use_wakelock -ne 0 ]; then
        report "removing wakelock $PSID (sec=$sec msec=$msec off=$off bug=$bug)"
        echo "$PSID" >/sys/power/wake_unlock
    fi

    local i=0
    while [ $i -lt $iterations ]; do
      test_print_trc "===suspend iteration $i==="

      wakeup_time_random $max_stime
      suspend_time=$sec
      # clear dmesg before suspend
      dmesg -c > /dev/null
      if [ -e $DEBUGFS_LOCATION/pm_debug/wakeup_timer_seconds ]; then
          report "Use wakeup_timer"
          report "suspend(sec=$sec msec=$msec off=$off bug=$bug)"
          echo -n "$power_state" > /sys/power/state
      elif [ -e /dev/rtc0 ]; then
          report "Use rtc to suspend resume"
          do_cmd rtcwake -d /dev/rtc0 -m ${power_state} -s ${suspend_time}
      else
          # Stop the test if there is no rtcwake or wakeup_timer support 
          die "There is no automated way (wakeup_timer or /dev/rtc0) to wakeup the board. No suspend!"
      fi

      check_suspend
      check_resume

      i=`expr $i + 1`
    done

    no_suspend
}

# check if suspend/standby is ok by checking the kernel messages
check_suspend()
{
    expect="PM: suspend of devices complete"
    dmesg | grep -i "$expect" && report "suspend successfully" || die "suspend failed"
}

# check if resume is ok by checking the kernel messages
check_resume()
{
    expect="PM: resume of devices complete"
    dmesg | grep -i "$expect" && report "resume successfully" || die "resume failed"
}

check_cpufreq_files() {

    local dirpath=$CPU_PATH/$1/cpufreq
    shift 1

    for i in $@; do
	check_file $i $dirpath || return 1
    done

    return 0
}

check_sched_mc_files() {

    local dirpath=$CPU_PATH

    for i in $@; do
	check_file $i $dirpath || return 1
    done

    return 0
}

check_topology_files() {

    local dirpath=$CPU_PATH/$1/topology
    shift 1

    for i in $@; do
	check_file $i $dirpath || return 1
    done

    return 0
}

check_cpuhotplug_files() {

    local dirpath=$CPU_PATH/$1
    shift 1

    for i in $@; do
	check_file $i $dirpath || return 1
    done

    return 0
}

save_governors() {

    governors_backup=
    local index=0

    for i in $(ls $CPU_PATH | grep "cpu[0-9].*"); do
	governors_backup[$index]=$(cat $CPU_PATH/$i/cpufreq/scaling_governor)
	index=$((index + 1))
    done
}

restore_governors() {

    local index=0
    local oldgov=

    for i in $(ls $CPU_PATH | grep "cpu[0-9].*"); do
	oldgov=${governors_backup[$index]}
	echo $oldgov > $CPU_PATH/$i/cpufreq/scaling_governor
	index=$((index + 1))
    done
}

save_frequencies() {

    frequencies_backup=
    local index=0
    local cpus=$(ls $CPU_PATH | grep "cpu[0-9].*")
    local cpu=

    for cpu in $cpus; do
	frequencies_backup[$index]=$(cat $CPU_PATH/$cpu/cpufreq/scaling_cur_freq)
	index=$((index + 1))
    done
}

restore_frequencies() {

    local index=0
    local oldfreq=
    local cpus=$(ls $CPU_PATH | grep "cpu[0-9].*")

    for cpu in $cpus; do
	oldfreq=${frequencies_backup[$index]}
	echo $oldfreq > $CPU_PATH/$cpu/cpufreq/scaling_setspeed
	index=$((index + 1))
    done
}

# give me detailed report
report_stats()
{
    local num_cpus=`get_num_cpus`
    report "============================================="
    report " $*"
    report "OMAP STATS: "
    report "$DEBUGFS_LOCATION/pm_debug/count"
    cat $DEBUGFS_LOCATION/pm_debug/count
    report "$DEBUGFS_LOCATION/pm_debug/time"
    cat $DEBUGFS_LOCATION/pm_debug/time
    report "$DEBUGFS_LOCATION/wakeup_sources"
    cat $DEBUGFS_LOCATION/wakeup_sources
    report "Core domain stats:"
    cat $DEBUGFS_LOCATION/pm_debug/count | grep "^core_pwrdm"
    if [ -f $DEBUGFS_LOCATION/suspend_time ]; then
        report "Suspend times:"
        cat $DEBUGFS_LOCATION/suspend_time
    fi
    report "CPUFREQ STATS: "
    report "/sys/devices/system/cpu/cpu0/cpufreq/stats/time_in_state"
    cat /sys/devices/system/cpu/cpu0/cpufreq/stats/time_in_state
    report "/sys/devices/system/cpu/cpu0/cpufreq/stats/total_trans"
    cat /sys/devices/system/cpu/cpu0/cpufreq/stats/total_trans
    report "/sys/devices/system/cpu/cpu0/cpufreq/stats/trans_table"
    cat /sys/devices/system/cpu/cpu0/cpufreq/stats/trans_table
    report "CPUIDLE STATS: "

    for cpu in `seq 0 $(($num_cpus - 1))`;
    do
        cpuidledir=/sys/devices/system/cpu/cpu$cpu/cpuidle
        if [ -d "$cpuidledir" ]; then
            report "CPU$cpu IDLE STATS: "
            k=`pwd`
            cd $cpuidledir
            report "NAME | DESCRIPTION | USAGE (number of entry)  | TIME | POWER | LATENCY"
            for state in *
            do
                DESC=`cat $state/desc`
                NAME=`cat $state/name`
                POWER=`cat $state/power`
                TIME=`cat $state/time`
                USAGE=`cat $state/usage`
                LATENCY=`cat $state/usage`
                report "$NAME | $DESC | $USAGE | $TIME | $POWER | $LATENCY"
            done
            cd $k
        fi
    done
    report "============================================="
}

# write pm counters into log file. The log will have something like "RET:0 \n RET-LOGIC-OFF:6"
# $1: power domain
# $2: power states seperated by delimiter Ex, "OFF:RET:INA","RET:RET-LOGIC-OFF" etc showing in pm count stat
# $3: power states delimiter
# $4: log name to save the counter
log_pm_count()
{
  pwrdm=$1
  pwr_states=$2
  states_delimiter=$3
  log_name=$4
  pwr_state_place=0

  tmp_ifs="$IFS"
  IFS=$states_delimiter
  for pwr_state in $pwr_states; do
    if [ "$pwr_state" = "DEVICE-OFF" ]; then
      pwr_state_place=1
    elif [ "$pwr_state" = "OFF" ]; then
      pwr_state_place=2
    elif [ "$pwr_state" = "RET" ]; then
      pwr_state_place=3
    elif [ "$pwr_state" = "INA" ]; then
      pwr_state_place=4
    elif [ "$pwr_state" = "ON" ]; then
      pwr_state_place=5
    elif [ "$pwr_state" = "RET-LOGIC-OFF" ]; then
      pwr_state_place=6
    fi

    pwrdm_stat=`cat $DEBUGFS_LOCATION/pm_debug/count | grep ^$pwrdm | \
                cut -d "," -f $pwr_state_place`
    report "Power domain stats requested: ${pwrdm}: $pwrdm_stat==========="
    echo "$pwrdm_stat" >> ${TMPDIR}/"$log_name"
  done
  IFS="$tmp_ifs"
}

# Compare two counters from two logs for pwrdm and pwr-state
#  The log contains something like "RET:0 \n RET-LOGIC-OFF:6 \n"
#  $1: pwrdm
#  $2: power states 
#  $3: power states delimiter; 
#  $4: log name before
#  $5: log name after  
compare_pm_count()
{
  pwrdm=$1
  pwr_state=$2
  state_delimiter=$3
  log_name_before=$4
  log_name_after=$5

  log_before=${TMPDIR}/"$log_name_before"
  log_after=${TMPDIR}/"$log_name_after"

  num_lines_1=`cat "$log_before" | wc -l`
  num_lines_2=`cat "$log_after" | wc -l`
  if [ $num_lines_1 -ne $num_lines_2 ]; then
    die "There is differnt number of pairs between log file $log_name_before and log file $log_name_after; can not compare these two logs" 
  fi

  tmp_ifs="$IFS"
  IFS=$state_delimiter
  for pwr_state in $pwr_states; do
    val_before=`get_value_for_key_from_file "$log_before" "$pwr_state" ":"` || die "Error getting value from $log_before for ${pwr_state}: $val_before"
    val_after=`get_value_for_key_from_file "$log_after" "$pwr_state" ":"` || die "Error getting value from $log_after for ${pwr_state}: $val_after"

    report "$pwrdm: Initial Value -> $pwr_state: $val_before"
    report "$pwrdm: Final Value -> $pwr_state: $val_after"

    # Verify the power domain counter increases
    report "Verifying $pwrdm: $pwr_state counter increases ..."
    sleep 1

    if [ "$val_after" -gt "$val_before" ]; then
      report "SUCCESS: $pwrdm: $pwr_state counters increased"
    else
      die "ERROR: $pwrdm: $pwr_state counters did not increase. Please review power states counters"
    fi

  done
  IFS="$tmp_ifs"

}

# $1: check type, either 'y', 'm', 'ym' or 'n'
# $2: Options to check. Uses same syntax returned by get_modular_config.names.sh
#     which is CONFIG1^CONFIG2:module1 CONFIG3:module2 
check_config_options()
{
  case $1 in
  	y) check='=y';;
  	m) check='=m';;
    ym) check='(=y|=m)';;
  	n) check=' is not set';;
  	*) die "$1 is not a valid check_config_options() option"
  esac
  OIFS=$IFS
  IFS=' '
  shift
  x=$*
  x=${x[@]}
  y=()
  for i in $x
  do
    newval=`echo $i | cut -d':' -f 1`
    y+=($newval)
  done
  IFS='^';y=${y[@]}
  IFS=' '
  for option in $y; do
  	zcat /proc/config.gz | egrep "$option$check" || die "$option is not $check"
  done
  IFS=$OIFS
}

sigtrap() {
    exit 255
}

# execute on exit - cleanup actions
on_exit()
{
    remove_cpuloadgen
    kill_memtest
}

trap on_exit EXIT


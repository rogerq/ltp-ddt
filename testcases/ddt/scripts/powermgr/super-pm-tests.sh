#!/bin/bash
#
# Test for power sequences on OMAP
# Copyright (C) 2013 Texas Instruments Incorporated.
#     Original worked by Nishanth Menon
#     Modified by Carlos Hernandez
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License version 2 as
# published by the Free Software Foundation.


# Uses following global definitions
# $GOVERNOR: cpufreq governor
# $USE_MEMORY_PERCENTAGE: memtester mem usage percentage

source "functions.sh" 

# test idle
test_idle()
{
	report_stats "BEFORE IDLE TEST"
	idle_random
	resume_memtest $USE_MEMORY_PERCENTAGE
	idle_random
	pause_memtest
	idle_random
	cpu_load_random
	idle_random
	remove_cpuloadgen
	resume_memtest $USE_MEMORY_PERCENTAGE
	cpu_load_random
	idlebig_random
	remove_cpuloadgen
	pause_memtest
	idlebig_random
	report_stats "AFTER IDLE TEST"
}

# test suspend
test_suspend()
{
	report_stats "BEFORE SUSPEND TEST"
	suspend
	resume_memtest $USE_MEMORY_PERCENTAGE
	suspend
	pause_memtest
	idle_random
	cpu_load_random
	idle_random
	suspend
	idle_random
	remove_cpuloadgen
	resume_memtest $USE_MEMORY_PERCENTAGE
	cpu_load_random
	suspend
	remove_cpuloadgen
	pause_memtest
	idlebig_random
	suspend
	idle_random
	report_stats "AFTER SUSPEND TEST"
}

# simple suspend resume test with reporting stats before and after
#  $1: power state like 'mem' or 'standby'
#  $2: max time for random suspend time
#  $3: suspend iterations
simple_suspend_w_stats()
{
  power_state=$1
  max_stime=$2
  iterations=$3
  report_stats "BEFORE SIMPLE SUSPEND TEST"
  suspend -i $iterations -p $power_state -t $max_stime
  report_stats "AFTER SIMPLE SUSPEND TEST"
}

# Test to exercise suspend, cpuidle and cpufreq
#  $1: Number of test iterations
super_pm_test()
{
	report "Kernel revision"
	cat /proc/version

	report "Switching to governor $GOVERNOR"
	echo -n "$GOVERNOR">/sys/devices/system/cpu/cpu0/cpufreq/scaling_governor

	start_memtest $USE_MEMORY_PERCENTAGE
	export MEMTESTERPID=`ps | grep memtester | grep -v grep | cut -c 0-5`

	# Test loop
    test_iteration=0
    iterations=$1
	while [ $test_iteration -lt $iterations ]
	do
		report "+++++++++++++++++++++++++++++++++++++++++++++"
		report "iteration: $test_iteration"
		report "+++++++++++++++++++++++++++++++++++++++++++++"
		# cleanup junk
		pause_memtest
		remove_cpuloadgen
		no_suspend

		# setup for the test iteration
		cpu_online_random
		offmode_random
		off=`cat $DEBUGFS_LOCATION/pm_debug/enable_off_mode`
		wakeup_time_random
		sec=`cat $DEBUGFS_LOCATION/pm_debug/wakeup_timer_seconds`
		msec=`cat $DEBUGFS_LOCATION/pm_debug/wakeup_timer_milliseconds`
		bug_random
		if [ -f $DEBUGFS_LOCATION/pm_debug/bug ]; then
			bug=`cat $DEBUGFS_LOCATION/pm_debug/bug`
		fi

		test_suspend
		test_idle
		test_suspend
		test_idle

		test_iteration=`expr $test_iteration + 1`
	done
}

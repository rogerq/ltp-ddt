#!/bin/sh
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
# this file contains routines for printing the logs

#
# starts the cpu load measurement.
#

start_cpuload_measurement ()
{
	START_TOTAL_TIME=0;
	START_IDLE_TIME=0;

	#grep the statistical values
	CPU=`cat /proc/stat | grep '^cpu '`

	# Calculate the total CPU time and idle time.
	j=0
	START_IDLE_TIME=0
	for i in $CPU; do
		if [ $j -ne 0 ] ; then
			let "START_TOTAL_TIME=$START_TOTAL_TIME+$i"
			#Idle and irqwait values are at 4th and 5th position
			if [ $j -eq 4 ] ; then
				START_IDLE_TIME=$(($START_IDLE_TIME + $i))
			fi
			if [ $j -eq 5 ] ; then
				START_IDLE_TIME=$(($START_IDLE_TIME + $i))
			fi
		fi
		j=$((j+1))
	done
}
 
#
# stops the cpu load measurement and calculates the cpu load and returns.
#
 
stop_cpuload_measurement ()
{

	END_TOTAL_TIME=0;
	END_IDLE_TIME=0;

	#grep the statistical values
	CPU=`cat /proc/stat | grep '^cpu '`
	# Calculate the total CPU time and idle time.
	j=0
	for i in $CPU; do
		if [ $j -ne 0 ] ; then
			let "END_TOTAL_TIME=$END_TOTAL_TIME+$i"
			#Idle and irqwait values are at 4th and 5th position
			if [ $j -eq 4 ] ; then
				END_IDLE_TIME=$(($END_IDLE_TIME + $i))
			fi
			if [ $j -eq 5 ] ; then
				END_IDLE_TIME=$(($END_IDLE_TIME + $i))
			fi
		fi
		j=$((j+1))
	done

	DIFF_TOTAL_TIME=0;
	DIFF_IDLE_TIME=0;

	#Calculate the cpu load.

	let "DIFF_TOTAL_TIME=$END_TOTAL_TIME-$START_TOTAL_TIME"
	let "DIFF_IDLE_TIME=$END_IDLE_TIME-$START_IDLE_TIME"
 
	let "CPU_LOAD=(($DIFF_TOTAL_TIME-$DIFF_IDLE_TIME)*100)/DIFF_TOTAL_TIME"

	return $CPU_LOAD
}



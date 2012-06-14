#! /bin/sh
############################################################################### 
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
###############################################################################

# @desc	set SoC to various available OPPs (option for delay & loop), passing suitable options
# @params d) DELAY	delay between OPP switching
#         e) EXTREME	set to true if switching required between extreme OPPs
#	  r} REVERSE	set to true if switching required in descending order of OPPs
#	  l) LOOP	number of loops to switch between available OPPs
# @returns Zero for success, non-zero for failure
# @history 2012-01-31: First version

source "common.sh"  # Import do_cmd(), die() and other functions

############################# Functions #######################################
usage()
{
cat <<-EOF >&2
        usage: ./${0##*/} [-d DELAY] [-e EXTREME] [-r REVERSE] [-l LOOP] 
	-d DELAY	delay between OPP switching, default is no delay 
	-e EXTREME	set to true if OPP switching is only required between extreme OPPs, default is to switch between all OPPs
	-r REVERSE	set to true if OPP switching is required to be descending, default is ascending
	-l LOOP		number of loops OPP has to be switched, default is 1 loop
	-h Help         print this usage
EOF
exit 0
}

############################ Script Variables ##################################
# Define default valus if possible
DELAY=0
LOOP=1
reverse="false"
extreme="false"

################################ CLI Params ####################################
# Please use getopts
while getopts  :d:e:r:l:h arg
do case $arg in
        d)      DELAY="$OPTARG";;
        e)      extreme="$OPTARG";;
        r)      reverse="$OPTARG";;
	l)	LOOP="$OPTARG";;
        h)      usage;;
        \?)     die "Invalid Option -$OPTARG ";;
esac
done

############################ USER-DEFINED Params ###############################
REGULATOR="VDD1"
REGULATOR_VOLT_PATH="/sys/class/regulator/regulator.0/microvolts"

case $MACHINE in
	am335x-evm) REGULATOR="VDD1";;
	beaglebone) REGULATOR="DCDC2";;
	am180x-evm) REGULATOR="VDCDC3";;
esac


########################### DYNAMICALLY-DEFINED Params #########################
mkfifo temppipe
ls /sys/class/regulator/ | awk ' {print "/sys/class/regulator/"$1"/name"}' > temppipe &
while read line
do
	CUR_REG=`cat $line`
	if [ $CUR_REG == $REGULATOR ]
	then
		REG_NAME=`echo $line | cut -d '/' -f 5`
		REGULATOR_VOLT_PATH="/sys/class/regulator/"$REG_NAME"/microvolts"
	fi
done < temppipe
rm temppipe

########################### REUSABLE TEST LOGIC ###############################
test_print_trc "cpufreq trace log"

cnt=`cat /sys/devices/system/cpu/cpu0/cpufreq/scaling_available_frequencies | awk ' {print NF} '`
k=0;

test_print_trc "OPP to be switched for $LOOP loops with $DELAY seconds delay between each OPP transition"

while [ $k -lt $LOOP ]
do
  echo "============ LOOP: $k============"
	k=$((k+1))
	if [ "$extreme" == "false" ]
	then
		i=0;
		while [ $i -lt $cnt ]
		do
			i=$((i+1))
			if [ "$reverse" == "false" ]
			then
				t=$i
			else
				t=`expr $cnt + 1 - $i`
			fi
			FREQ=`cat /sys/devices/system/cpu/cpu0/cpufreq/scaling_available_frequencies | awk -v tmp=$t '{print $tmp}'`
			do_cmd echo $FREQ > "/sys/devices/system/cpu/cpu0/cpufreq/scaling_setspeed" || die "unable to set OPP for $FREQ KHz"
			FREQUENCY=`cat /sys/devices/system/cpu/cpu0/cpufreq/scaling_cur_freq`
			if [ $FREQ -ne $FREQUENCY ]
			then
				die "unable to set OPP for $FREQ KHz"
			fi
			VOLTAGE=`cat $REGULATOR_VOLT_PATH`
			test_print_trc "OPP set for: $FREQUENCY KHz $VOLTAGE uV"
			sleep $DELAY
		done
	else
		FREQ=`cat /sys/devices/system/cpu/cpu0/cpufreq/scaling_available_frequencies | awk '{print $1}'`
		do_cmd echo $FREQ > "/sys/devices/system/cpu/cpu0/cpufreq/scaling_setspeed" || die "unable to set OPP for $FREQ KHz"
		FREQUENCY=`cat /sys/devices/system/cpu/cpu0/cpufreq/scaling_cur_freq`
		if [ $FREQ -ne $FREQUENCY ]
		then
			die "unable to set OPP for $FREQ KHz"
		fi
		VOLTAGE=`cat $REGULATOR_VOLT_PATH`
		test_print_trc "OPP set for: $FREQUENCY KHz $VOLTAGE uV"
		sleep $DELAY
		FREQ=`cat /sys/devices/system/cpu/cpu0/cpufreq/scaling_available_frequencies | awk -v tmp=$cnt '{print $tmp}'`
		do_cmd echo $FREQ > "/sys/devices/system/cpu/cpu0/cpufreq/scaling_setspeed" || die "unable to set OPP for $FREQ KHz"
		FREQUENCY=`cat /sys/devices/system/cpu/cpu0/cpufreq/scaling_cur_freq`
		if [ $FREQ -ne $FREQUENCY ]
		then
			die "unable to set OPP for $FREQ KHz"
		fi
		VOLTAGE=`cat $REGULATOR_VOLT_PATH`
		test_print_trc "OPP set for: $FREQUENCY KHz $VOLTAGE uV"
		sleep $DELAY
	fi
done

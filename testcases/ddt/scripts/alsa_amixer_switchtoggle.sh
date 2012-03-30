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
# @desc Toggles the switch by playing the audio in backgroung using amixer interface.
# @params  l) TEST_LOOP    test loop for switch toggling. default is 1.
# @history 2011-04-07: First version
# @history 2011-05-13: Removed st_log.sh
source "common.sh"  # Import do_cmd(), die() and other functions

############################# Functions #######################################
usage()
{
	cat <<-EOF >&2
	usage: ./${0##*/} [-l TEST_LOOP]
	EOF
	exit 0
}

################################ CLI Params ####################################
# Please use getopts
while getopts  :l:h arg
do case $arg in  
        l)      TEST_LOOP="$OPTARG";;
        h)      usage;;
        :)      die "$0: Must supply an argument to -$OPTARG.";; 
        \?)     die "Invalid Option -$OPTARG ";;
esac
done

# Define default values if possible
: ${TEST_LOOP:=3}

############################ USER-DEFINED Params ###############################
# Try to avoid defining values here, instead see if possible
# to determine the value dynamically. ARCH, DRIVER, SOC and MACHINE are 
# initilized and exported by runltp script based on platform option (-P)
case $ARCH in
esac
case $DRIVER in

esac
case $SOC in
esac
case $MACHINE in
am180x-evm|dm355-evm|dm365-evm|dm6446-evm|dm6467-evm|dm368-evm)
	CAPTURE_SWITCH_NAME_1="PGA Capture Switch";
	PLAYBACK_SWITCH_NAME_1="LineL Playback Switch"
	PLAYBACK_SWITCH_NAME_2="LineR Playback Switch"
	;;
omap3evm)	
	CAPTURE_SWITCH_NAME_1="Analog Right AUXR Capture Switch";
	CAPTURE_SWITCH_NAME_2="Analog Left AUXL Capture Switch";	
	PLAYBACK_SWITCH_NAME_1="DAC2 Analog Playback Switch"
	amixer cset numid=25 1,1
	amixer cset numid=28 1,1	
	;;
am37x-evm|beagleboard)
	CAPTURE_SWITCH_NAME_1="Analog Right AUXR Capture Switch";
	CAPTURE_SWITCH_NAME_2="Analog Left AUXL Capture Switch";	
	PLAYBACK_SWITCH_NAME_1="DAC2 Analog Playback Switch"
	;;
am3517-evm)
	CAPTURE_SWITCH_NAME_1="Digital Capture Switch";
	PLAYBACK_SWITCH_NAME_1="Digital Playback Switch"
	;;	
da850-omapl138-evm)
	CAPTURE_SWITCH_NAME_1="PGA Capture Switch";
	PLAYBACK_SWITCH_NAME_1="Line Playback Switch"
	;;	
am387x-evm|am389x-evm|am335x-evm|dm385-evm)
        CAPTURE_SWITCH_NAME_1="PGA Capture Switch";
        PLAYBACK_SWITCH_NAME_1="HP Playback Switch"
        ;;
esac

########################### REUSABLE TEST LOGIC ###############################

amixer controls
amixer contents
arecord -f dat -d 300 | aplay -f dat -d 300&

i=0
while [[ $i -lt $TEST_LOOP ]]
do
	do_cmd amixer cset name=\'$CAPTURE_SWITCH_NAME_1\' 0
	if [ "$CAPTURE_SWITCH_NAME_2" != "" ] ; then
		do_cmd amixer cset name=\'$CAPTURE_SWITCH_NAME_2\' 0
	fi
	sleep 15
	do_cmd amixer cset name=\'$CAPTURE_SWITCH_NAME_1\' 1
	if [ "$CAPTURE_SWITCH_NAME_2" != "" ] ; then
		do_cmd amixer cset name=\'$CAPTURE_SWITCH_NAME_2\' 1
	fi
	sleep 15
	
	do_cmd amixer cset name=\'$PLAYBACK_SWITCH_NAME_1\' 0
	if [ "$PLAYBACK_SWITCH_NAME_2" != "" ] ; then
		do_cmd amixer cset name=\'$PLAYBACK_SWITCH_NAME_2\' 0
	fi
	sleep 15
	do_cmd amixer cset name=\'$PLAYBACK_SWITCH_NAME_1\' 1
	if [ "$PLAYBACK_SWITCH_NAME_2" != "" ] ; then
		do_cmd amixer cset name=\'$PLAYBACK_SWITCH_NAME_2\' 1
	fi
	sleep 15	
	let "i += 1"	
done


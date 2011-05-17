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
# @desc Checks the multi stream Capture and Playback
# @params t) Test type     : Capture,playback
#         D) Audio Device  : Audio Device.
#         l) Capture Log   : Whether to retain captured file or delete.
# @history 2011-04-07: First version
# @history 2011-05-13: Removed st_logs.sh

source "common.sh"  # Import do_cmd(), die() and other functions

############################# Functions #######################################
usage()
{
	cat <<-EOF >&2
	usage: ./${0##*/} [-t TEST_TYPE] [-D DEVICE]
	-t TEST_TYPE		Test Type. Possible Values are Capture,playback,loopback.
	-D DEVICE           Device Name like hw:0,0.
	-l CAPTURELOG_FLAG  Whether to retain captured file or delete.( 1 -> To retain, 0 -> delete )	
	EOF
	exit 0	
}

################################ CLI Params ####################################
# Please use getopts
while getopts  :t:D:l:h arg
do case $arg in
        t)      TYPE="$OPTARG";;
        D)      DEVICE="$OPTARG";;                
        l)      CAPTURELOGFLAG="$OPTARG";;                        
        h)      usage;;
        :)      die "$0: Must supply an argument to -$OPTARG.";; 
        \?)     die "Invalid Option -$OPTARG ";;
esac
done

# Define default values if possible
: ${TYPE:='capture'}
: ${DEVICE:='hw:0,0'}

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
omap3evm)
		amixer cset numid=25 1,1
		amixer cset numid=28 1,1
	;;	
esac

########################### REUSABLE TEST LOGIC ###############################
# DO NOT HARDCODE any value. If you need to use a specific value for your setup
# use USER-DEFINED Params section above.

# Print the test params.
test_print_trc " ****************** TEST PARAMETERS ******************"
test_print_trc " TYPE		: $TYPE"
test_print_trc " DEVICE		: $DEVICE"

test_print_trc " *************** END OF TEST PARAMETERS ***************"

case "$TYPE" in
	
	capture)
		RESULT=0
		test_print_trc "CMD=arecord -D $DEVICE -f dat test.snd -d 60 "
		arecord -D $DEVICE_NODE -f dat test.snd -d 60&	
		pidlist="$pidlist $!"
		arecord -D $DEVICE_NODE -f dat test.snd -d 10
		RESULT=$(( $RESULT + $? ));
		for job in $pidlist
		do
			wait $job || let "RESULT+=1"
		done
		if [ $RESULT -eq 0 ] ; then
			test_print_err "Capture Multistream Test failed."
			exit 1		
		fi	
		;;		
	playback)
		RESULT=0
		test_print_trc "CMD=arecord -D $DEVICE -f dat test.snd -d 60 "
		arecord -D $DEVICE -f dat test.snd -d 60 
		RESULT=$(( $RESULT + $? ));
		test_print_trc "CMD:aplay -D $DEVICE_NODE -f dat test.snd -d 10 -v"
		aplay -D $DEVICE_NODE -f dat /usr/share/sounds/alsa/Front_Right.wav -d 60&
		pidlist="$pidlist $!"
		aplay -D $DEVICE_NODE -f dat /usr/share/sounds/alsa/Front_Left.wav -d 10
		RESULT=$(( $RESULT + $? ));
		for job in $pidlist
		do
			wait $job || let "RESULT+=1"
		done
		if [ $RESULT -eq 0 ] ; then
			test_print_err "Playback Multistream Test failed."
			exit 1
		fi		
		if [ $CAPTURELOGFLAG -eq 0 ] ; then
			rm test.snd
		fi		
		;;			
esac	

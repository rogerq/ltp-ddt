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
# @history 2011-05-13: First version
# @desc Run speaker-test utility with all available options to test sound output 

source "common.sh"  # Import do_cmd(), die() and other functions

############################# Functions #######################################
usage()
{
	echo "run_speaker_test.sh [For options (see speaker-test help)"
	echo " all other args are passed as-is to speaker-test"
	echo " speaker-test help:"
        echo `speaker-test -h`
	exit 1
}

################################ CLI Params ####################################
# Please use getopts
while getopts  :H:h arg
do case $arg in
        h)      usage;;
        :)      ;; 
        \?)     ;;
esac
done
# Define default values if possible

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
    *omap3evm|am37x-evm|beagleboard)
        amixer cset name='Analog Left AUXL Capture Switch' 1 
        amixer cset name='Analog Right AUXR Capture Switch' 1
        amixer cset name='HeadsetL Mixer AudioL1' on
        amixer cset name='HeadsetR Mixer AudioR1' on
        amixer cset name='Headset Playback Volume' 3
	;;
    *da850-omapl138-evm)
        amixer cset name='PCM Playback Volume' 127,127
	;;
esac
# Define default values for variables being overriden

########################### REUSABLE TEST LOGIC ###############################
# DO NOT HARDCODE any value. If you need to use a specific value for your setup
# use USER-DEFINED Params section above.

test_print_trc "Starting speaker-test TEST"

do_cmd "speaker-test $*"


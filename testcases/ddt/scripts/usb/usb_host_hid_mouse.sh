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


source "common.sh"
source "st_log.sh"
source "usb_HID_search_device.sh"

############################# Functions #######################################
usage()
{
	cat <<-EOF >&2
	usage: ./${0##*/} 
	EOF
	exit 0
}

################################ CLI Params ####################################
# Please use getopts
while getopts  :h arg
do case $arg in       
        h)      usage;;
        :)      die "$0: Must supply an argument to -$OPTARG.";; 
        \?)     die "Invalid Option -$OPTARG ";;
esac
done


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
esac

############################ DEFAULT Params #######################

#
# USB HID Class and Protocol codes
#
HID_CLASS=3
HID_PROTOCOL_NONE=0
HID_PROTOCOL_KEYBOARD=1
HID_PROTOCOL_MOUSE=2

########################### REUSABLE TEST LOGIC ###############################

test_print_trc " ::"
test_print_trc " :: :::::::::::::::::::::::: HID class tests (Standalone) :::::::::::::::::::::::"
test_print_trc " ::"


HID_Search_Device $HID_PROTOCOL_MOUSE

if test $devProtocol_mouse -eq 2
then

    test_print_trc "TestName:: HID : USB Mouse"
    test_print_trc "TestDesc:: Test input events from USB Mouse using evtest"

    evtest /dev/input/event$hidevent &

	test_print_trc "This test requires manual intervention."

	 sleep 10;
	 kill -9 $(pidof evtest)
	 test_print_end $TestId
 else
	 test_print_trc " ::"
	 test_print_trc " :: USB Mouse not found. Exiting USB Mouse tests..."
	 test_print_trc " ::"
	 exit 2;
fi





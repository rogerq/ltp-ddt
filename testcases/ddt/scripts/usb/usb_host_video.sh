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

############################# Functions #######################################
usage()
{
	cat <<-EOF >&2
	usage: ./${0##*/} [-w WIDTH] [-l LENGTH] [-i FILE_NAME] [-h HELP] [-d DEV_NAME]
	-w WIDTH          	Video Resolution Width
	-l LENGTH        	Video Resolution Length
	-i FILE_NAME       	File Name
	-d DEV_NAME		Device Name Eg: /dev/video0
	-h HELP          	Help
	EOF
	exit 0
}

################################ CLI Params ####################################
# Please use getopts
while getopts  :w:l:i:d:h arg
do case $arg in
        w)      WIDTH="$OPTARG";;
        l)      LENGTH="$OPTARG";;
        i)      FILE_NAME="$OPTARG";;
	d)	DEV_NAME="$OPTARG";;
        h)      HELP="$OPTARG";;
        :)      die "$0: Must supply an argument to -$OPTARG.";;
        \?)     die "Invalid Option -$OPTARG ";;
esac
done

############################ Default Values for Params ###############################

: ${DEV_NAME:='/dev/video0'}
: ${FILE_NAME:='/home/root/capture.YUV'}
: ${WIDTH:='320'}
: ${LENGTH:='240'}

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

########################### REUSABLE TEST LOGIC ###############################

# DO NOT HARDCODE any value. If you need to use a specific value for your setup
# use USER-DEFINED Params section above.


# Print the test params.

test_print_trc " ****************** TEST PARAMETERS ******************"
test_print_trc " DEVICE_NAME		:$DEV_NAME"
test_print_trc " RES_WIDTH		:$WIDTH"
test_print_trc " RES_LENGTH		:$LENGTH"
test_print_trc " FILE_NAME  		:$FILE_NAME"
test_print_trc " *************** END OF TEST PARAMETERS ***************"

#capturing the video

do_cmd cd /opt/ltp/testcases/bin/ddt
do_cmd ./usb_video_capture -d $DEV_NAME -w $WIDTH -l $LENGTH -i $FILE_NAME


test_print_trc " :: ########################################################## ::"
test_print_trc " ::                                                            ::"
test_print_trc " ::                                                            ::"
test_print_trc " :: Captured video is saved at $FILE_NAME.                     ::"
test_print_trc " ::                                                            ::"
test_print_trc " ::                                                            ::"
test_print_trc " :: ########################################################## ::"

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

# @desc Mount debug filesystem on target 
# @returns Non-Zero value on error. 
# @history 2012-06-11: First version

source "common.sh"  # Import do_cmd(), die() and other functions

############################# Functions #######################################
usage()
{
	echo "$0 -m <mount point>" 
   echo "Mount debugfs on /debug directory by default"
	exit 1
}

############################ Script Variables ##################################

################################ CLI Params ####################################
# Please use getopts
while getopts  m:h arg
do case $arg in
		  m)      MOUNT_POINT="$OPTARG" ;;
        h)      usage;;
        \?)     die "Invalid Option -$OPTARG ";;
esac
done

############################ USER-DEFINED Params ###############################
# Try to avoid defining values here, instead see if possible
# to determine the value dynamically. ARCH, DRIVER, SOC and MACHINE are 
# initilized and exported by runltp script based on platform option (-P)

# Define default values for variables being overriden
: ${MOUNT_POINT:="/debug"}

case $ARCH in
esac
case $DRIVER in
esac
case $SOC in
esac
case $MACHINE in
esac

########################### DYNAMICALLY-DEFINED Params #########################
# Try to use /sys and /proc information to determine values dynamically.
# Alternatively you should check if there is an existing script to get the
# value you want

########################### REUSABLE TEST LOGIC ###############################
# DO NOT HARDCODE any value. If you need to use a specific value for your setup
# use USER-DEFINED Params section above.

# Uncomment the following line if you want the change the behavior of
# do_cmd to treat non-zero values as pass and zero as fail.
# inverted_return="true"

# Avoid using echo. Instead use print functions provided by st_log.sh
IS_MOUNTED="no"
mount |grep "debugfs" > /dev/null && IS_MOUNTED="yes"
if [ "$IS_MOUNTED" == "yes" ];then
	MOUNT_POINT=`mount |grep "debugfs" |cut -d" " -f3`
else
	if ! [ -e $MOUNT_POINT ]; then
		do_cmd mkdir $MOUNT_POINT > /dev/null
	fi
	do_cmd mount -t debugfs debug $MOUNT_POINT > /dev/null
fi

echo "$MOUNT_POINT"

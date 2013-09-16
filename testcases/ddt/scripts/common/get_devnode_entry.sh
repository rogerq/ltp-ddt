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

# Get devnode entry like /dev/sda from /dev/sda1; /dev/mmcblk0 from /dev/mmcblk0p1
# Input: DEVICE_TYPE like 'mmc', 'usb', 'ata'
#	DEV_NODE like "/dev/mmcblk0p1"; "/dev/sda1"
# Output: DEVNODE_ENTRY like /dev/mmcblk0 

source "common.sh"

if [ $# -ne 2 ]; then
        echo "Error: Invalid Argument Count"
        echo "Syntax: $0 <dev_node like /dev/sda1> <device_type like 'usb', 'mmc'>"
        exit 1
fi

############################ Default Params ##############################

######################### Logic here ###########################################
DEV_NODE=$1
DEVICE_TYPE=$2
case $DEVICE_TYPE in
	mmc | emmc)
		DEVNODE_ENTRY=`echo $DEV_NODE | sed 's/p[0-9]*$//' `
	;;
	*)
		DEVNODE_ENTRY=`echo $DEV_NODE | sed 's/[0-9]*$//' `
	;;
esac
[ -z $DEVNODE_ENTRY ] && exit 1
echo $DEVNODE_ENTRY





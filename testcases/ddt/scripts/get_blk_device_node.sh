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

# Get devnode for non mtd device like 'mmc', 'usb', 'ata'
# Input: DEVICE_TYPE like 'mmc', 'usb', 'ata'
# Output: DEV_NODE like /dev/mmcblk0p1 

source "common.sh"
source "mtd_common.sh"
source "blk_device_common.sh"


if [ $# -ne 1 ]; then
        echo "Error: Invalid Argument Count"
        echo "Syntax: $0 <device_type>"
        exit 1
fi
DEVICE_TYPE=$1

############################ Functions ################################
# this function is to get SCSI device usb or sata node based on vendor info
# assume SATA's vendor always ATA; if not, throw error
# input is either 'sata' or 'usb'
 
find_scsi_node() {
	SCSI_DEVICE=$1
	# find the first drive for SATA to test, ignore the second one if any
	# Assume maximum 2 usb and 2 sata plugged in
	DRIVES='a b c d'
	for DRIVE in $DRIVES; do
		if [ -e /sys/block/sd"$DRIVE"/device/vendor ]; then
		VENDOR=`cat /sys/block/sd"$DRIVE"/device/vendor`
		RESULT=`echo $VENDOR | grep -i "ATA"`
		case $SCSI_DEVICE in
			sata)
				if [ -n "$RESULT" ]; then
					DEV_NODE="/dev/sd"$DRIVE"1"
					echo $DEV_NODE
					exit 0				
				fi
			;;
			usb)
				if [ -z "$RESULT" ]; then
					DEV_NODE="/dev/sd"$DRIVE"1"
					echo $DEV_NODE
					exit 0				
				fi
			;;
		esac
		fi
	done
	# if could not find match, let user know
	echo "Could not find device node for SCSI device!"
	exit 1
}

############################ Default Params ##############################
DEV_TYPE=`get_device_type_map.sh "$DEVICE_TYPE"` || die "error getting device type: $DEV_TYPE"
case $DEV_TYPE in
        mtd)
		PART=`get_mtd_partition_number.sh "$DEVICE_TYPE"` || die "error getting mtd partition number: $PART"
		DEV_NODE="$MTD_BLK_DEV$PART"
        ;;
        mmc)
          DEV_NODE=`find_part_with_biggest_size "/dev/mmcblk0" "mmc"` || die "error getting partition with biggest size: $DEV_NODE"
        ;;
        usb)
                #DEV_NODE="/dev/sda1"
		DEV_NODE=`find_scsi_node "usb"` || die "error getting usb node: $DEV_NODE" 
        ;;
        ata)
                DEV_NODE="/dev/hda1"
        ;;
        sata)
		DEV_NODE=`find_scsi_node "sata"` || die "error getting sata node: $DEV_NODE"
        ;;
        *)
                die "Invalid device type in $0 script"
        ;;
esac

############################ USER-DEFINED Params ##############################
# Try to avoid defining values here, instead see if possible
# to determine the value dynamically
case $ARCH in
esac
case $DRIVER in
esac
case $SOC in
esac
case $MACHINE in
esac

######################### Logic here ###########################################
echo $DEV_NODE

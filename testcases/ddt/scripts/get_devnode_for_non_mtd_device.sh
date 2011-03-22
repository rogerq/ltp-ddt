#!/bin/sh

# Get devnode for non mtd device like 'mmc', 'usb', 'ata'
# Input: DEVICE_TYPE like 'mmc', 'usb', 'ata'
# Output: DEV_NODE like /dev/mmcblk0p1 

source "st_log.sh"

DEVICE_TYPE=$1

############################ Default Params ##############################
DEV_TYPE=$DEVICE_TYPE
# for now, hardcode the devnode. Later on, figure out a way to dynamicaly decide the value
case $DEVICE_TYPE in
	mmc)
		DEV_NODE="/dev/mmcblk0p1"
	;;
	usb)
		DEV_NODE="/dev/sda1"
	;;
	ata)
		DEV_NODE="/dev/hda1"
	;;
	*)
		test_print_err "Invalid Device type input in $0 script"
		exit 1;
	;;
esac


echo $DEV_NODE

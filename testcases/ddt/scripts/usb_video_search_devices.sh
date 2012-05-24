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

# search for USB Video devices

source "common.sh"
source "st_log.sh"

#
# Search for an USB Video device
#


videodev=none
devices=`ls -d /sys/class/video4linux/video*`
for device in $devices
	do
		module=`egrep usb $device'/device/modalias' | cut -c1-3`
		if test "$module" = "usb";
		then
			videodev=`basename "$device"`
			camname=`cat $device'/name'`
			echo /dev/$videodev
		fi
done
if test "$videodev" = "none";
then
	test_print_trc " ::"
	test_print_trc " :: No USB Camera found. Exiting Video tests..."
	test_print_trc " ::"
	exit 2	
fi

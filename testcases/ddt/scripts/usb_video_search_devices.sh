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
usb_interface=$1
usb_cnt_interface=`get_usb_controller_name.sh "$usb_interface"` 
for device in $devices
	do
		module=`egrep usb $device'/device/modalias' | cut -c1-3`
		if test "$module" = "usb";
		then
                  videodev=`basename "$device"`         
                # take care of target not using udev - for backward compatiblity
                  udev_present=""
                  udev_present=`ps|grep -m 1 udevd|grep -v grep`
                  if  [ -n "$udev_present" ];
                  then
                        udev_tree_search=""
                        udev_tree_search=`udevadm info -a -n /\dev/\$videodev|grep -m 1 "$usb_cnt_interface"`
                          if [ -n "$udev_tree_search" ]; then
                            echo /dev/$videodev 
                          else  
                            videodev=none
                            echo "this is usb case but no camera found on $usb_cnt_interface"
                          fi
                  else # no udev case
                        camname=`cat $device'/name'`          
                        echo /dev/$videodev 
		  fi
                fi
done
if test "$videodev" = "none";
then
	test_print_trc " ::"
	test_print_trc " :: No USB Camera found. Exiting Video tests..."
	test_print_trc " ::"
	exit 2	
fi

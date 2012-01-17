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

# search for USB audio devices

source "common.sh"
source "st_log.sh"


############################# Functions #######################################

############################### CLI Params ###################################

############################ DEFAULT Params #######################

############# Do the work ###########################################

#
# Search for an USB Audio device
#
#Audio_Search_Device()
#{
#test_print_trc " ::"
#test_print_trc " :: Determining audio card number for USB Audio device"
#test_print_trc " ::"

audiodev=none
devices=`ls -d /sys/class/sound/card*`
for device in $devices
	do
		module=`egrep usb $device'/device/modalias' | cut -c1-3`
		if test "$module" = "usb";
		then
			audiodev=`cat $device'/number'`
#			test_print_trc " ::"
#			test_print_trc " :: Found USB Audio device mapped to [card$audiodev]"
#			test_print_trc " ::"
			echo $audiodev
		fi
	done
if test "$audiodev" = "none";
then
	test_print_trc " ::"
	test_print_trc " :: No USB Audio device found. Exiting Audio tests..."
	test_print_trc " ::"	
fi
#}

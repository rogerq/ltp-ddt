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

# search for USB HID devices

source "common.sh"
source "st_log.sh"
source "usb_HID_get_event.sh"

############################# Functions #######################################

usage()
{
cat <<-EOF >&2
        usage: ./${0##*/}  [-p HID_PROTOCOL_DEVICE]
        -p HID_PROTOCOL_DEVICE  HID Device protocol like 0,1,2
        -h Help         print this usage
EOF
exit 0
}


############################ CLI Params ###########################################
while getopts p: arg
do case $arg in

        p)
                HID_PROTOCOL_DEVICE=$OPTARG ;;

        h)
		usage ;;
        \?)
		echo "Invalid Option -$OPTARG ignored." >&2
                usage
                exit 1
                ;;

esac
done

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

############################ DEFAULT Params #######################

#
# USB HID Class and Protocol codes
#
HID_CLASS=3
HID_PROTOCOL_NONE=0
HID_PROTOCOL_KEYBOARD=1
HID_PROTOCOL_MOUSE=2


############# Do the work ###########################################

#
# Search for an USB Keyboard and USB Mouse devices
#
HID_Search_Device()
{
hidSearch=$1
devClass=0
devProtocol=0
hidevent=0
hiddevices=0
hiddevices_aftersearch=0
devices=`ls -d /sys/bus/usb/devices/*`
for device in $devices
	do
		module=`egrep usbhid $device'/uevent'`
		if test "$module" != ""
		then
			hiddevices_aftersearch=`expr $hiddevices_aftersearch + 1 | bc`
		fi
done

if test  $hiddevices_aftersearch -gt $hiddevices
then
	for device in $devices
		do
			module=`egrep usbhid $device'/uevent'`
			if test "$module" != ""
			then
				devClass=`cat $device'/bInterfaceClass'`
				devProtocol=`cat $device'/bInterfaceProtocol'`
				if test $devClass -eq $HID_CLASS
				then
					if test $devProtocol -eq $hidSearch
					then
						if test $hidSearch -eq $HID_PROTOCOL_NONE
						then
							test_print_trc "HID None Found"
						    test_print_trc " ******************************************************"
        				    test_print_trc "  HID None found. Exiting USB HID tests..."
				            test_print_trc " ******************************************************"
					         exit 2;
						elif test $hidSearch -eq $HID_PROTOCOL_KEYBOARD
						then	
							test_print_trc "HID Keyboard Found"
							devProtocol_keyboard=$devProtocol
							HID_Get_Event $device
						elif test $hidSearch -eq $HID_PROTOCOL_MOUSE
						then
							test_print_trc "HID Mouse Found"
							devProtocol_mouse=$devProtocol
							HID_Get_Event $device
						fi
					fi
				fi
			fi
	done
else
		    test_print_trc " ******************************************************"
		    test_print_trc "  USB HID devices not found. Exiting USB HID tests..."
		    test_print_trc " ******************************************************"
		    exit 2;
fi
}

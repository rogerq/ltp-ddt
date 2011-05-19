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

# Get mtd partition number either by default or by command line option based on device type
# Input: device_type;
#	 partition number(optional)
# Output: partition_number

source "st_log.sh"
source "common.sh"

############################### CLI Params ###################################
if [ $# -lt 1 ]; then
        echo "Error: Invalid Argument Count"
        echo "Syntax: $0 <device_type> [partition_number](optional)"
        exit 1
fi
DEVICE_TYPE=$1
if [ $# -ge 2 -a -n $2 ]; then
	PARTITION=$2
fi

############################# Functions #######################################
# Assume the type is always nand for nand device
# this function is to check if the device /dev/mtd$partition is nand based on /sys entry
# if this /sys/ entry doesn't exist, don't do any check. 
is_nand() {
        MTD_DEV="/mtdblock$1"
        if [ -e /sys/block/$MTD_DEV/device/type ]; then
                TYPE=`cat /sys/block/$MTD_DEV/device/type`
                if [ $TYPE != 'nand' ]; then
                        return 1
                fi
        fi
        return 0
}

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
        # example to redefine the params based on platform
        am389x-evm)

                case $DEVICE_TYPE in
                        nand)
                                : ${PARTITION:='7'}
                                ;;
                        spi)
                                : ${PARTITION:='3'}
                                ;;
                esac

                ;;
esac

# If no ARCH/DRIVER/SOC/MACHINE specific value, take the value below
case $DEVICE_TYPE in
        nand)
                : ${PARTITION:='4'}
                ;;
        spi)
                : ${PARTITION:='2'}
                ;;
        nor)
                : ${PARTITION:='3'}
                ;;
        *)
                die "Wrong device_type input in $0 script"
                ;;
esac

# Make sure partition number matches device
is_nand $PARTITION
RST=$?
case $DEVICE_TYPE in
        nand)
		if [ $RST -ne 0 ]; then
			die "mtd$PARTITION is not nand device"
		fi
                ;;
        spi)
		if [ $RST -eq 0 ]; then
			die "mtd$PARTITION is nand instead of spi"
		fi
                ;;
        nor)
		if [ $RST -eq 0 ]; then
			die "mtd$PARTITION is nand instead of nor"
		fi
                ;;
        *)
                die "Wrong device_type input in $0 script"
                ;;
esac

echo $PARTITION

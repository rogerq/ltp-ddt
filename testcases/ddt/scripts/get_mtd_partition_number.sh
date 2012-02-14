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
source "mtd_common.sh"

############################### CLI Params ###################################
if [ $# -lt 1 ]; then
	echo "Error: Invalid Argument Count"
	echo "Syntax: $0 <device_type> [partition_number](optional)"
	exit 1
fi
DEVICE_TYPE=$1
if [ $# -ge 2 -a -n $2 ]; then
	PARTITION=$2
  echo "$PARTITION"
  exit 0
fi

############################# Functions #######################################

PARTITION=`get_mtd_biggest_part $DEVICE_TYPE`

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
        am387x-evm)

                case $DEVICE_TYPE in
                        nand)
                                : ${PARTITION:='9'}
                                ;;
                        spi)
                                : ${PARTITION:='4'}
                                ;;
                esac

                ;;
        am181x-evm)

                case $DEVICE_TYPE in
                        spi)
                                : ${PARTITION:='2'}
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
                : ${PARTITION:='4'}
                ;;
        nor)
                : ${PARTITION:='3'}
                ;;
        *)
                die "Wrong device_type input in $0 script"
                ;;
esac

################
echo "$PARTITION"

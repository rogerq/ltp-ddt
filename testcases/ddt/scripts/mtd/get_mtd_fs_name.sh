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

# Get mtd filesystem partition name for various platform
#
# Input: device_type;
# Output: fs part name

source "common.sh"

############################### CLI Params ###################################
if [ $# -lt 1 ]; then
        echo "Error in $0: Invalid Argument Count"
        echo "Syntax: $0 device_type (Ex, nand, spi) "
        exit 1
fi
DEVICE_TYPE=$1

# Set default value
: ${FS_PART_NAME:='File System'}

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
    am335x-evm)
        case $DEVICE_TYPE in
          nand)
            FS_PART_NAME="File System"
            ;;
          spi)
            FS_PART_NAME="File System"
            ;;
          *)
            die "Unknown device type! Can not get FS part name."
            ;;
        esac
		;;

	  *)
        die "Invalid Machine name! No FS part name found"
                ;;
esac

echo $FS_PART_NAME

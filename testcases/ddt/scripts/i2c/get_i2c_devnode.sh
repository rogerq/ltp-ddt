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

# Get I2C devnode for different platform

# Input: 
# Output: i2c_devnode

source "common.sh"

############################### CLI Params ###################################

############################ DYNAMIC-DEFINED Params ##############################
I2C_NODELIST=`ls /dev/i2c*` || die "No I2C nodes available"
I2C_DEVNODE=`echo $I2C_NODELIST | cut -f1 -d' '` || die "I2C devnode is not found"

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

echo $I2C_DEVNODE

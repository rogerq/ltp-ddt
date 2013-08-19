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

# Get I2C Bus Number for different platform. Usually this number 
# is the number appeared in /dev/i2c-1 or /dev/i2c/1

# Input: 
# Output: i2cbus number

source "common.sh"

############################### CLI Params ###################################

############################ DYNAMIC-DEFINED Params ##############################
I2C_NODES=`ls /dev/i2c*` || die "No I2C nodes availble"
I2CBUS_NUM=`echo $I2C_NODES | cut -f1 -d' ' | awk '{print substr ($0, length($0))}'` || die "I2C bus number is not found"

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
  am37x-evm) I2CBUS_NUM=3;;
  am335x-evm) I2CBUS_NUM=2;;
esac

#: ${I2CBUS_NUM:=1}
echo $I2CBUS_NUM

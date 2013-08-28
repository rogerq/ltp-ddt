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

# Get testable I2C Bus Numbers for different platform. 
# by default return all the i2c bus numbers

# Input: 
# Output: Testable i2cbus numbers

source "common.sh"

############################### CLI Params ###################################

############################ DYNAMIC-DEFINED Params ##############################
i2c_nodes=`ls /dev/i2c*` || die "No I2C nodes availble"
i2cbus_numbers=""
for i2c_node in $i2c_nodes; do
  i2cbus_num=`echo $i2c_node | awk '{print substr ($0, length($0))}'`
  i2cbus_numbers+=" $i2cbus_num"
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
  omap5-evm) i2cbus_numbers="0 4";;
esac

if [ -z "i2cbus_numbers" ]; then
  die "Failed to get i2cbus_numbers"
else
  echo $i2cbus_numbers
fi

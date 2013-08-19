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

# Get spidev node 

source "common.sh"

############################### CLI Params ###################################

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

SPIDEV_NODES=`ls /dev/spidev*`
if [ $? -eq 0 ]; then
  SPIDEV_NODE=`echo ${SPIDEV_NODES} |cut -d" " -f1`
  echo ${SPIDEV_NODE}
else
  die "spidev node doesn't exist. Please enable it in kernel"
fi

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
# @desc Get usb controller name for a driver.
# @params 
#        Input: usb device type like "usb", "usbxhci" 
# @returns name of usb controller by splitting driver name. Return value would be name of
# ip or controller such as xhci, ehci, musb, davinci, ...
# @history 2013-04-26: First version

source "common.sh"  # Import do_cmd(), die() and other functions

############################# Functions #######################################

################################ CLI Params ####################################
if [ $# -ne 1 ]; then
  echo "Error: Invalid Argument Count"
  echo "Syntax: $0 <device_type like 'usb' and 'usbxhci'>"
  exit 1
fi
DEVICE_TYPE=$1
interface=none
interface=`get_modular_config_names.sh $1`
set `echo $interface |awk -F: '{print $1,$2}'`
driver_name=$2
if [[ "$driver_name" =~ '-' ]]; then
  set `echo $driver_name |awk -F- '{print $1,$2}'`
else
  set `echo $driver_name |awk -F_ '{print $1,$2}'`
fi
controller_name=$1
echo $controller_name

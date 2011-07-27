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
# @desc Get modular name for a driver.
# @params 
#        Input: device type like "nand", "mmc", "spi", "sound", "graphics" 
# @returns Modular name for this driver
# @history 2011-03-22: First version

source "common.sh"  # Import do_cmd(), die() and other functions

############################# Functions #######################################

################################ CLI Params ####################################
if [ $# -ne 1 ]; then
        echo "Error: Invalid Argument Count"
        echo "Syntax: $0 <device_type like 'nand', 'mmc', 'spi', 'rtc', 'graphics'>"
        exit 1
fi
DEVICE_TYPE=$1
############################ USER-DEFINED Params ###############################

########################### REUSABLE TEST LOGIC ###############################
# DO NOT HARDCODE any value. If you need to use a specific value for your setup
# use USER-DEFINED Params section above.

# TODO: return all module names in the future. this requires the changes in several
#       modular scripts. Now, only return the last module name from MODULE_CONFIG_NAMES

MODULE_CONFIG_NAMES=`get_modular_config_names.sh "$DEVICE_TYPE"` || die "error getting config and module names: "$MODULE_CONFIG_NAMES" "
#echo "MODULE_CONFIG_NAMES: $MODULE_CONFIG_NAMES"
for pair in $MODULE_CONFIG_NAMES; do
        MODULE_NAME=`echo $pair | cut -d':' -f2`
done
echo "$MODULE_NAME"

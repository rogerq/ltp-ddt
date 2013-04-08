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
# @desc Install modules. If driver is built as module, do modprobe; 
#       otherwise, do nothing
# @params
#        Input: device type like "nand", "mmc", "spi", "sound", "graphics"
# @returns 
# @history 2011-07-27: First version

source "common.sh"  # Import do_cmd(), die() and other functions

# Let udev handle module insertion if present
udevadm --help > /dev/null && exit 0

############################# Functions #######################################

################################ CLI Params ####################################
if [ $# -lt 1 ]; then
        echo "Error: Invalid Argument Count"
        echo "Syntax: $0 <device_type like 'nand', 'mmc', 'spi', 'rtc', 'graphics'> <optional_params>"
        exit 1
fi
DEVICE_TYPE=$1
shift
OPTIONS="$*"

############################ USER-DEFINED Params ###############################
#DRIVERS='can/can_driver,edma/edma,mmc-host/sd/mmci-omap-hs,mmc-host/sdhc/mmci-omap-hs,nand/omap2_nand'

########################### REUSABLE TEST LOGIC ################################
#if [ ! -f /proc/config.gz ]; then
#  die "/proc/config.gz doesn't exist"
#fi

IS_BUILT_IN='false'
MODULE_CONFIG_NAMES=`get_modular_config_names.sh "$DEVICE_TYPE"` || die "error getting config and module names: "$MODULE_CONFIG_NAMES" "
echo "MODULE_CONFIG_NAMES: $MODULE_CONFIG_NAMES"
for pair in $MODULE_CONFIG_NAMES; do
	CONFIG_ENTRY=`echo $pair | cut -d':' -f1`
	MODULE_NAME=`echo $pair | cut -d':' -f2`
	if [ -f /proc/config.gz ]; then
 		ym=`zcat /proc/config.gz | grep $CONFIG_ENTRY'=' | cut -d'=' -f2`
		case $ym in
			m)
				IS_BUILT_IN='false'
				test_print_trc "$MODULE_NAME is built as module"
				;;
			y)
				IS_BUILT_IN='true'
				test_print_trc "$MODULE_NAME is statically built in"
				;;
			*)	
				IS_BUILT_IN='false'
				die "Could not find $CONFIG_ENTRY in config or the option was not set to m or y"
				;;
    esac
  fi
	
  # if don't know is_built_in, do insmod anyway.
  if [ $IS_BUILT_IN != 'true' ]; then
		do_cmd insmod.sh $MODULE_NAME $OPTIONS
  fi	
done




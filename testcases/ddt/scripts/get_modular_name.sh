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
#        Input: device type like "nand", "mmc", "spi", "sound" 
# @returns Modular name for this driver
# @history 2011-03-22: First version

source "common.sh"  # Import do_cmd(), die() and other functions

############################# Functions #######################################

################################ CLI Params ####################################
if [ $# -ne 1 ]; then
        echo "Error: Invalid Argument Count"
        echo "Syntax: $0 <device_type like 'nand', 'mmc', 'spi', 'rtc'>"
        exit 1
fi
DEVICE_TYPE=$1
############################ USER-DEFINED Params ###############################
# Try to avoid defining values here, instead see if possible
# to determine the value dynamically. ARCH, DRIVER, SOC and MACHINE are 
# initilized and exported by runltp script based on platform option (-P)
case $ARCH in
esac
IFS_ORIG=$IFS
IFS=","
for DRIVER in $DRIVERS
do
    case $DRIVER in
        *omap2-nand)
            nand="omap2?";;
        *davinci-nand)
            nand="davinci_nand";;

        *mmci-omap-hs)
            mmc="omap_hsmmc";;
        *davinci-mmc)
            mmc="davinci_mmc";;

        *rtc-s35390a)
            rtc="rtc-s35390a";;
	*omap_rtc)
	    rtc="rtc-omap";;	

        *davinci_spi)
            spi="davinci_spi";;
    esac
done
IFS=$IFS_ORIG

case $SOC in
esac
case $MACHINE in
    am3517-evm)
        sound="snd-soc-omap"
    ;;
esac
# Define default values for variables being overriden
: ${nand:="davinci_nand"}
: ${sound="davinci_sound"}
: ${mmc:="davinci_mmc"}
: ${spi:="davinci_spi"}
: ${usb:="musb_hdrc"}

########################### DYNAMICALLY-DEFINED Params #########################
# Try to use /sys and /proc information to determine values dynamically.
# Alternatively you should check if there is an existing script to get the
# value you want

########################### REUSABLE TEST LOGIC ###############################
# DO NOT HARDCODE any value. If you need to use a specific value for your setup
# use USER-DEFINED Params section above.

# Avoid using echo. Instead use print functions provided by st_log.sh

# Use do_cmd() (imported from common.sh) to execute your test steps.
# do_cmd() will check return code and fail the test is return code is non-zero.
#echo ${!DEVICE_TYPE}
eval MODULE_NAME=\$$DEVICE_TYPE
if [ -z $MODULE_NAME ]; then
	die 'error module name not found'
	exit 1
fi
echo $MODULE_NAME

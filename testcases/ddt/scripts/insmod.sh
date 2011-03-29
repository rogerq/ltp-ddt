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
# @desc modprobe modules.
# @params 
#       Input: modular name like "davinci_spi"
# @returns 
#	none
# @history 2011-03-22: First version

source "common.sh"  # Import do_cmd(), die() and other functions
#source "st_log.sh"  # Import log functions such as test_print_trc()

############################# Functions #######################################

################################ CLI Params ####################################
if [ $# -lt 1 ]; then
        echo "Error: Invalid Argument Count"
        echo "Syntax: $0 <modular_name> <optional_params>"
        exit 1
fi
MOD_NAME=$1
OPT_PARAMS=''
shift
if [ -n "$*" ]; then
	OPT_PARAMS="$*"
fi
############################ USER-DEFINED Params ###############################
# Try to avoid defining values here, instead see if possible
# to determine the value dynamically. ARCH, DRIVER, SOC and MACHINE are 
# initilized and exported by runltp script based on platform option (-P)
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
do_cmd depmod -a
do_cmd modprobe $MOD_NAME $OPT_PARAMS
do_cmd lsmod | grep $MOD_NAME || die "error when modprobe $MOD_NAME"
sleep 3

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

########################### DYNAMICALLY-DEFINED Params #########################

########################### REUSABLE TEST LOGIC ###############################
do_cmd depmod -a
do_cmd modprobe $MOD_NAME $OPT_PARAMS
LSMOD_NAME=`echo $MOD_NAME | sed 's/-/_/g'` || die "error getting lsmod_name"
do_cmd lsmod | grep $LSMOD_NAME || die "error when modprobe $MOD_NAME"
sleep 3

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
# @desc Check if the graphics package has been built and the required script has been run
# @params
# @returns
# @history 2012-07-27: First version

source "st_log.sh"
source "common.sh"  # Import do_cmd(), die() and other functions

############################# Functions #######################################

################################ CLI Params ####################################

############################ USER-DEFINED Params ###############################
case $ARCH in
esac
case $DRIVER in
esac
case $SOC in
esac
case $MACHINE in
        am335x-evm|am335x-sk)
                SCRIPT="335x-demo";;
        am37x-evm)
                SCRIPT="omap-demo";;
esac


########################### DYNAMICALLY-DEFINED Params #########################
# Try to use /sys and /proc information to determine values dynamically.
# Alternatively you should check if there is an existing script to get the
# value you want

########################### REUSABLE TEST LOGIC ###############################
echo "$SCRIPT"
ls /etc/init.d/"$SCRIPT" || die "Tests cannot be run as demo script is not available!"
ls '/usr/local/bin/sgx_blit_test' || do_cmd '/etc/init.d/"$SCRIPT"'


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

# @desc This script is used to display 'hello world' and 
#  check control of lcd using escape sequences.
# @params no parameters required
#
# @returns None
# @history 2012-04-27: First version

source "common.sh"
source "st_log.sh"

############################# Functions #######################################
usage()
{
	usage: ./${0##*/} [Please check the lcd while test is running]
	EOF
	exit 0
}


############################ CLI Params ###########################################
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

############################ DEFAULT Params #######################
DEV_NODE="/dev/lcd" # not expected to change

########################## Do the work #########################################
do_cmd 'echo "hello world" > $DEV_NODE'
sleep 2
do_cmd 'echo "[Ld" > $DEV_NODE'
sleep 2
do_cmd 'echo "[LD" > $DEV_NODE'
sleep 2
do_cmd 'echo "[Lc" > $DEV_NODE'
sleep 2
do_cmd 'echo "[LC" > $DEV_NODE'
sleep 2
do_cmd 'echo "[Lb" > $DEV_NODE'
sleep 2
do_cmd 'echo "[LB" > $DEV_NODE'
sleep 2
do_cmd 'echo "[Ll" > $DEV_NODE'
sleep 2
do_cmd 'echo "[Lr" > $DEV_NODE'
sleep 2
do_cmd 'echo "[LL" > $DEV_NODE'
sleep 2
do_cmd 'echo "[LR" > $DEV_NODE'
sleep 2
do_cmd 'echo "[LI" > $DEV_NODE'
sleep 2
do_cmd 'echo "[2J" > $DEV_NODE'

# End Of File


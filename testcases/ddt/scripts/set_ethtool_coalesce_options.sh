#! /bin/sh
############################################################################### 
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
###############################################################################

# @desc Set ethtool coalesce options 

source "common.sh"  # Import do_cmd(), die() and other functions

############################# Functions #######################################
usage()
{
	echo "set_eth_int_pacing.sh -d <devname> -p <coalesce parameter> -n <value to set>"
	echo " -d <devname>: Eth interface name like eth0"
  echo " -p <coalesce param>: coalesce parameter for the eth iface."
	echo " -n <value to set>: value to be set for coalesce param specified in -p like 250"
	exit 1
}

################################ CLI Params ####################################
# Please use getopts
while getopts  :d:n:p:h arg
do case $arg in
        d)      ETH_IFACE="$OPTARG";; 
        p)      ETH_PARAM="$OPTARG";;
        n)      PARAM_VAL="$OPTARG";; 
        h)      usage;;
        :)      ;; 
        \?)     ;;
esac
done
# Define default values if possible

############################ USER-DEFINED Params ###############################
# Try to avoid defining values here, instead see if possible
# to determine the value dynamically. ARCH, DRIVER, SOC and MACHINE are 
# initilized and exported by runltp script based on platform option (-P)
case $ARCH in
esac
case $DRIVER in
esac
case $SOC in
esac
case $MACHINE in
esac
# Define default values for variables being overriden

########################### DYNAMICALLY-DEFINED Params #########################
# Try to use /sys and /proc information to determine values dynamically.
# Alternatively you should check if there is an existing script to get the
# value you want

########################### REUSABLE TEST LOGIC ###############################
# DO NOT HARDCODE any value. If you need to use a specific value for your setup
# use USER-DEFINED Params section above.
pacing_support=`ethtool -c $ETH_IFACE | grep $ETH_PARAM: | cut -d ' ' -f1`
if [ $pacing_support == "$ETH_PARAM:" ]; then
    CUR_VAL=`ethtool -c $ETH_IFACE | grep $ETH_PARAM: |cut -d' ' -f2`
    if [ $CUR_VAL -ne $PARAM_VAL ]; then
        test_print_trc "Set $ETH_PARAM to $PARAM_VAL"
          do_cmd "ethtool -C $ETH_IFACE $ETH_PARAM $PARAM_VAL"
    else
          test_print_trc "The current $ETH_PARAM is already set to $PARAM_VAL so skipping calling ethtool!"
    fi
else
  die "DUT does not have support PACING"
fi

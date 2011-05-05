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

# @desc Run IPERF http://sourceforge.net/projects/iperf/ 

source "common.sh"  # Import do_cmd(), die() and other functions
source "st_log.sh"  # Import log functions such as test_print_trc()

############################# Functions #######################################
usage()
{
	echo "run_iperf.sh -H <host> [other iperf options (see iperf help)"
	echo " -H <host>: IP address of Host running iperf in server mode"
	echo " all other args are passed as-is to iperf"
	echo " iperf help:"
        echo `iperf -h`
	exit 1
}

################################ CLI Params ####################################
# Please use getopts
while getopts  :H:h arg
do case $arg in
        H)      IPERFHOST="$OPTARG"; shift 2 ;;
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

: ${IPERFHOST:=`cat /proc/cmdline | awk '{for (i=1; i<=NF; i++) { print $i} }' | grep 'nfsroot=' | awk -F '[=:]' '{print $2}'`}

########################### REUSABLE TEST LOGIC ###############################
# DO NOT HARDCODE any value. If you need to use a specific value for your setup
# use USER-DEFINED Params section above.

[ -n "$IPERFHOST" ] || die "IPERF server IP address could not be determined \
dynamically. Please specify it when calling the script. \
(i.e. run_iperf.sh -H <host>)"

#IPERFCMD=`echo $* | sed -r s/-H[[:space:]]+[0-9\.]+/-c $IPERFHOST/`

test_print_trc "Starting IPERF TEST"

do_cmd "iperf -c ${IPERFHOST} $*"


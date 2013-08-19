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

# @desc Run test with one or more infinite background loads
# @params d) load(s) seperated by ~ 
#         a) action
# @returns 
# @history 2011-03-05: First version

source "common.sh"  # Import do_cmd(), die() and other functions

############################# Functions #######################################
usage()
{
cat <<-EOF >&2
        usage: ./${0##*/} [-a ACTION] [-d LOADS] 
        -d LOADS 	action/test runs under these LOADS seperated by ~ 
        -a ACTION 	action or test need to be run 
        -h Help         print this usage
EOF
exit 0
}

############################ Script Variables ##################################
# Define default valus if possible

################################ CLI Params ####################################
# Please use getopts
while getopts  :a:d:h arg
do case $arg in
        a)      ACTION="$OPTARG";;
        d)      LOADS="$OPTARG"
                #NL=`echo $LOADS | awk -F';' '{print NF}'`
                ;;
        h)      usage;;
        :)      die "$0: Must supply an argument to -$OPTARG.";; 
        \?)     die "Invalid Option -$OPTARG ";;
esac
done

############################ USER-DEFINED Params ###############################
# Try to avoid defining values here, instead see if possible
# to determine the value dynamically. ARCH, DRIVER, SOC and MACHINE are 
# initilized and exported by runltp script based on platform option (-P)

# Define default values for variables being overriden
case $ARCH in
esac
case $DRIVER in
esac
case $SOC in
esac
case $MACHINE in
esac

########################### DYNAMICALLY-DEFINED Params #########################

########################### REUSABLE TEST LOGIC ###############################
test_print_trc "Running $ACTION under loads..."
OIFS=$IFS
IFS='~'

for LOAD in $LOADS
do
        test_print_trc "Load---------------$LOAD"
	while true
	do
		do_cmd $LOAD
		#eval $LOAD
	done &
done
IFS=$OIFS
do_cmd "$ACTION"





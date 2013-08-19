#! /bin/sh
############################################################################### 
# Copyright (C) 2013 Texas Instruments Incorporated - http://www.ti.com/
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

# @desc returns numbers of interrupts raised for a given irq on a given cpu
# @params -i irq number or name (as shown in /proc/interrupts) 
#         -c cpu number
# @history 2013-04-22: First version

source "common.sh"  # Import do_cmd(), die() and other functions

############################# Functions #######################################
usage()
{
	echo "$0 -i <irq number> -c <0-based cpu number>."
	exit 1
}

############################ Script Variables ##################################
# Define default valus if possible
IRQ_NUM=''
CPU_NUM=''

################################ CLI Params ####################################
# Please use getopts
while getopts  :i:c:h arg
do case $arg in
        i)      IRQ_NUM="$OPTARG";;
	c)	CPU_NUM="$OPTARG";;
        h)      usage;;
        :)      die "$0: Must supply an argument to -$OPTARG.";; 
        \?)     die "Invalid Option -$OPTARG ";;
esac
done

########################### REUSABLE TEST LOGIC ###############################
# DO NOT HARDCODE any value. If you need to use a specific value for your setup
# use USER-DEFINED Params section above.
if [ "x$IRQ_NUM" == "x" ]; then
	die "Must supply -i <irq number> argument"
fi
if [ "x$CPU_NUM" == "x" ]; then
	die "Must supply -c <cpu number> argument"
fi

interrupts=`cat /proc/interrupts | grep "${IRQ_NUM}:" | cut -d':' -f 2`
INT_NUM=`echo $interrupts | cut -d' ' -f $(( $CPU_NUM + 1 ))`

if [ "x$INT_NUM" == "x" ]; then
	die "Could not parse number of interrupts for IRQ $IRQ_NUM and CPU $CPU_NUM"
fi
# Return number of interrupts
echo $INT_NUM


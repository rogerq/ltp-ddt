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

# @desc Briefly describe what the script does.
# @params d) y/n
# @returns Display/capture device node
# @history 2011-03-05: First version

source "common.sh"  # Import do_cmd(), die() and other functions

############################# Functions #######################################
usage()
{
cat <<-EOF >&1
        usage: ./${0##*/} [-d DISPLAY] 
        -d DISPLAY  	y/n depending on whether a display or capture device driver node is required
        -h Help      print this usage
EOF
exit 0
}

############################ Script Variables ##################################
DISPLAY="y"

################################ CLI Params ####################################
# Please use getopts
while getopts  :d:h arg
do case $arg in
	 d)      DISPLAY="$OPTARG";;
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
# Try to use /sys and /proc information to determine values dynamically.
# Alternatively you should check if there is an existing script to get the
# value you want

########################### REUSABLE TEST LOGIC ###############################
# The driver names are for omap3.
NODE=0
for devicenode in $(echo `ls /sys/class/video4linux/video?/name`)
do

        DRIVER=`cat /sys/class/video4linux/video$NODE/name`
	 if [ "$DISPLAY" == "y" ];then
        	 if [ "$DRIVER" == "omap_vout" ];then
              	  DEVICE_DISP="/dev/video$NODE"
	                echo "$DEVICE_DISP"
			  exit 0
		 fi
        else
		 if [ "$DRIVER" == "OMAP3 ISP CCDC output" ];then
		 	  DEVICE_CAP="/dev/video$NODE"
             		  echo "$DEVICE_CAP"
			  exit 0
	        fi
	 fi
        NODE=$((NODE+1))
done




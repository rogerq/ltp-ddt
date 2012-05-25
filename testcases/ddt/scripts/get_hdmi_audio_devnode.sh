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

# search for USB audio devices

source "common.sh"
source "st_log.sh"


############################# Functions #######################################

usage()
{
	cat <<-EOF >&2
	usage: ./${0##*/} [-m DISPLAY_MODE]
	-m DISPLAY_MODE	 Possible video modes  are 1080p-60,1080i-60,720p-60 and 480p-60.
	EOF
	exit 0
}

RET=-1
MODULE_EXISTS=1

isModuleLoaded() {
     MODULE_NAME=$*
     lsmod | grep `echo $MODULE_NAME | sed 's/\.ko$//g'` > /dev/null
     RET=$?
     if test "$RET" = "0"
     then
         MODULE_LOADED=1;
     else
         MODULE_LOADED=0;
     fi
}

############################### CLI Params ###################################

while getopts :m: arg
	do case $arg in
		m)  DISPLAY_MODE="$OPTARG";;
		:)  die "$0: Must supply an argument to -$OPTARG.";;
		\?) die "Invalid Option -$OPTARG ";;
	esac
done
############################ DEFAULT Params #######################

: ${DISPLAY_MODE:='720p-60'}

############# Do the work ###########################################

SOUND_DEVICE=-1
SOUND_CARD=-1
SYSLINK_DRIVER=syslink.ko
VPSS_DRIVER=vpss.ko
SLAVELOADER=slaveloader
MODULES_DIR=/home/root/hdmi_modules


case $MACHINE in
	*am389x-evm|am387x-evm)
		FBDEV_DRIVER=ti81xxfb.ko
		V4L2_DISPLAY_DRIVER=ti81xxvo.ko
		ON_CHIP_HDMI_DRIVER=ti81xxhdmi.ko
			;;
esac

case $MACHINE in
	*am389x-evm)
		M3_BIOS_FIRMWARE=ti816x_hdvpss.xem3
			;;
esac

# Load VPSS and HDMI Module
isModuleLoaded "$SYSLINK_DRIVER"
if test "$MODULE_LOADED" = "0"
then
	insmod $MODULES_DIR/$SYSLINK_DRIVER 
	sleep 1
fi

eval $MODULES_DIR/$SLAVELOADER list $MODULES_DIR/$M3_BIOS_FIRMWARE | grep Running > /dev/null
RET=$?
if test "$RET" = "1"
then
	eval $MODULES_DIR/$SLAVELOADER startup VPSS-M3 $MODULES_DIR/$M3_BIOS_FIRMWARE > /dev/null
sleep 1
fi

isModuleLoaded "$VPSS_DRIVER"
if test "$MODULE_LOADED" = "0"
then
	insmod $MODULES_DIR/$VPSS_DRIVER
	sleep 1
fi

isModuleLoaded "$FBDEV_DRIVER"
if test "$MODULE_LOADED" = "0"
then
	insmod $MODULES_DIR/$FBDEV_DRIVER
	sleep 1
fi


isModuleLoaded "$V4L2_DISPLAY_DRIVER"
if test "$MODULE_LOADED" = "0"
then
	insmod $MODULES_DIR/$V4L2_DISPLAY_DRIVER
	sleep 1
fi


isModuleLoaded "$ON_CHIP_HDMI_DRIVER"
if test "$MODULE_LOADED" = "0"
then
	insmod $MODULES_DIR/$ON_CHIP_HDMI_DRIVER
	sleep 1
fi

# Disable Venc 
eval echo 0 > /sys/devices/platform/vpss/display0/enabled

# Select Display Mode 
eval echo $DISPLAY_MODE > /sys/devices/platform/vpss/display0/mode

# Enable Venc 
eval echo 1 > /sys/devices/platform/vpss/display0/enabled

# Get ALSA HDMI devices 
SOUND_CARD=`aplay -l | grep -i hdmi | cut -f 2,7 -d ' ' | cut -c 1`
SOUND_DEVICE=`aplay -l | grep -i hdmi | cut -f 2,7 -d ' ' | cut -c 4`

if test "$SOUND_CARD" = " "
then
	eval echo "No HDMI sound card found"
	exit 1
fi
if test "$SOUND_DEVICE" = " "
then
	eval echo "No HDMI sound device found"
	exit 2
fi
SOUND_DEVNODE=$SOUND_CARD,$SOUND_DEVICE
echo $SOUND_DEVNODE



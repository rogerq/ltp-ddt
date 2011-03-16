#!/bin/sh

# Translate device type to what users see
# Input: DEVICE_TYPE like 'nand', 'spi', 'mmc', 'usb'
# Output: 'mtd' or 'storage_device' or no translation

#source "$LTPROOT/scripts/ddt/st_log.sh"
source "st_log.sh"

DEVICE_TYPE=$1

############################ Default Params ##############################
DEV_TYPE=$DEVICE_TYPE

if [ $DEVICE_TYPE == "nand" ] || [ $DEVICE_TYPE == "nor" ] || [ $DEVICE_TYPE == "spi" ]; then
	DEV_TYPE="mtd"
fi
if [ $DEVICE_TYPE == "mmc" ] || [ $DEVICE_TYPE == "usb" ] || [ $DEVICE_TYPE == "ata" ]; then
        DEV_TYPE="storage_device"
fi



############################ USER-DEFINED Params ##############################
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

echo $DEV_TYPE

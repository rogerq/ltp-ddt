#!/bin/sh

# Translate device type to what users see
# Input: DEVICE_TYPE like 'nand', 'spi', 'mmc', 'usb'
# Output: 'mtd' or 'storage_device' or no translation

source "st_log.sh"

if [ $# -ne 1 ]; then
        echo "Error: Invalid Argument Count"
        echo "Syntax: $0 <device_type>"
        exit 1
fi

DEVICE_TYPE=$1

############################ Default Params ##############################
DEV_TYPE=$DEVICE_TYPE

if [ $DEVICE_TYPE == "nand" ] || [ $DEVICE_TYPE == "nor" ] || [ $DEVICE_TYPE == "spi" ]; then
	DEV_TYPE="mtd"
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

#!/bin/sh

# Get mtd partition number either by default or by command line option based on device type
# Input: device_type;
#	 partition number(optional)
# Output: partition_number

source "st_log.sh"
source "common.sh"

############################### CLI Params ###################################
if [ $# -lt 1 ]; then
        echo "Error: Invalid Argument Count"
        echo "Syntax: $0 <device_type> [partition_number](optional)"
        exit 1
fi
DEVICE_TYPE=$1
if [ $# -ge 2 -a -n $2 ]; then
	PARTITION=$2
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
        # example to redefine the params based on platform
        am3517-evm)

                case $DEVICE_TYPE in
                        nand)
#                                : ${PARTITION:='8'}
                                ;;
                esac

                ;;
esac

# If no ARCH/DRIVER/SOC/MACHINE specific value, take the value below
case $DEVICE_TYPE in
        nand)
                : ${PARTITION:='4'}
                ;;
        spi)
                : ${PARTITION:='2'}
                ;;
        nor)
                : ${PARTITION:='3'}
                ;;
        *)
                test_print_err "Wrong device_type input in $0 script"
		exit 1
                ;;
esac


echo $PARTITION

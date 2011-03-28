#! /bin/sh
# @desc Get modular name for a driver.
# @params 
#        Input: driver_name like "nand", "mmc", "spi", "sound" 
# @returns Modular name for this driver
# @history 2011-03-22: First version

source "common.sh"  # Import do_cmd(), die() and other functions

############################# Functions #######################################

################################ CLI Params ####################################
if [ $# -ne 1 ]; then
        echo "Error: Invalid Argument Count"
        echo "Syntax: $0 <driver_name>"
        exit 1
fi
DRIVER_NAME=$1
############################ USER-DEFINED Params ###############################
# Try to avoid defining values here, instead see if possible
# to determine the value dynamically. ARCH, DRIVER, SOC and MACHINE are 
# initilized and exported by runltp script based on platform option (-P)
case $ARCH in
esac
IFS_ORIG=$IFS
IFS=","
for DRIVER in $DRIVERS
do
    case $DRIVER in
        *omap2-nand)
            nand="omap2?";;
        *davinci-nand)
            nand="davinci_nand";;

        *mmci-omap-hs)
            mmc="omap_hsmmc";;
        *davinci-mmc)
            mmc="davinci_mmc";;

        *omap-rtc)
            rtc="rtc-s35390a";;

        *davinci_spi)
            spi="davinci_spi";;
    esac
done
IFS=$IFS_ORIG

case $SOC in
esac
case $MACHINE in
    am3517-evm)
        sound="snd-soc-omap"
    ;;
esac
# Define default values for variables being overriden
: ${nand:="davinci_nand"}
: ${sound="davinci_sound"}
: ${mmc:="davinci_mmc"}
: ${spi:="davinci_spi"}

########################### DYNAMICALLY-DEFINED Params #########################
# Try to use /sys and /proc information to determine values dynamically.
# Alternatively you should check if there is an existing script to get the
# value you want

########################### REUSABLE TEST LOGIC ###############################
# DO NOT HARDCODE any value. If you need to use a specific value for your setup
# use USER-DEFINED Params section above.

# Avoid using echo. Instead use print functions provided by st_log.sh

# Use do_cmd() (imported from common.sh) to execute your test steps.
# do_cmd() will check return code and fail the test is return code is non-zero.
#echo ${!DRIVER_NAME}
eval MODULE_NAME=\$$DRIVER_NAME
if [ -z $MODULE_NAME ]; then
	die 'error module name not found'
	exit 1
fi
echo $MODULE_NAME

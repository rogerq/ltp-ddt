#! /bin/sh
# @desc Determine Write and read performance of MTD devices
# @params d) device type: NAND, NOR, SPI
#         f) Filesystem type (i.e. jffs2)
# @history 2011-03-05: First version

#source "${LTPROOT}/scripts/ddt/common.sh"
source "common.sh"

############################# Functions #######################################
usage()
{
	echo "Here is the usage of this script"
	exit 1
}


################################ CLI Params ###################################

while getopts  :f:n:d:B:s:d:h arg
do case $arg in
        f)      FS_TYPE="$OPTARG";;
        B)      BUFFER_SIZES="$OPTARG";;
        s)      FILE_SIZE="$OPTARG";;
        d)      DEVICE_TYPE="$OPTARG";;
        h)      usage;;
        :)      test_print_err "$0: Must supply an argument to -$OPTARG." >&2
                exit 1
                ;;

        \?)     test_print_err "Invalid Option -$OPTARG ignored." >&2
                usage
                exit 1
                ;;
esac
done
: ${FS_TYPE:='jffs2'}
: ${BUFFER_SIZES:='102400 256000 512000 1048576 5242880'}
: ${FILE_SIZE:='100'}
: ${DEVICE_TYPE:='nand'}

case $DEVICE_TYPE in
	nand)
		: ${PARTITION:='4'}
		;;
	spi)
		: ${PARTITION:='2'}
		;;
	nor)
		: ${PARTITION:='4'}
		;;
	*)	: ${PARTITION:='3'}
		;;
esac

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
	am3517-evm) PARTITION='3';;
#	am180x-evm) PARTITION='2';;
esac

########################### DYNAMICALLY-DEFINED Params ########################
MNT_POINT=/mnt/partition_$DEVICE_TYPE
# Use /sys/bus/spi/devices/spi1.0/mtd/mtd info to determine best partition to 

########################### REUSABLE TEST LOGIC ###############################
# DO NOT HARDCODE any value. If you need to use a specific value for your setup
# use user-defined Params section above.
test_print_trc "STARTING MTD PERFORMANCE Test"
test_print_trc "DEV_TYPE:${DEVICE_TYPE}"
test_print_trc "FS_TYPE:${FS_TYPE}"
test_print_trc "MOUNT POINT:${MNT_POINT}"
test_print_trc "PARTITION:${PARTITION}"
test_print_trc "FILE SIZE:${FILE_SIZE}"


# CALL Filesystem Performance helper script
#export LDTP_ROOT=$LDTP_ROOT
do_cmd filesystem_perf_test.sh -f "$FS_TYPE" -n "/dev/mtdblock$PARTITION" -m "$MNT_POINT" -B "\"$BUFFER_SIZES\"" -s "$FILE_SIZE" -d "$DEVICE_TYPE"

#if [ $? -ne 0 ]; then
#    test_print_err "FATAL: error while running filesystem_perf_test"
#    exit 1
#fi


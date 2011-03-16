#! /bin/sh
# @desc Determine Write and read performance of MTD devices
# @params d) device type: NAND, NOR, SPI
#         f) Filesystem type (i.e. jffs2)
# @history 2011-03-05: First version

source "common.sh"
source "mtd_common.sh"

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
	n)	PARTITION="$OPTARG";;
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

############################ USER-DEFINED Params ##############################

: ${FS_TYPE:='jffs2'}
: ${BUFFER_SIZES:='102400 256000 512000 1048576 5242880'}
: ${FILE_SIZE:='100'}
: ${DEVICE_TYPE:='nand'}
if [ -z $PARTITION ]; then
	PARTITION=`get_mtd_partition_number.sh "$DEVICE_TYPE"` || die "error while getting mtd partition number"
fi

MNT_POINT=/mnt/partition_$DEVICE_TYPE

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
do_cmd filesystem_perf_test.sh -f "$FS_TYPE" -n "$MTD_BLK_DEV$PARTITION" -m "$MNT_POINT" -B "\"$BUFFER_SIZES\"" -s "$FILE_SIZE" -d "$DEVICE_TYPE"

#if [ $? -ne 0 ]; then
#    test_print_err "FATAL: error while running filesystem_perf_test"
#    exit 1
#fi


#! /bin/sh
# @desc Helper for file system read/write performance 
# @params d) device type: NAND, NOR, SPI
#         f) Filesystem type (i.e. jffs2)
# @history 2011-03-05: First version

source "st_log.sh"
source "common.sh"

############################# Functions #######################################
usage()
{
	cat <<-EOF >&2
	usage: ./${0##*/} [-f FS_TYPE] [-n DEVICE_NODE] [-m MOUNT POINT] [-B BUFFER SIZES] [-s FILE SIZE] [-d DEVICE TYPE]
	-f FS_TYPE	filesystem type like jffs2, ext2, etc
	-n DEVICE_NODE	block device node like /dev/mtdblock4, /dev/sda1
	-m MNT POINT	mount point like /mnt/mmc
	-B BUFFER_SIZES	buffer sizes for perf test like '102400 256000 512000 1048576 5242880'
	-s FILE SIZE 	file size in MB for perf test
	-d DEVICE_TYPE	device type like 'mtd', 'mmc', 'usb' etc
	-h Help 	print this usage
	EOF
	exit 0
}



############################### CLI Params ###################################
if [ $# == 0 ]; then
	echo "Please provide options; see usage"
	usage
	exit 1
fi

while getopts  :f:n:m:B:s:d:h arg
do case $arg in
	f)	FS_TYPE="$OPTARG";;
	n)	DEV_NODE="$OPTARG";;
	m)	MNT_POINT="$OPTARG";;
	B)	BUFFER_SIZES="$OPTARG";;
	s)	FILE_SIZE="$OPTARG";;
	d)	DEVICE_TYPE="$OPTARG";;
	h)	usage;;
	:)	test_print_trc "$0: Must supply an argument to -$OPTARG." >&2
		exit 1 
		;;

	\?)	test_print_trc "Invalid Option -$OPTARG ignored." >&2
		usage 
		exit 1
		;;
esac
done

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

: ${BUFFER_SIZES:='102400 256000 512000 1048576 5242880'}
: ${FILE_SIZE:='100'}
########################### DYNAMICALLY-DEFINED Params ########################

########################### REUSABLE TEST LOGIC ###############################
# DO NOT HARDCODE any value. If you need to use a specific value for your setup
# use user-defined Params section above.
test_print_trc "STARTING FILE SYSTEM PERFORMANCE Test"
test_print_trc "FS_TYPE:${FS_TYPE}"
test_print_trc "DEV_NODE:${DEV_NODE}"
test_print_trc "MOUNT POINT:${MNT_POINT}"
test_print_trc "BUFFER SIZES:${BUFFER_SIZES}"
test_print_trc "FILE SIZE:${FILE_SIZE}MB"
test_print_trc "DEVICE_TYPE:${DEVICE_TYPE}"

# check if input are valid for this machine
DEVICE_PART_SIZE=`get_device_part_size.sh -d $DEVICE_TYPE -n $DEV_NODE` || die "error while getting device partition size"
#if [ $? -ne 0 ]; then
#    test_print_err "FATAL: error while getting device partition size for $DEVICE_TYPE"
#    exit 1
#fi
test_print_trc "Device Partition Size is $DEVICE_PART_SIZE"
[ $(( $FILE_SIZE * $MB )) -gt $DEVICE_PART_SIZE ] && die "File Size: $FILE_SIZE MB is not less than or equal to Device Partition Size: $DEVICE_PART_SIZE"

# run filesystem perf test
do_cmd "mkdir -p $MNT_POINT"
for BUFFER_SIZE in $BUFFER_SIZES; do
	test_print_trc "BUFFER SIZE = $BUFFER_SIZE"
	test_print_trc "Checking if Buffer Size is valid"
	[ $BUFFER_SIZE -gt $(( $FILE_SIZE * $MB )) ] && die "Buffer size provided: $BUFFER_SIZE is not less than or equal to File size $FILE_SIZE MB"

	test_print_trc "Checking if it is already mounted; if yes, unmount it; otherwise, contiune."
	do_cmd "mount" | grep $DEV_NODE && do_cmd "umount $DEV_NODE"

	test_print_trc "Erasing this partition completely"
	do_cmd blk_device_erase_partition.sh -d $DEVICE_TYPE -n $DEV_NODE
	test_print_trc "Mounting the partition"
	do_cmd blk_device_do_mount.sh -n "$DEV_NODE" -f "$FS_TYPE" -d "$DEVICE_TYPE" -m "$MNT_POINT"

	do_cmd filesystem_tests -write -file $MNT_POINT/test_file -buffer_size $BUFFER_SIZE -file_size $FILE_SIZE -performance 
	# should do umount and mount before read to force to write to device
	do_cmd "umount $DEV_NODE"
	do_cmd "mount -t $FS_TYPE $DEV_NODE $MNT_POINT"
	do_cmd filesystem_tests -read -file $MNT_POINT/test_file -buffer_size $BUFFER_SIZE -file_size $FILE_SIZE -performance 

	do_cmd "sync"

	test_print_trc "Unmount the device"
	do_cmd "umount $DEV_NODE"
done



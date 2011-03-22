#! /bin/sh

# Perform dd read write concurrently on blk device like mtd, mmc, mount point 
# Input  

source "common.sh"
source "st_log.sh"
source "mtd_common.sh"

############################### CLI Params ###################################

while getopts  :d:f:n:b:c:h arg
do case $arg in
        n)      
		# optional param
		PART="$OPTARG";;
        d)      DEVICE_TYPE="$OPTARG";;
        f)      FS_TYPE="$OPTARG";;
        b)      DD_BUFSIZE="$OPTARG";;
        c)      DD_CNT="$OPTARG";;
        h)      usage;;
        :)      test_print_trc "$0: Must supply an argument to -$OPTARG." >&2
                exit 1
                ;;

        \?)     test_print_trc "Invalid Option -$OPTARG ignored." >&2
                usage
                exit 1
                ;;
esac
done

############################ DEFAULT Params #######################
DEV_TYPE=`get_device_type_map.sh "$DEVICE_TYPE"` || die "error getting device type"
case $DEV_TYPE in
	mtd)
		if [ -z $PART ]; then
			PART=`get_mtd_partition_number.sh "$DEVICE_TYPE"` || die "error getting mtd partition number"
		fi
		MTD_BLK_DEV_NODE="$MTD_BLK_DEV$PART"
		test_print_trc "MTD_BLK_DEV_NODE: $MTD_BLK_DEV$PART"

		DEV_NODE=$MTD_BLK_DEV_NODE
	;;
	storage_device*)
		DEV_NODE=`get_devnode_for_non_mtd_device.sh $DEVICE_TYPE` || die "error getting mmc blk dev node"
	;;
	*)
		test_print_err "Invalid device type in $0 script"
		exit 1;
	;;
esac

MNT_POINT=/mnt/partition_$DEVICE_TYPE
: ${FS_TYPE:='jffs2'}

############# Do the work ###########################################
test_print_trc "Checking if the device is already mounted; if yes, unmount it; otherwise, contiune."
do_cmd "mount" | grep $DEV_NODE && do_cmd "umount $DEV_NODE"
test_print_trc "Erasing this partition completely"
do_cmd blk_device_erase_partition.sh -d $DEVICE_TYPE -n $DEV_NODE
do_cmd blk_device_do_mount.sh -n "$DEV_NODE" -f "$FS_TYPE" -d "$DEVICE_TYPE" -m "$MNT_POINT"

test_print_trc "Doing write concurrence test"
do_cmd dd if=/dev/zero of=$MNT_POINT/test1.file bs=$DD_BUFSIZE count=$DD_CNT &
do_cmd dd if=/dev/zero of=$MNT_POINT/test2.file bs=$DD_BUFSIZE count=$DD_CNT
do_cmd sleep 10
FILE_SIZE=$((`convert_to_bytes.sh $DD_BUFSIZE` * `convert_to_bytes.sh $DD_CNT`))
echo `get_file_size.sh $MNT_POINT/test1.file` | grep $FILE_SIZE || die "error: file1 size is not expected file size"
echo `get_file_size.sh $MNT_POINT/test2.file` | grep $FILE_SIZE || die "error: file2 size is not expected file size"

#do_cmd dd if=$MNT_POINT/test.file of=/dev/null bs=$DD_BUFSIZE count=$DD_CNT
do_cmd rm $MNT_POINT/test1.file $MNT_POINT/test2.file

test_print_trc "Umounting device"
do_cmd "umount $DEV_NODE"




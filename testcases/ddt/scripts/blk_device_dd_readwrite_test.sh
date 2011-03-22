#! /bin/sh

# Perform dd read write test on blk device like mtd, mmc, mount point 
# Input  

source "common.sh"
source "st_log.sh"
source "mtd_common.sh"

############################### CLI Params ###################################

while getopts  :d:f:m:n:b:c:i:l:h arg
do case $arg in
        n)      
		# optional param
		DEV_NODE="$OPTARG";;
        d)      DEVICE_TYPE="$OPTARG";;
        f)      FS_TYPE="$OPTARG";;
        m)      MNT_POINT="$OPTARG";;
        b)      DD_BUFSIZE="$OPTARG";;
        c)      DD_CNT="$OPTARG";;
	i) 	IO_OPERATION="$OPTARG";;
	l) 	TEST_LOOP="$OPTARG";;
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
		if [ -z $DEV_NODE ]; then
			PART=`get_mtd_partition_number.sh "$DEVICE_TYPE"` || die "error getting mtd partition number"
			DEV_NODE="$MTD_BLK_DEV$PART"
			test_print_trc "MTD_BLK_DEV_NODE: $DEV_NODE"
		fi

		if [ $DD_BUFSIZE == "erase_size" ]; then
			DD_BUFSIZE=`get_mtd_erase_size.sh $PART` || die "error getting mtd erase size"
		fi

	;;
	storage_device*)
		if [ -z $DEV_NODE ]; then
			DEV_NODE=`get_devnode_for_non_mtd_device.sh $DEVICE_TYPE` || die "error getting mmc blk dev node"
		fi
	;;
	*)
		test_print_err "Invalid device type in $0 script"
		exit 1;
	;;
esac

: ${MNT_POINT:=/mnt/partition_$DEVICE_TYPE}
: ${FS_TYPE:='jffs2'}
: ${IO_OPERATION:='read'}
: ${TEST_LOOP:='1'}
############# Do the work ###########################################
test_print_trc "Checking if the device is already mounted; if yes, unmount it; otherwise, contiune."
do_cmd "mount" | grep $DEV_NODE && do_cmd "umount $DEV_NODE"
test_print_trc "Erasing this partition completely"
do_cmd blk_device_erase_partition.sh -d $DEVICE_TYPE -n $DEV_NODE
do_cmd blk_device_do_mount.sh -n "$DEV_NODE" -f "$FS_TYPE" -d "$DEVICE_TYPE" -m "$MNT_POINT"

test_print_trc "Doing read/write test for $TEST_LOOP times"
x=0
while [ $x -lt $TEST_LOOP ]
do
	do_cmd date	
	case $IO_OPERATION in
		write)
			do_cmd dd if=/dev/zero of=$MNT_POINT/test.file bs=$DD_BUFSIZE count=$DD_CNT
		;;
		read)
			do_cmd dd if=/dev/zero of=$MNT_POINT/test.file bs=$DD_BUFSIZE count=$DD_CNT
			do_cmd dd if=$MNT_POINT/test.file of=/dev/null bs=$DD_BUFSIZE count=$DD_CNT
		;;
		*)
		test_print_err "Invalid IO operation type in $0 script"
		exit 1;
		;;	
	esac
	do_cmd rm $MNT_POINT/test.file
	x=$((x+1))
	do_cmd date
done
test_print_trc "Umounting device"
do_cmd "umount $DEV_NODE"




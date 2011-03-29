#! /bin/sh

# Perform dd read write test on blk device like mtd, mmc, mount point 
# Input  

source "common.sh"
source "st_log.sh"
source "mtd_common.sh"

############################# Functions #######################################
usage()
{
cat <<-EOF >&2
        usage: ./${0##*/} [-n DEV_NODE] [-d DEVICE_TYPE] [-f FS_TYPE] [-m MNT_POINT] [-b DD_BUFSIZE] [-c DD_CNT] [-i IO_OPERATION] [-l TEST_LOOP]
        -n DEV_NODE     optional param; device node like /dev/mtdblock2; /dev/sda1
        -f FS_TYPE      filesystem type like jffs2, ext2, etc
        -m MNT_POINT    mount point
        -b DD_BUFSIZE   dd buffer size for 'bs'
        -c DD_CNT       dd count for 'count'
        -i IO_OPERATION IO operation like 'write_in_bg', 'read_in_bg', default is 'write_in_bg'
        -d DEVICE_TYPE  device type like 'nand', 'mmc', 'usb' etc
        -l TEST_LOOP    test loop for r/w. default is 1.
        -h Help         print this usage
EOF
exit 0
}

############################### CLI Params ###################################

while getopts  :d:f:m:n:b:c:i:M:l:h arg
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
	M) 	MOD_NAME="$OPTARG";;
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
if [ -z $DEV_NODE ]; then
        DEV_NODE=`get_blk_device_node.sh "$DEVICE_TYPE"` || die "error getting device node for $DEVICE_TYPE"
fi

: ${MNT_POINT:=/mnt/partition_$DEVICE_TYPE}
: ${FS_TYPE:='jffs2'}
: ${IO_OPERATION:='write_in_bg'}
: ${TEST_LOOP:='1'}
############# Do the work ###########################################
test_print_trc "Erasing or Formatting this partition"
do_cmd blk_device_erase_format_part.sh -d $DEVICE_TYPE -n $DEV_NODE -f $FS_TYPE
do_cmd blk_device_do_mount.sh -n "$DEV_NODE" -f "$FS_TYPE" -d "$DEVICE_TYPE" -m "$MNT_POINT"

test_print_trc "Doing read/write test for $TEST_LOOP times"
x=0
while [ $x -lt $TEST_LOOP ]
do
	do_cmd date	
	case $IO_OPERATION in
		write_in_bg)
			do_cmd dd if=/dev/zero of=$MNT_POINT/test.file bs=$DD_BUFSIZE count=$DD_CNT &
		;;
		read_in_bg)
			do_cmd dd if=/dev/zero of=$MNT_POINT/test.file bs=$DD_BUFSIZE count=$DD_CNT
			do_cmd dd if=$MNT_POINT/test.file of=/dev/null bs=$DD_BUFSIZE count=$DD_CNT &
		;;
		*)
		test_print_err "Invalid IO operation type in $0 script"
		exit 1;
		;;	
	esac
	rmmod $MOD_NAME && exit 1 
	#rmmod $MOD_NAME && exit 1 || exit 0 
	do_cmd rm $MNT_POINT/test.file
	x=$((x+1))
	do_cmd date
done
test_print_trc "Umounting device"
do_cmd "umount $DEV_NODE"




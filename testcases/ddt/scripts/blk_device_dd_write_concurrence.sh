#! /bin/sh

# Perform dd read write concurrently on blk device like mtd, mmc, mount point 
# Input  

source "common.sh"
source "st_log.sh"
source "mtd_common.sh"

############################# Functions #######################################
usage()
{
cat <<-EOF >&2
        usage: ./${0##*/} [-n DEV_NODE] [-d DEVICE_TYPE] [-f FS_TYPE] [-b DD_BUFSIZE] [-c DD_CNT] 
        -n DEV_NODE     optional param; device node like /dev/mtdblock2; /dev/sda1
        -f FS_TYPE      filesystem type like jffs2, ext2, etc
        -b DD_BUFSIZE   dd buffer size for 'bs'
        -c DD_CNT       dd count for 'count'
        -d DEVICE_TYPE  device type like 'nand', 'mmc', 'usb' etc
        -h Help         print this usage
EOF
exit 0
}

############################### CLI Params ###################################

while getopts  :d:f:n:b:c:h arg
do case $arg in
        n)      
		# optional param
		DEV_NODE="$OPTARG";;
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
if [ -z $DEV_NODE ]; then
	DEV_NODE=`get_blk_device_node.sh "$DEVICE_TYPE"` || die "error getting device node for $DEVICE_TYPE"
fi

MNT_POINT=/mnt/partition_$DEVICE_TYPE
: ${FS_TYPE:='jffs2'}

############# Do the work ###########################################
test_print_trc "Checking if the device is already mounted; if yes, unmount it; otherwise, contiune."
do_cmd "mount" | grep $DEV_NODE && do_cmd "umount $DEV_NODE"
test_print_trc "Erasing this partition completely"
do_cmd blk_device_erase_format_part.sh -d $DEVICE_TYPE -n $DEV_NODE -f $FS_TYPE
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




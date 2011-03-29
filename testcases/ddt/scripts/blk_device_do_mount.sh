#! /bin/sh

# Perform mount and check if mount ok
# Input  

source "common.sh"
source "st_log.sh"
source "mtd_common.sh"

############################# Functions #######################################
usage()
{
cat <<-EOF >&2
        usage: ./${0##*/} [-f FS_TYPE] [-n DEV_NODE] [-m MNT_POINT] [-d DEVICE TYPE]
        -f FS_TYPE      filesystem type like jffs2, ext2, etc
	-n DEV_NODE	device_node like /dev/mtdblock4
	-m MNT_POINT	mount point
        -d DEVICE_TYPE  device type like 'nand', 'mmc', 'usb' etc
        -h Help         print this usage
EOF
exit 0
}
############################### CLI Params ###################################

while getopts  :d:f:n:m:h arg
do case $arg in
        n)      
		# optional param
		DEV_NODE="$OPTARG";;
        d)      DEVICE_TYPE="$OPTARG";;
        f)      FS_TYPE="$OPTARG";;
        m)      MNT_POINT="$OPTARG";;
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
: ${FS_TYPE:='jffs2'}
: ${MNT_POINT:="/mnt/partition_$DEVICE_TYPE"}

############# Do the work ###########################################
DEVNODE_ENTRY=`get_devnode_entry.sh "$DEV_NODE" "$DEVICE_TYPE"` || die "error getting devnode entry for $DEV_NODE"
test_print_trc "Umount $DEV_NODE or $DEVNODE_ENTRY if it is mounted"
do_cmd "mount" | grep $DEV_NODE && do_cmd "umount $DEV_NODE"
do_cmd "mount" | grep $DEVNODE_ENTRY && do_cmd "umount $DEVNODE_ENTRY"
sleep 2
#do_cmd "mount" | grep $DEV_NODE && do_cmd "umount $DEV_NODE"
#test_print_trc "Erasing this partition completely"
#do_cmd erase_partition.sh -d $DEVICE_TYPE -n $DEV_NODE
[ -d $MNT_POINT ] || do_cmd mkdir -p $MNT_POINT
test_print_trc "Mounting the partition"
do_cmd "mount -t $FS_TYPE $DEV_NODE $MNT_POINT"
do_cmd "mount | grep $DEV_NODE"


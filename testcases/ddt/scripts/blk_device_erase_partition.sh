#! /bin/sh

# erase or format storage device based on type
# Input $DEVICE_TYPE 	like 'nand', 'spi', 'mmc', 'usb'
#	$DEV_NODE	like '/dev/mtdblock3', '/dev/mmcblk0p1', '/dev/sda1'
#	$FS_TYPE	like 'jffs2', 'ext2', 'vfat'

source "common.sh"
source "st_log.sh"

usage()
{
        echo "This is the usage of this script"
}

while getopts :d:n:f: arg
do case $arg in
        d)
                DEVICE_TYPE=$OPTARG ;;
        n)
                DEV_NODE=$OPTARG ;;
        f)
                FS_TYPE=$OPTARG ;;
        \?)
		echo "Invalid Option -$OPTARG ignored." >&2
                usage
                exit 1
                ;;
esac

done


test_print_trc "DEVICE TYPE: $DEVICE_TYPE"
test_print_trc "DEVICE NODE: $DEV_NODE"

############################ DEFAULT Params #######################
MKFS="mkfs.$FS_TYPE"
FLASH_ERASEALL="flash_eraseall"


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
	am3517-evm)
		MKFS="mkfs.$FS_TYPE"	#Just an example to show how to overwrite the default value.
		;;
esac

# translate DEVICE_TYPE to DEV_TYPE (mtd or storage_device)
DEV_TYPE=`get_device_type_map.sh $DEVICE_TYPE` || die "error while translating device type"
#if [ $? -ne 0 ]; then
#    test_print_err "FATAL: error while mapping $DEVICE_TYPE to the type what users see"
#    exit 1
#fi

# do erase or format for following device type.
if [ $DEV_TYPE == "mtd" ]; then
exit 0
	test_print_trc "device type is mtd"
	CHAR_DEV_NODE=`echo $DEV_NODE | sed s/block//` || die "error while getting mtd char dev_node"
	[ ! -c $CHAR_DEV_NODE ] && die "$CHAR_DEV_NODE is not char device" 
        do_cmd "$FLASH_ERASEALL $CHAR_DEV_NODE"
fi
if [ $DEV_TYPE == "storage_device*" ]; then
        do_cmd "$MKFS $DEV_NODE"
fi



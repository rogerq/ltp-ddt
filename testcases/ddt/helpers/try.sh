#! /bin/sh

# erase or format storage device based on type
# Input $DEVICE_TYPE 	like 'nand', 'spi', 'mmc', 'usb'
#	$DEV_NODE	like '/dev/mtdblock3', '/dev/mmcblk0p1', '/dev/sda1'
#	$FS_TYPE	like 'jffs2', 'ext2', 'vfat'

#source "$LDTP_ROOT/helper/common.sh"
source "common.sh"
#source "$LDTP_ROOT/helper/st_log.sh"
source "st_log.sh"

usage()
{
        echo "This is the usage of this script"
}

while getopts :d: arg
do case $arg in
        d)
                MY_STR=$OPTARG ;;
        \?)
		echo "Invalid Option -$OPTARG ignored." >&2
                usage
                exit 1
                ;;
esac

done


echo $MY_STR | grep 'abc'


#! /bin/sh
# 
# Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/
#  
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License as 
# published by the Free Software Foundation version 2.
# 
# This program is distributed "as is" WITHOUT ANY WARRANTY of any
# kind, whether express or implied; without even the implied warranty
# of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 

# erase or format storage device based on type
# Input $DEVICE_TYPE 	like 'nand', 'spi', 'mmc', 'usb'
#	$DEV_NODE	like '/dev/mtdblock3', '/dev/mmcblk0p1', '/dev/sda1'
#	$FS_TYPE	like 'jffs2', 'ext2', 'vfat'

source "common.sh"
source "st_log.sh"

############################# Functions #######################################
usage()
{
cat <<-EOF >&2
        usage: ./${0##*/} [-f FS_TYPE] [-n DEV_NODE] [-d DEVICE TYPE]
        -f FS_TYPE      filesystem type like jffs2, ext2, etc
        -n DEV_NODE	device node like /dev/mtdblock3; /dev/sda1 
        -d DEVICE_TYPE  device type like 'nand', 'mmc', 'usb' etc
        -h Help         print this usage
EOF
exit 0
}

while getopts :d:n:f:h arg
do case $arg in
        d)
                DEVICE_TYPE=$OPTARG ;;
        n)
                DEV_NODE=$OPTARG ;;
        f)
                FS_TYPE=$OPTARG ;;
	h)
		usage ;;
        \?)
		echo "Invalid Option -$OPTARG ignored." >&2
                usage
                exit 1
                ;;
esac
done

test_print_trc "DEVICE TYPE: $DEVICE_TYPE"
test_print_trc "DEVICE NODE: $DEV_NODE"
test_print_trc "FS TYPE: $FS_TYPE"

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
esac

# translate DEVICE_TYPE to DEV_TYPE (mtd or not)
DEV_TYPE=`get_device_type_map.sh $DEVICE_TYPE` || die "error while translating device type"

# if mount, umount it first before erase or format
DEVNODE_ENTRY=`get_devnode_entry.sh "$DEV_NODE" "$DEVICE_TYPE"` || die "error getting devnode entry for $DEV_NODE"
test_print_trc "Umount $DEV_NODE or $DEVNODE_ENTRY if it is mounted"
do_cmd "mount" | grep $DEV_NODE && do_cmd "umount $DEV_NODE"
do_cmd "mount" | grep "$DEVNODE_ENTRY' '" && do_cmd "umount $DEVNODE_ENTRY"

sleep 3

# do erase or format for following device type.
case $DEV_TYPE in
	mtd)
		test_print_trc "device type is mtd"
		CHAR_DEV_NODE=`echo $DEV_NODE | sed s/block//` || die "error while getting mtd char dev_node"
		[ ! -c $CHAR_DEV_NODE ] && die "$CHAR_DEV_NODE is not char device"
	#exit 0
		do_cmd "$FLASH_ERASEALL $CHAR_DEV_NODE"
		sleep 1
	;;
	*)
		do_cmd "$MKFS $DEV_NODE"
	;;
esac



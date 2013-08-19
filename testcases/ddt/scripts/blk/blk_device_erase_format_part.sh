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
# Input $DEVICE_TYPE  like 'nand', 'spi', 'mmc', 'usb'
# $DEV_NODE like '/dev/mtdblock3', '/dev/mmcblk0p1', '/dev/sda1'
# $FS_TYPE like 'jffs2', 'ext2', 'vfat'

source "common.sh"
source "mtd_common.sh"

############################# Functions #######################################
usage()
{
cat <<-EOF >&2
        usage: ./${0##*/} [-f FS_TYPE] [-n DEV_NODE] [-d DEVICE TYPE] [-m MNTPNT]
        -f FS_TYPE      filesystem type like jffs2, ext2, etc
        -n DEV_NODE device node like /dev/mtdblock3; /dev/sda1 
        -d DEVICE_TYPE  device type like 'nand', 'mmc', 'usb' etc
        -m MNT_POINT
        -h Help         print this usage
EOF
exit 0
}

while getopts :d:n:f:m:h arg
do case $arg in
        d)
                DEVICE_TYPE=$OPTARG ;;
        n)
                DEV_NODE=$OPTARG ;;
        f)
                FS_TYPE=$OPTARG ;;
        m)  
                MNT_POINT=$OPTARG ;;
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
if [ "$FS_TYPE" = "vfat" ]; then
  MKFS="mkfs.${FS_TYPE} -F 32"
else
  MKFS="mkfs.${FS_TYPE}"
fi
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
    PART=`get_mtd_partnum_from_devnode.sh "$DEV_NODE"` || die "error getting partition number: $PART"
    if [ "$FS_TYPE" = "ubifs" ]; then
      # TODO: volume name is set to 'test'. Later on, get the name from proc mtd or sys entry.
      # TODO: assume we only test one partition attched to ubi0, so hardcode to ubi0. 
      #       need to add ubi device number as input option
      UBI_DEVICE_NUM=0
      UBI_VOLUME_NUM=0
      VOL_NAME="test"

      # need erase all blocks with option -e since some blocks may contain non-ubifs data 
      MTD_ERASEBLOCKS=`get_mtd_eraseblocks.sh $PART` || die "error getting how many eraseblocks mtd$PART have: $MTD_ERASEBLOCKS"
      SUBPAGE_SIZE=`get_subpagesize "/dev/mtd$PART" ` || die "error getting subpage size for mtd$PART: ${SUBPAGE_SIZE}"
      PAGE_SIZE=`get_pagesize "/dev/mtd$PART" ` || die "error getting page size for mtd$PART: ${PAGE_SIZE}"
      test_print_trc "mtd eraseblocks: $MTD_ERASEBLOCKS"
      test_print_trc "mtd subpage size: $SUBPAGE_SIZE"
      test_print_trc "mtd pagesize: $PAGE_SIZE"
      # before format, make sure it is not attached.
      test_print_trc "Detach $CHAR_DEV_NODE and ubi$UBI_DEVICE_NUM"
      #umount "ubi$UBI_DEVICE_NUM:$VOL_NAME"
      test_print_trc "umount "$MNT_POINT""
      umount "$MNT_POINT"
      test_print_trc "ubidetach -p "$CHAR_DEV_NODE""
      ubidetach -p "$CHAR_DEV_NODE"
      test_print_trc "ubidetach -d $UBI_DEVICE_NUM"
      ubidetach -d $UBI_DEVICE_NUM
      ls /dev/ub*
      # For now, temp hard code -s and -O.
      #do_cmd "ubiformat "$CHAR_DEV_NODE" -s 512 -O 2048 -e $MTD_ERASEBLOCKS -y -q"
      do_cmd "ubiformat "$CHAR_DEV_NODE" -s "$SUBPAGE_SIZE" -O "$PAGE_SIZE" -e $MTD_ERASEBLOCKS -y -q"
      do_cmd "ubiattach /dev/ubi_ctrl -m "$PART" -O "$PAGE_SIZE" "
      do_cmd "ls /dev/ub*"
      
      # before mk vol, need remove the existing one
      # First find out the name of existing volume name if it exists
      if [ -e /sys/class/ubi/ubi"$UBI_DEVICE_NUM"_"$UBI_VOLUME_NUM" ]; then
        $VOLUME_NAME=`cat /sys/class/ubi/ubi"$UBI_DEVICE_NUM"_"$UBI_VOLUME_NUM"/name`  
        do_cmd "ubirmvol /dev/ubi$UBI_DEVICE_NUM -N "$VOLUME_NAME" "
      fi
      LEB_CNT=`cat /sys/class/ubi/ubi0/avail_eraseblocks`
      #do_cmd "ubimkvol /dev/ubi0 -N test -s 200MiB"
      do_cmd "ubimkvol /dev/ubi$UBI_DEVICE_NUM -N "$VOL_NAME" -S $LEB_CNT"
    else
      do_cmd "$FLASH_ERASEALL $CHAR_DEV_NODE"
    fi
    sleep 1
    ;;
 *)
  if [ -n "$FS_TYPE" ]; then
   do_cmd "${MKFS} $DEV_NODE"
  fi
 ;;
esac



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

# Perform mount and check if mount ok
# Input  

source "common.sh"
source "st_log.sh"
source "mtd_common.sh"

############################# Functions #######################################
usage()
{
cat <<-EOF >&2
	usage: ./${0##*/} [-f FS_TYPE] [-n DEV_NODE] [-m MNT_POINT] [-d DEVICE TYPE] [-o MNT_MODE]
	-f FS_TYPE      filesystem type like jffs2, ext2, etc. if not specified, try
									all fs type.
	-n DEV_NODE	device_node like /dev/mtdblock4
	-m MNT_POINT	mount point
	-d DEVICE_TYPE  device type like 'nand', 'mmc', 'usb' etc
  -o MNT_MODE     mount mode: either 'sync' or 'async'. default is 'async'
	-h Help         print this usage
EOF
exit 0
}
############################### CLI Params ###################################

while getopts  :d:f:n:m:o:h arg
do case $arg in
        n)      
                # optional param
                DEV_NODE="$OPTARG";;
        d)      DEVICE_TYPE="$OPTARG";;
        f)      FS_TYPE="$OPTARG";;
        m)      MNT_POINT="$OPTARG";;
        o)      MNT_MODE="$OPTARG";;
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
: ${MNT_POINT:="/mnt/partition_$DEVICE_TYPE"}
: ${MNT_MODE:="async"}

############# Do the work ###########################################
if [ "$FS_TYPE" = "ubifs" ]; then
  # TODO: pass in ubi0 instead of hardcode
  # TODO: need check what dev_node is attached to. It may not be ubi0_0.
  VOLUME_NAME=`cat /sys/class/ubi/ubi0_0/name` || die "error getting volume name for ubi0_0"
  MNT_DEV_NODE="ubi0:$VOLUME_NAME"
else
  MNT_DEV_NODE="$DEV_NODE"
fi

# DEVNODE_ENTRY is something like /dev/mmcblk0, /dev/sda etc
DEVNODE_ENTRY=`get_devnode_entry.sh "$DEV_NODE" "$DEVICE_TYPE"` || die "error getting devnode entry for $DEV_NODE"
test_print_trc "Umount $MNT_DEV_NODE or $DEVNODE_ENTRY if it is mounted"
do_cmd "mount" | grep $MNT_DEV_NODE && do_cmd "umount $MNT_DEV_NODE"
do_cmd "mount" | grep $DEVNODE_ENTRY' ' && do_cmd "umount $DEVNODE_ENTRY"
sleep 2
#do_cmd "mount" | grep $DEV_NODE && do_cmd "umount $DEV_NODE"
#test_print_trc "Erasing this partition completely"
#do_cmd erase_partition.sh -d $DEVICE_TYPE -n $DEV_NODE
[ -d $MNT_POINT ] || do_cmd mkdir -p $MNT_POINT
test_print_trc "Mounting the partition"
if [ -n "$FS_TYPE" ]; then
  do_cmd "mount -t $FS_TYPE -o $MNT_MODE $MNT_DEV_NODE $MNT_POINT"
  do_cmd "mount | grep $MNT_DEV_NODE"
else
  DEV_TYPE=`get_device_type_map.sh "$DEVICE_TYPE"` || die "error getting device type: $DEV_TYPE"
	case $DEV_TYPE in
		mtd)
			fs_to_try="jffs2:ubifs"
		;;
		*)
			fs_to_try="vfat:ext3:ext2"
		;;
	esac	
	# try all fs to mount
	oldIFS=$IFS 
	IFS=":"
	for FS in $fs_to_try; do
  	echo "---$FS---"
		test_print_trc "Try to mount $FS"	
		mount -t $FS -o $MNT_MODE $MNT_DEV_NODE $MNT_POINT
		mount | grep $MNT_DEV_NODE
		if [ $? -eq 0 ]; then
			break
		fi
	done
	IFS=$oldIFS	
fi


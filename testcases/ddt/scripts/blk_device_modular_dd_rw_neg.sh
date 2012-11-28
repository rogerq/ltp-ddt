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

# Perform modprobe, then dd read write test on blk device like mtd, mmc, mount point 
# in the same time, remove the module to see if it can be removed
# Input  

source "common.sh"
source "st_log.sh"
source "mtd_common.sh"

############################# Functions #######################################
usage()
{
cat <<-EOF >&2
        usage: ./${0##*/} [-n DEV_NODE] [-d DEVICE_TYPE] [-f FS_TYPE] [-m MNT_POINT] [-b DD_BUFSIZE] [-c DD_CNT] [-i IO_OPERATION] [-s SKIP_RW]
	-n DEV_NODE	optional param; device node like /dev/mtdblock2; /dev/sda1
        -f FS_TYPE      filesystem type like jffs2, ext2, etc
        -m MNT_POINT	optional param; mount point; if not given, use /mnt/partition_<dev_type> 
        -b DD_BUFSIZE 	dd buffer size for 'bs'
        -c DD_CNT 	dd count for 'count'
        -i IO_OPERATION	IO operation like 'write_in_bg', 'read_in_bg', default is 'write_in_bg'
        -d DEVICE_TYPE  device type like 'nand', 'mmc', 'usb' etc
        -h Help         print this usage
EOF
exit 0
}

############################### CLI Params ###################################
while getopts  :d:f:m:n:b:c:i:h arg
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
: ${IO_OPERATION:='read'}
: ${SKIP_RW:=0}
test_print_trc "DEV_NODE: $DEV_NODE"
test_print_trc "MNT_POINT: $MNT_POINT"
test_print_trc "FS_TYPE: $FS_TYPE"

############# Do the work ###########################################
MOD_NAME=`get_modular_name.sh "$DEVICE_TYPE"` || die "error getting modular name"
do_cmd insmod.sh $MOD_NAME
do_cmd blk_device_erase_format_part.sh -d "$DEVICE_TYPE" -n "$DEV_NODE" -f "$FS_TYPE" -m "$MNT_POINT"
do_cmd blk_device_do_mount.sh -n "$DEV_NODE" -f "$FS_TYPE" -d "$DEVICE_TYPE" -m "$MNT_POINT"

test_print_trc "Doing read/write test in the same time, remove module"
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
test_print_trc "rmmod.sh $MOD_NAME "
rmmod.sh $MOD_NAME && exit 1 
test_print_trc "Waiting for process $! to finish..."
wait $!
do_cmd rm $MNT_POINT/test.file
test_print_trc "Umounting device"
do_cmd blk_device_umount.sh -m "$MNT_POINT"


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
# Input  

source "common.sh"
source "st_log.sh"
source "mtd_common.sh"

############################# Functions #######################################
usage()
{
cat <<-EOF >&2
        usage: ./${0##*/} [-n DEV_NODE] [-d DEVICE_TYPE] [-f FS_TYPE] [-m MNT_POINT] [-b DD_BUFSIZE] [-c DD_CNT] [-i IO_OPERATION] [-l TEST_LOOP] 
	-n DEV_NODE	optional param; device node like /dev/mtdblock2; /dev/sda1
        -f FS_TYPE      filesystem type like jffs2, ext2, etc
        -m MNT_POINT	optional param; mount point; if not given, use /mnt/partition_<dev_type> 
        -b DD_BUFSIZE 	dd buffer size for 'bs'
        -c DD_CNT 	dd count for 'count'
        -i IO_OPERATION	IO operation like 'write', 'read', default is 'read'
        -d DEVICE_TYPE  device type like 'nand', 'mmc', 'usb' etc
	-l TEST_LOOP	test loop for r/w. default is 1.
        -h Help         print this usage
EOF
exit 0
}

############################### CLI Params ###################################
while getopts  :d:f:m:n:b:c:i:l:sh arg
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
: ${MNT_POINT:=/mnt/partition_$DEVICE_TYPE}
: ${IO_OPERATION:='read'}
: ${TEST_LOOP:='1'}
test_print_trc "DEV_NODE: $DEV_NODE"
test_print_trc "MNT_POINT: $MNT_POINT"
test_print_trc "FS_TYPE: $FS_TYPE"

############# Do the work ###########################################
test_print_trc "Doing insmod;read/write;rmmod test for $TEST_LOOP times"
if [ -z $DEV_NODE ]; then
  do_cmd do_modular_common.sh -d "$DEVICE_TYPE" -l "$TEST_LOOP" -a "\"do_cmd blk_device_dd_readwrite_test.sh -f "$FS_TYPE" -b "$DD_BUFSIZE" -c "$DD_CNT" -l "1" -d "$DEVICE_TYPE" \" " 
else
  do_cmd do_modular_common.sh -d "$DEVICE_TYPE" -l "$TEST_LOOP" -a "\"do_cmd blk_device_dd_readwrite_test.sh -f "$FS_TYPE" -b "$DD_BUFSIZE" -c "$DD_CNT" -l "1" -d "$DEVICE_TYPE" -n "$DEV_NODE"\" " 
fi
#x=0
#while [ $x -lt $TEST_LOOP ]
#do
#
#	MOD_NAME=`get_modular_name.sh "$DEVICE_TYPE"` || die "error getting modular name" 
#	do_cmd insmod.sh "$MOD_NAME"
#
#	if [ !$SKIP_RW ]; then
#		do_cmd blk_device_dd_readwrite_test.sh -f "$FS_TYPE" -b "$DD_BUFSIZE" -c "$DD_CNT" -l "1" -d "$DEVICE_TYPE" -n "$DEV_NODE"
#	fi
#
#	do_cmd rmmod.sh "$MOD_NAME"
#
#        x=$((x+1))
#
#done




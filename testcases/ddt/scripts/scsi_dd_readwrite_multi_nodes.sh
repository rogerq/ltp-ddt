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

# Perform dd read write test on blk device like mtd, mmc, mount point 
# Input  

source "common.sh"
source "mtd_common.sh"
source "blk_device_common.sh"

############################# Functions #######################################
usage()
{
cat <<-EOF >&2
  usage: ./${0##*/} [-n DRIVE_NAME] [-d DEVICE_TYPE] [-f FS_TYPE] [-b DD_BUFSIZE] [-c DD_CNT] [-i IO_OPERATION] [-l TEST_LOOP]  
	-n DRIVE_NAME   drive name like "a b c" to overwrite the ones find dynamically;
  -f FS_TYPE      filesystem type like jffs2, ext2, etc
  -b DD_BUFSIZE 	dd buffer size for 'bs'
  -c DD_CNT 	    dd count for 'count'
  -i IO_OPERATION	IO operation like 'wr', 'rd', default is 'wr'
  -d DEVICE_TYPE  device type like 'nand', 'mmc', 'usb' etc
	-l TEST_LOOP	  test loop for r/w. default is 1.
  -h Help         print this usage
EOF
exit 0
}

############################### CLI Params ###################################

while getopts  :d:f:m:n:b:c:i:l:swh arg
do case $arg in
        n)      DRIVE_NAME="$OPTARG";;
        d)      DEVICE_TYPE="$OPTARG";;
        f)      FS_TYPE="$OPTARG";;
        b)      DD_BUFSIZE="$OPTARG";;
        c)      DD_CNT="$OPTARG";;
        i) 	    IO_OPERATION="$OPTARG";;
        l) 	    TEST_LOOP="$OPTARG";;
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
: ${IO_OPERATION:='wr'}
: ${TEST_LOOP:='1'}

############# Do the work ###########################################
if [ -z "$DRIVE_NAME" ]; then
  DRIVES=`find_all_scsi_drives "$DEVICE_TYPE"` || die "Error when calling find_all_scsi_drives: $DRIVES"
fi
for drive in $DRIVES; do
  do_cmd "cat /sys/block/sd${drive}/device/model"
  MNT_POINT="/mnt/sd${drive}_$$"
  node="/dev/sd${drive}1"
  if [ -n "$FS_TYPE" ]; then
    do_cmd blk_device_dd_readwrite_test.sh -n "$node" -f "$FS_TYPE" -m "$MNT_POINT" -b "$DD_BUFSIZE" -c "$DD_CNT" -i "$IO_OPERATION" -l "$TEST_LOOP" -d "$DEVICE_TYPE" 
  else
    do_cmd blk_device_dd_readwrite_test.sh -n "$node" -m "$MNT_POINT" -b "$DD_BUFSIZE" -c "$DD_CNT" -i "$IO_OPERATION" -l "$TEST_LOOP" -d "$DEVICE_TYPE" 
  fi
done


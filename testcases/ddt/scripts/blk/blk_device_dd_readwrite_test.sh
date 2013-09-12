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

source "blk_device_common.sh"
source "mtd_common.sh"

SKIP_FORMAT=0
WRITE_TO_FILL=0
############################# Functions #######################################
usage()
{
cat <<-EOF >&2
  usage: ./${0##*/} [-n DEV_NODE] [-d DEVICE_TYPE] [-f FS_TYPE] [-m MNT_POINT] [-b DD_BUFSIZE] [-c DD_CNT] [-i IO_OPERATION] [-l TEST_LOOP] [-s SKIP_FORMAT] [-w WRITE_TO_FILLUP]
  -n DEV_NODE     optional param; device node like /dev/mtdblock2; /dev/sda1
  -f FS_TYPE      filesystem type like jffs2, ext2, etc
  -m MNT_POINT    mount point 
  -b DD_BUFSIZE   dd buffer size for 'bs'
  -c DD_CNT       dd count for 'count'
  -i IO_OPERATION IO operation like 'wr', 'cp', default is 'wr'
                  'oversize_write' is to test if driver throw error when the size > partition size
  -d DEVICE_TYPE  device type like 'nand', 'mmc', 'usb' etc
  -l TEST_LOOP    test loop for r/w. default is 1.
  -s SKIP_FORMAT  skip erase/format part and just do r/w 
  -w WRITE_TO_FILLUP keep writing different files TEST_LOOP times to device
  -h Help         print this usage
EOF
exit 0
}

compare_md5sum()
{
  FILE1=$1
  FILE2=$2
  a=$(md5sum "$FILE1"|cut -d' ' -f1)
  echo "$1: $a"
  b=$(md5sum "$FILE2"|cut -d' ' -f1)
  echo "$2: $b"
  [ "$a" = "$b" ] 
}
############################### CLI Params ###################################

while getopts  :d:f:m:n:b:c:i:l:swh arg
do case $arg in
        n)      
                # optional param
                DEV_NODE="$OPTARG";;
        d)      DEVICE_TYPE="$OPTARG";;
        f)      FS_TYPE="$OPTARG";;
        m)      MNT_POINT="$OPTARG"; MNT_POINT="${MNT_POINT}_$$";;
        b)      DD_BUFSIZE="$OPTARG";;
        c)      DD_CNT="$OPTARG";;
        i)      IO_OPERATION="$OPTARG";;
        l)      TEST_LOOP="$OPTARG";;
        s)      SKIP_FORMAT=1;;
        w)      WRITE_TO_FILL=1;; 
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
: ${MNT_POINT:=/mnt/partition_${DEVICE_TYPE}_$$}
: ${IO_OPERATION:='wr'}
: ${TEST_LOOP:='1'}

############# Do the work ###########################################
if [ -z $DEV_NODE ]; then
  DEV_NODE=`get_blk_device_node.sh "$DEVICE_TYPE"` || die "error getting device node for $DEVICE_TYPE: $DEV_NODE"
  test_print_trc "DEV_NODE returned from get_blk_device_node is: $DEV_NODE"
fi
test_print_trc "DEV_NODE: $DEV_NODE"
test_print_trc "MNT_POINT: $MNT_POINT"
test_print_trc "FS_TYPE: $FS_TYPE"

# print out the model number if possible
do_cmd printout_model "$DEV_NODE" "$DEVICE_TYPE"

if [ $SKIP_FORMAT -ne 1 ]; then 
  if [ -n "$FS_TYPE" ]; then
    do_cmd blk_device_prepare_format.sh -d "$DEVICE_TYPE" -n "$DEV_NODE" -f "$FS_TYPE" -m "$MNT_POINT"
  else
    do_cmd blk_device_prepare_format.sh -d "$DEVICE_TYPE" -n "$DEV_NODE" -m "$MNT_POINT"
  fi
fi

# find out what is FS in the device
if [ -z "$FS_TYPE" ]; then
  FS_TYPE=`mount | grep $DEV_NODE | cut -d' ' -f5 | head -1`
  test_print_trc "Current FS_TYPE: ${FS_TYPE}"
fi

remove_srcfile()
{
  echo "Removing srcfile"
  do_cmd rm "$SRC_FILE"
}

remove_testfiles()
{
  if [ "$WRITE_TO_FILL" -eq 1 ]; then
    echo "Removing test files"
    TEST_FILE="${MNT_POINT}/test_file_$$_*"
    do_cmd "rm ${TEST_FILE}"
  fi
  # umount etc
  [ $SKIP_FORMAT -eq 1 ] || do_cmd blk_device_unprepare.sh -n "$DEV_NODE" -d "$DEVICE_TYPE" -f "$FS_TYPE" -m "$MNT_POINT"
}

# execute on exit - cleanup actions
on_exit()
{
  echo "clean up......."
  remove_srcfile
  remove_testfiles
  echo "done with clean up"
}

trap on_exit EXIT 

test_print_trc "Doing read/write test for $TEST_LOOP times"
# not using tmpfs because it is too small and we don't measure performance here
#SRC_FILE='/dev/shm/srctest_file' 
SRC_FILE="/home/root/srctest_file_${DEVICE_TYPE}_$$"
do_cmd "time dd if=/dev/urandom of=$SRC_FILE bs=$DD_BUFSIZE count=$DD_CNT"
sleep 10
do_cmd ls -lh $SRC_FILE
do_cmd "df -h"

x=0
while [ $x -lt $TEST_LOOP ]
do
  echo "============R/W LOOP: $x============"
  do_cmd date  
  if [ "$WRITE_TO_FILL" -ne 1 ]; then
    TEST_FILE="${MNT_POINT}/test_file_$$"
  else
    # write to different file to fill up the device
    TEST_FILE="${MNT_POINT}/test_file_$$_${x}"
  fi
  case $IO_OPERATION in
    wr)
      do_cmd time dd if="$SRC_FILE" of="$TEST_FILE" bs=$DD_BUFSIZE count=$DD_CNT
      do_cmd "sync"
      do_cmd "echo 3 > /proc/sys/vm/drop_caches"
      #do_cmd hexdump "$SRC_FILE" > /srcfile
      #do_cmd hexdump "$TEST_FILE" > /dstfile
      #do_cmd diff /srcfile /dstfile
      test_print_trc "diff "$SRC_FILE" "$TEST_FILE" "
      diff "$SRC_FILE" "$TEST_FILE"
      if [ $? -ne 0 ]; then
        do_cmd cmp -l "$SRC_FILE" "$TEST_FILE"
      fi
      do_cmd time dd if=$TEST_FILE of=/dev/null bs=$DD_BUFSIZE count=$DD_CNT
      do_cmd "sync"
      do_cmd "echo 3 > /proc/sys/vm/drop_caches"

    ;;
    oversize_write)
        test_print_trc "time dd if="$SRC_FILE" of="$TEST_FILE" bs=$DD_BUFSIZE count=$DD_CNT > "${TMPDIR}/temp_$$" 2>&1 "
        time dd if="$SRC_FILE" of="$TEST_FILE" bs=$DD_BUFSIZE count=$DD_CNT > "${TMPDIR}/temp_$$" 2>&1
        if [ $? -ne 0 ]; then
          # check if the failure is due to out of space; if not fail the test
          cat "${TMPDIR}/temp_$$" |grep -i "No space left" || die "dd write failed!" 
          do_cmd rm "${MNT_POINT}/test_file_$$_*"
          test_print_trc "Writing a file after space is available to make sure the driver is ok"
          test_print_trc "time dd if="$SRC_FILE" of="$TEST_FILE" bs=$DD_BUFSIZE count=$DD_CNT"
          time dd if="$SRC_FILE" of="$TEST_FILE" bs=$DD_BUFSIZE count=$DD_CNT || die "Write failed after overflowing the device" && exit 0
        fi
        do_cmd time dd if=$TEST_FILE of=/dev/null bs=$DD_BUFSIZE count=$DD_CNT
        do_cmd "sync"
        do_cmd "echo 3 > /proc/sys/vm/drop_caches"
        rm "${TMPDIR}/temp_$$" 
    ;;
    write_in_bg)
      do_cmd time dd if=/dev/urandom of="$TEST_FILE" bs=$DD_BUFSIZE count=$DD_CNT &
    ;;
    cp)
      do_cmd time cp "$SRC_FILE" "${TEST_FILE}"
      do_cmd "sync"
      do_cmd "echo 3 > /proc/sys/vm/drop_caches"

      ls -lh "$SRC_FILE"
      ls -lh "${TEST_FILE}"
      do_cmd compare_md5sum "$SRC_FILE" "$TEST_FILE" 
      do_cmd time cp "${TEST_FILE}" "${TEST_FILE}_2"
      do_cmd "sync"
      do_cmd "echo 3 > /proc/sys/vm/drop_caches"

      ls -lh "${TEST_FILE}_2"
      test_print_trc "compare_md5sum "${TEST_FILE}" "${TEST_FILE}_2" "
      compare_md5sum "${TEST_FILE}" "${TEST_FILE}_2"
      if [ $? -ne 0 ]; then
        do_cmd cmp -l "${TEST_FILE}" "${TEST_FILE}_2"
      fi
      sleep 1
      do_cmd rm "${TEST_FILE}_2"
    ;;
    *)
    test_print_err "Invalid IO operation type in $0 script"
    exit 1;
    ;;  
  esac
  if [ "$WRITE_TO_FILL" -ne 1 ]; then
    do_cmd rm "$TEST_FILE"
  else
    # don't remove the testfiles so that to fillup the device
    do_cmd echo "Did not remove the testfiles in order to fillup the device"
  fi
  x=$((x+1))
  do_cmd date
done

do_cmd "df -h"



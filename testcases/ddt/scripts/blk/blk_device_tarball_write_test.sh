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

# Perform extract tar ball into both blk device and tmpfs and then
# compair the contents to check if write ok on blk device
# Input See usage() 

source "common.sh"

############################# Functions #######################################
usage()
{
cat <<-EOF >&2
  usage: ./${0##*/} [-n DEV_NODE] [-d DEVICE_TYPE] [-f FS_TYPE] [-m MNT_POINT] [-l TEST_LOOP] [-t TARBALL_SRC] 
	-n DEV_NODE	optional param; device node like /dev/mtdblock2; /dev/sda1
  -f FS_TYPE      filesystem type like jffs2, ext2, etc
  -m MNT_POINT	mount point 
  -d DEVICE_TYPE  device type like 'nand', 'mmc', 'usb' etc
	-l TEST_LOOP	test loop for r/w. default is 1.
  -t TARBALL_SRC  tarball location
  -h Help         print this usage
EOF
exit 0
}


#TODO: move the below two functions to common 
# wget tar ball
get_tarball() {
  if [ $# -ne 2 ]; then
    die "Usage: $0 <tarball-wget-src-path> <tarball-dest-path>"
  fi

  TARBALL_PATH=$2
  TARBALL_SRC=$1
  TARBALL_BASENAME=`basename "$TARBALL_SRC"`
  test_print_trc "Getting a rootfs tarball"
  if [ -f "/$TARBALL_BASENAME" ]; then
    do_cmd rm -f "/$TARBALL_BASENAME"
  fi
  do_cmd cd /
  do_cmd wget "$TARBALL_SRC"
  do_cmd mv "/$TARBALL_BASENAME" "$TARBALL_PATH"
}

# Extract tarball to destination folder
# Input see Usage
untar() {
  if [ $# -ne 2 ]; then
    die "Usage: $0 <tarball-path> <destination-folder>"
  fi
  TARBALL=$1
  DEST=$2

  if [ ! -f "$TARBALL" -o ! -d "$DEST" ]; then
    die "$TARBALL or $DEST doesn't exist!"
  fi

  # find out type
  # 'file' is not supported command in busybox now.
  # so for now hard code it to 'gzip'
  TYPE=`file "$TARBALL" |cut -d' ' -f2`
  TYPE='gzip'
  do_cmd cd "$DEST"
  case "$TYPE" in
    gzip)
      do_cmd tar xzf "$TARBALL" ;;
    bzip2)
      do_cmd tar xjf "$TARBALL" ;;
    *)
      die "Unrecognized tarball type" ;;
  esac
  do_cmd cd /
}


############################### CLI Params ###################################
main() {
while getopts  :d:f:m:n:l:t:h arg
do case $arg in
  n)  DEV_NODE="$OPTARG";;  # optional param
  d)  DEVICE_TYPE="$OPTARG";;
  f)  FS_TYPE="$OPTARG";;
  m)  MNT_POINT="$OPTARG";; # optional
	l) 	TEST_LOOP="$OPTARG";; # optional
  t)  TARBALL_SRC="$OPTARG";;
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
	DEV_NODE=`get_blk_device_node.sh "$DEVICE_TYPE"` || die "error getting device node for $DEVICE_TYPE: $DEV_NODE"
  test_print_trc "DEV_NODE returned from get_blk_device_node is: $DEV_NODE"
fi
: ${MNT_POINT:=/mnt/partition_$DEVICE_TYPE}
: ${TEST_LOOP:='1'}
test_print_trc "DEV_NODE: $DEV_NODE"
test_print_trc "MNT_POINT: $MNT_POINT"
test_print_trc "FS_TYPE: $FS_TYPE"

############# Do the work ###########################################
TAR_BALL="/test_rootfs.tgz"
LOCATION1="/dev/shm"
#Before wget this file, one may need set http_proxy in target
#TARBALL_SRC="http://arago-project.org/files/releases/opentest/opentest-0.0.20.tar.gz"
do_cmd get_tarball "$TARBALL_SRC" "$TAR_BALL"
do_cmd untar "$TAR_BALL" "$LOCATION1" 

x=0
while [ $x -lt $TEST_LOOP ]
do
  test_print_trc "=============Doing extract tarball test loop $x ..."
	do_cmd date	

	if [ -n "$FS_TYPE" ]; then
		do_cmd blk_device_prepare_format.sh -d "$DEVICE_TYPE" -n "$DEV_NODE" -f "$FS_TYPE" -m "$MNT_POINT"
	else
		do_cmd blk_device_prepare_format.sh -d "$DEVICE_TYPE" -n "$DEV_NODE" -m "$MNT_POINT"
    FS_TYPE=`mount | grep $DEV_NODE | cut -d' ' -f5`
    test_print_trc "Current FS_TYPE: ${FS_TYPE}"
	fi
  
  # untar tarball to blk device
  if [ "$MNT_POINT" = "" ]; then
    die "MNT_POINT can not be empty!"
  fi
  if [ -n "$MNT_POINT" ]; then
    do_cmd rm -rf "${MNT_POINT}/*"
  fi
  do_cmd untar "$TAR_BALL" "$MNT_POINT"
  do_cmd diff -r "$LOCATION1" "$MNT_POINT"
  if [ $? -ne 0 ]; then
    die "The contents in $MNT_POINT is different from the original tarball!!!"
  fi
	x=$((x+1))
	do_cmd date
  do_cmd rm -rf "$MNT_POINT/*"
  do_cmd blk_device_unprepare.sh -n "$DEV_NODE" -d "$DEVICE_TYPE" -f "$FS_TYPE" -m "$MNT_POINT"
  # set FS_TYPE to nothing so the device will not be formatted for the 2nd and following times
  FS_TYPE=""
done
}

# clean up
cleanup() {
  if [ -n "$LOCATION1" ]; then
    do_cmd rm -rf "${LOCATION1}/*"
  fi
  do_cmd rm -f "$TAR_BALL" 
}

trap "cleanup" 0
main "$@"




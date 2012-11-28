#!/bin/sh
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

# Functions related to block device partition and its size 

source "common.sh"

############################ Functions ################################
# This function return DEVNODE with the biggest size.
#   If the partition is boot or rootfs partition, it will be skipped.
# Input: DEV_BASE_NODE: like /dev/mmcblk0 etc
#        device_type: like 'mmc', 'usb'
# It only tested on MMCSD and it should work on other block devices
#   like usb, ata etc.
find_part_with_biggest_size() {
  if [ $# -ne 2 ]; then
    die "Usage: $0 <dev_base_node like /dev/mmcblk0, /dev/sda> <device_type like mmc>"
  fi

  DEV_BASE_NODE=$1
  DEVICE_TYPE=$2
  MATCH=`fdisk -l $DEV_BASE_NODE |grep "$DEV_BASE_NODE"p`
  #ROOT=`get_fs_root` || die "error getting fs root: $ROOT"

  SIZE_BIGGEST=0
  TMP_IFS=$IFS
  IFS=$'\n'
  for i in $MATCH
  do
    LINE=$i
    FIELD_2ND=`echo "$LINE" | awk -F " " '{print $2}'`
    DEVNODE=`echo "$LINE" | awk -F " " '{print $1}'`

    # check if this part is holding rootfs or not
    IS_ROOTFS=`is_part_rootfs "$DEVICE_TYPE" "$DEVNODE"` || die "error when calling is_part_rootfs"
    # skip boot partition
    #if [ "$FIELD_2ND" != '*' -a "$DEVNODE" != "$ROOT" ]; then
    if [ "$FIELD_2ND" != '*' -a "$IS_ROOTFS" == "no" ]; then
      SIZE=`echo "$LINE" | awk -F " " '{print $4}' | sed s/+$//`
      if [ $SIZE -gt $SIZE_BIGGEST ]; then
        SIZE_BIGGEST=$SIZE
        PART_DEVNODE="$DEVNODE"
      fi
    fi
  done
  IFS=$TMP_IFS

  if [ -z "$PART_DEVNODE" ]
  then
    die "Could not find the partition to test! All partitions on device either boot or rootfs"
  fi  
  echo $PART_DEVNODE
}

# get size of the partition with PART_DEVNODE
# return size is in 'MBytes'
get_part_size_of_devnode() {
  PART_DEVNODE=$1
  #PARTSIZE=`fdisk -l "$PART_DEVNODE" | grep "$PART_DEVNODE:" |cut -d"," -f2 |sed s/bytes//`  
  PARTSIZE=`fdisk -l "$PART_DEVNODE" | grep "Disk "$PART_DEVNODE":" | awk '{print $3 }' `
  if [ $PARTSIZE -le 0 ]; then
    die "Could not get partition size from $PART_DEVNODE"
  fi
  echo $PARTSIZE
}

# Get the partition is used for current rootfs
# Input: None
get_fs_root() {
  BOOTARGS=`cat /proc/cmdline`
  ROOT=`get_value_for_key "root" "$BOOTARGS" "="` || die "error getting root from proc cmdline: $ROOT"
  echo "$ROOT"
}

# Check if the partition is holding the root fs
# Input: partition devnode: like /dev/mmcblk0p1
#        device_type: to decide which fs to try to mount
is_part_rootfs(){
  if [ $# -ne 2 ]; then
    die "Usage: $0 <device_type like 'mmc', 'usb'> <device_node like /dev/mmcblk0p1, /dev/sda1>"
  fi
  DEVICE_TYPE=$1
  DEV_NODE=$2
  IS_MOUNTED="no"
  RTN="no"
  mount |grep "$DEV_NODE" > /dev/null && IS_MOUNTED="yes"
  if [ "$IS_MOUNTED" == "yes" ];then
    MNT_POINT=`mount |grep "$DEV_NODE" |cut -d" " -f3` 
  else
    MNT_POINT="/mnt/partition_$DEVICE_TYPE"
    do_cmd blk_device_do_mount.sh -n "$DEV_NODE" -d "$DEVICE_TYPE" -m "$MNT_POINT" > /dev/null 2>$1 
    NEED_UMOUNT="yes"
  fi
  if [ -e "$MNT_POINT/etc" -a -e "$MNT_POINT/dev" ]; then
    RTN="yes"
  fi
  if [ "$NEED_UMOUNT" == "yes" ]; then
    do_cmd blk_device_umount.sh -m "$MNT_POINT" > /dev/null 2>$1
  fi
  echo "$RTN"
}

printout_model(){
  DEV_NODE=$1
  DEV_TYPE=$2
  case "$DEV_NODE" in
    *sd*)
      BASE_SD=`echo "$DEV_NODE" |sed "s/\/dev\///" |sed "s/[0-9]$//"` 
      do_cmd "cat /sys/block/$BASE_SD/device/model"
      ;;
    *)
      test_print_trc "model info is not available for non scsi devices."
      ;;
  esac
}

# find all available scsi drives
# Input: "usb" or "sata"
# Output: drives_found
find_all_scsi_drives() {
  SCSI_DEVICE=$1
  DRIVES_FOUND=""
  DRIVES=`fdisk -l |grep "Disk /dev/sd" | cut -b 13`
  for DRIVE in $DRIVES; do
    if [ -e /sys/block/sd"$DRIVE"/device/vendor ]; then
      VENDOR=`cat /sys/block/sd"$DRIVE"/device/vendor`
      RESULT=`echo $VENDOR | grep -i "ATA"`
      case $SCSI_DEVICE in
        sata)
          if [ -n "$RESULT" ]; then
            DRIVES_FOUND="${DRIVES_FOUND} $DRIVE"
          fi
        ;;
        usb)
          if [ -z "$RESULT" ]; then
            DRIVES_FOUND="${DRIVES_FOUND} $DRIVE"
          fi
        ;;
        all)
          DRIVES_FOUND="${DRIVES_FOUND} $DRIVE"
        ;;
      esac
    fi
  done
  if [ -n "$DRIVES_FOUND" ]; then
    echo "$DRIVES_FOUND"
  else
    # if could not find match, let user know
    echo "Could not find any device node for SCSI device!"
    exit 1
  fi
}

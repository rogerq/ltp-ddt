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
# This function is get partition dev node which has the biggest size.
# This function return DEVNODE:SIZE pair.
# It only tested on MMCSD.
find_part_with_biggest_size() {
  if [ -z $1 ]; then
    die "Please provide device base node like /dev/mmcblk0, /dev/sda..."
  fi
  #DEV_BASE_NODE='/dev/mmcblk0'
  DEV_BASE_NODE=$1
  #CNT=`fdisk -l $DEV_BASE_NODE |grep -c "$DEV_BASE_NODE"p`
  MATCH=`fdisk -l $DEV_BASE_NODE |grep "$DEV_BASE_NODE"p`

  SIZE_BIGGEST=0
  TMP_IFS=$IFS
  IFS=$'\n'
  for i in $MATCH
  do
    LINE=$i
    #echo LINE: $LINE
  FIELD_2ND=`echo "$LINE" | awk -F " " '{print $2}'`
  if [ "$FIELD_2ND" != '*' ]; then
    SIZE=`echo "$LINE" | awk -F " " '{print $4}' | sed s/+$//`
    #echo size: $SIZE
    if [ $SIZE -gt $SIZE_BIGGEST ]; then
      SIZE_BIGGEST=$SIZE
      PART_DEVNODE=`echo "$LINE" | awk -F " " '{print $1}'`
    fi
  fi
  done
  IFS=$TMP_IFS

  if [ $SIZE_BIGGEST -le 0 ]; then
    die "Could not find the biggest partition!"
  fi
  #echo biggestsize: $SIZE_BIGGEST
  #echo part: $PART_DEVNODE
  #echo $PART_DEVNODE:$SIZE_BIGGEST
  echo $PART_DEVNODE

}

# get size of the partition with PART_DEVNODE
# return size is in 'bytes'
get_part_size_of_devnode() {
  PART_DEVNODE=$1
  PARTSIZE=`fdisk -l "$PART_DEVNODE" | grep "$PART_DEVNODE:" |cut -d"," -f2 |sed s/bytes//`  
  if [ $PARTSIZE -le 0 ]; then
    die "Could not get partition size from $PART_DEVNODE"
  fi
  echo $PARTSIZE
}




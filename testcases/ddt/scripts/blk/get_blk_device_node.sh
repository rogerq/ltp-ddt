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

# Get devnode for non mtd device like 'mmc', 'usb', 'usbxhci', 'sata'
# Input: DEVICE_TYPE like 'mmc', 'usb', 'usbxhci', 'sata'
# Output: DEV_NODE like /dev/mmcblk0p1 

source "common.sh"
source "mtd_common.sh"
source "blk_device_common.sh"


if [ $# -ne 1 ]; then
    echo "Error: Invalid Argument Count"
    echo "Syntax: $0 <device_type>"
    exit 1
fi
DEVICE_TYPE=$1

############################ Functions ################################
# this function is to get SCSI device usb or sata node based on by-id
# input is either 'sata' or 'usb' or 'usbxhci'
 
find_scsi_node() {
  SCSI_DEVICE=$1
    case $SCSI_DEVICE in
      sata)
        file=`ls /dev/disk/by-id/*-part1|grep -i s*ata`
        if [[ ! -z "$file" ]]; then
        DEV_NODE="/dev/""$(basename $(readlink $file))"
         echo $DEV_NODE
         exit 0
        fi
      ;;
      usb|usbxhci)
        usb_cnt_interface=`get_usb_controller_name.sh "$SCSI_DEVICE"` 
        file=`ls /dev/disk/by-path/*-part1|grep -i "$usb_cnt_interface"|head -1`
        if [[ ! -z "$file" ]]; then
        DEV_NODE="/dev/""$(basename $(readlink $file))"
         echo $DEV_NODE
         exit 0
        fi
      ;;
    esac
  # if could not find match, let user know
  echo "Could not find device node for SCSI device!"
  exit 1
}

# return: /dev/mmcblk0 etc
find_emmc_basenode() {
    emmc_node=`ls /dev/mmcblk* |grep boot |head -1 |sed s'/boot[0-9]*//' `
    echo $emmc_node
}

find_mmc_basenode() {
    emmc_node=`find_emmc_basenode` 
    if [ -n "$emmc_node" ]; then
      mmc_node=`ls /dev/mmcblk* |sed s",$emmc_node.*$,,g" |grep -E ".*blk[[:digit:]]+$" |head -1`
    else
      mmc_node=`ls /dev/mmcblk* |grep -E ".*blk[[:digit:]]+$" |head -1`
    fi  
    echo $mmc_node
}

# create one test partition if mmc or emmc doesn't have any partition on it
# and create initial file system on it. Choose mkfs.vfat since this operation is fast
#   $1: basenode like /dev/mmcblk0, /dev/mmcblk1
create_one_partition() {
    basenode=$1
    ls ${basenode}p*
    if [ $? -ne 0 ]; then
      echo -e "p\np\nn\np\n1\n\n\np\nw\n" | fdisk $basenode
      mkfs.vfat ${basenode}p1
    fi
}

############################ Default Params ##############################
DEV_TYPE=`get_device_type_map.sh "$DEVICE_TYPE"` || die "error getting device type: $DEV_TYPE"
case $DEV_TYPE in
        mtd)
          PART=`get_mtd_partition_number.sh "$DEVICE_TYPE"` || die "error getting mtd partition number: $PART"
          DEV_NODE="$MTD_BLK_DEV$PART"
        ;;
        mmc)
          mmc_basenode=`find_mmc_basenode`
          if [ -z "$mmc_basenode" ]; then
            die "Could not fine mmc basenode"
          fi
          DEV_NODE=`find_part_with_biggest_size "$mmc_basenode" "mmc"` || die "error getting partition with biggest size: $DEV_NODE"
        ;;
        emmc)
          emmc_basenode=`find_emmc_basenode`
          if [ -z "$emmc_basenode" ]; then
            die "Could not fine emmc basenode"
          fi
          # create one test partition if emmc doesn't have any partition on it
          create_one_partition $emmc_basenode > /dev/null
          DEV_NODE=`find_part_with_biggest_size "$emmc_basenode" "emmc"` || die "error getting partition with biggest size: $DEV_NODE"
        ;;
        usb)
          DEV_NODE=`find_scsi_node "usb"` || die "error getting usb node: $DEV_NODE" 
        ;;
        usbxhci)
          DEV_NODE=`find_scsi_node "usbxhci"` || die "error getting usbxhci node: $DEV_NODE" 
        ;;
        ata)
          DEV_NODE="/dev/hda1"
        ;;
        sata)
          DEV_NODE=`find_scsi_node "sata"` || die "error getting sata node: $DEV_NODE"
        ;;
        *)
          die "Invalid device type in $0 script"
        ;;
esac

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

######################### Logic here ###########################################
if [ -n "$DEV_NODE" ]; then
  echo $DEV_NODE
else
  die "Was not able to get devnode to test. Backtrace:: $DEV_NODE ::"
fi

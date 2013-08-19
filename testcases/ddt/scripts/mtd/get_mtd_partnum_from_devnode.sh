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

# Get partition number from block mtd device node
# Input: BLK_DEV_NODE like /dev/mtdblock3
# Output: partition_number

source "st_log.sh"
source "common.sh"

if [ $# -ne 1 ]; then
	echo "Error: Invalid Argument Count"
	echo "Syntax: $0 blk_dev_node"
	exit 1
fi

BLK_DEV_NODE=$1

#PART=`echo $BLK_DEV_NODE | awk '{print substr ($0, length($0))}'` || die "mtd partition number is not found"
PART=`echo $BLK_DEV_NODE | sed s/"\/dev\/mtdblock"//` || die "mtd partition number is not found"

echo $PART


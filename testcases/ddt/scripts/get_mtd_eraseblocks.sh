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

# Get how many erase blocks of one mtd partition
# Input: Partition Number like 3
# Output: MTD_ERASEBLOCKS

source "common.sh"

if [ $# -ne 1 ]; then
	echo "Error: Invalid Argument Count"
	echo "Syntax: $0 partition_number"
	exit 1
fi

PART=$1
MTD_SIZE=`get_mtd_size.sh $PART` || die "Error getting mtd$PART size: $MTD_SIZE"
MTD_ERASE_SIZE=`get_mtd_erase_size.sh $PART` || die "Error getting mtd$PART erase size: $MTD_ERASE_SIZE"
MTD_ERASEBLOCKS=`echo "scale=0; $MTD_SIZE/$MTD_ERASE_SIZE" | bc `
#echo $((0x$SIZE))
echo $MTD_ERASEBLOCKS





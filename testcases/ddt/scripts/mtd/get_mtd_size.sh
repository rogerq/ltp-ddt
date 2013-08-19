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

# Get size of one mtd partition
# Input: Partition Number like 3
# Output: MTD_SIZE

source "st_log.sh"
source "common.sh"
source "mtd_common.sh"

if [ $# -ne 1 ]; then
	echo "Error: Invalid Argument Count"
	echo "Syntax: $0 partition_number"
	exit 1
fi

PART=$1
MTD_PROC_ENTRY=`cat /proc/mtd | grep "$MTD_CHAR$PART"` || die "Could not retrieve info from mtd proc entry for mtd$PART"
SIZE=`echo $MTD_PROC_ENTRY | cut -d ' ' -f2` || die "Could not get the mtd size"
#TEMP=`echo $SIZE | tr "a-z" "A-Z"` || die "Could not convert mtd size from a-z to A-Z"
#SIZE=`echo "ibase=16; ${TEMP}" | bc` || die "Mtd size could not be converted from Hex to Decimal"
#echo $SIZE
echo $((0x$SIZE))

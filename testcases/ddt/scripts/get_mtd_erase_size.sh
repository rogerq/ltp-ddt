#!/bin/sh

# Get erase size of one mtd partition
# Input: Partition Number like 3
# Output: MTD_ERASE_SIZE

source "st_log.sh"
source "common.sh"

if [ $# -ne 1 ]; then
	echo "Error: Invalid Argument Count"
	echo "Syntax: $0 partition_number"
	exit 1
fi

PART=$1
MTD_PROC_ENTRY=`cat /proc/mtd | grep "mtd$PART"` || die "Could not retrieve info from mtd proc entry"
SIZE=`echo $MTD_PROC_ENTRY | cut -d ' ' -f3` || die "Could not get the mtd erase size"
temp=`echo $SIZE | tr "a-z" "A-Z"` || die "Could not convert mtd erase size from a-z to A-Z"
SIZE=`echo "ibase=16; ${temp}" | bc` || die "Mtd erase size could not be converted from Hex to Decimal"
echo $SIZE





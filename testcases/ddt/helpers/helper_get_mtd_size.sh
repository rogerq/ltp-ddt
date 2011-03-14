#!/bin/sh

# Get size of one mtd partition
# Input: BLK_DEV_NODE like /dev/mtdblock3
# Output: MTD_SIZE and MTD_ERASE_SIZE

source "st_log.sh"
source "common.sh"

if [ $# -ne 1 ]; then
	echo "Error: Invalid Argument Count"
	echo "Syntax: $0 blk_dev_node"
	exit 1
fi

BLK_DEV_NODE=$1

PART=`echo $BLK_DEV_NODE | awk '{print substr ($0, length($0))}'` || die "mtd partition number is not found"
#test_print_trc "partition number is: $PART"
MTD_PROC_ENTRY=`cat /proc/mtd | grep "mtd$PART"` || die "Could not retrieve info from mtd proc entry"
SIZE=`echo $MTD_PROC_ENTRY | cut -d ' ' -f2` || die "Could not get the mtd size"
temp=`echo $SIZE | tr "a-z" "A-Z"` || die "Could not convert mtd size from a-z to A-Z"
size=`echo "ibase=16; ${temp}" | bc` || die "Mtd size could not be converted from Hex to Decimal"
echo $size

#ERASE_SIZE=`echo $MTD_PROC_ENTRY | cut -d ' ' -f3`
#temp=`echo $ERASE_SIZE | tr "a-z" "A-Z"`
#erase_size=`echo "ibase=16; ${temp}" | bc`
#echo ERASE_SIZE=$erase_size


#export MTD_SIZE=$size
#export MTD_ERASE_SIZE=$erase_size





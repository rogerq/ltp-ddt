#!/bin/sh

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

PART=`echo $BLK_DEV_NODE | awk '{print substr ($0, length($0))}'` || die "mtd partition number is not found"

echo $PART


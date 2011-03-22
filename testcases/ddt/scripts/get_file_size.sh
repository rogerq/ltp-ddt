#!/bin/sh

# Get file size using ls
# Input: filename
# Output: file size

source "common.sh"

if [ $# -ne 1 ]; then
	echo "Error: Invalid Argument Count"
	echo "Syntax: $0 filename"
	exit 1
fi

FILE=$1
SIZE=`ls -l $FILE | awk '{print $5}'` || die "error getting size for $FILE"
echo $SIZE





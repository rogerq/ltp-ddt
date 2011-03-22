#!/bin/sh

# Get ceiling result of $1/$2
# Input: number and divisor
# Output: ceiling result of number/divisor 

source "common.sh"

if [ $# -ne 2 ]; then
	echo "Error: Invalid Argument Count"
	echo "Syntax: $0 <number> <divisor>"
	exit 1
fi

NUMBER=$1
DIVISOR=$2
CEILING_RESULT=$((($NUMBER+$DIVISOR-1)/$DIVISOR))
echo $CEILING_RESULT





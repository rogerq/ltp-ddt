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





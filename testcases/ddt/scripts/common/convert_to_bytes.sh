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

# Convert M/K ending data to bytes
# Input: Data like *M, *K, or *G
# Output: Data in bytes

source "common.sh"

if [ $# -ne 1 ]; then
	echo "Error: Invalid Argument Count"
	echo "Syntax: $0 <data in M/K/G>"
	exit 1
fi
IN_DATA=$1
case $IN_DATA in
	*K)
		OUT_DATA=$(( KB * `echo $IN_DATA | sed s/[a-zA-Z]//g` ))
	;;
	*M)
		OUT_DATA=$(( MB * `echo $IN_DATA | sed s/[a-zA-Z]//g` ))
	;;
	*G)
		OUT_DATA=$(( GB * `echo $IN_DATA | sed s/[a-zA-Z]//g` ))
	;;
	*)
		OUT_DATA=$IN_DATA
	;;
esac
echo $OUT_DATA





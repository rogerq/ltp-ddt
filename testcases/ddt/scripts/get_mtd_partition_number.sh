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

# Get mtd partition number either by default or by command line option based on device type
# Input: device_type;
#	 partition number(optional)
# Output: partition_number

source "st_log.sh"
source "common.sh"

############################### CLI Params ###################################
if [ $# -lt 1 ]; then
	echo "Error: Invalid Argument Count"
	echo "Syntax: $0 <device_type> [partition_number](optional)"
	exit 1
fi
DEVICE_TYPE=$1
if [ $# -ge 2 -a -n $2 ]; then
	PARTITION=$2
fi

############################# Functions #######################################
# Assume the type is always nand for nand device
# this function is to check if the device /dev/mtd$partition is nand based on /sys entry
# if this /sys/ entry doesn't exist, don't do any check. 

find_part_type() {
  MTD_DEV="mtdblock$1"
  if [ -e /sys/block/$MTD_DEV/device/type ]; then
    TYPE=`cat /sys/block/$MTD_DEV/device/type`
    if [ $TYPE == 'nand' ]; then
			PART_TYPE='nand'
		else
  		if [ -d /sys/class/mtd/mtd$PART/device/driver ]; then
    		ls /sys/class/mtd/mtd$PART/device/driver | grep 'flash' && PART_TYPE="nor" || PART_TYPE="spi"
    		#ls /sys/class/mtd/mtd$PART/device/driver | grep 'spi' && PART_TYPE="spi"
  		else
    		die "/sys/class/mtd/mtd$PART/device/driver doesn't exist"
  		fi
			
		fi
	else
		die "/sys/block/$MTD_DEV/device/type doesn't exist"
	fi
	echo $PART_TYPE
}

# search through Lo to Hi to find out which partition has the biggest size
find_part_with_biggest_size() {
  local LO=$1
  local HI=$2
  # search through Lo to Hi to find out which one has the biggest size
  local MAX=0
  local PART=$LO
  while [ $PART -le $HI ]; do
    SIZE=`get_mtd_size.sh $PART` || die "error getting mtd$PART size: $SIZE"
    if [ $SIZE -gt $MAX ]; then
      PART_MAX=$PART
      MAX=$SIZE
    fi
    PART=`expr $PART + 1`
  done
  echo $PART_MAX
}
  
# search all partitions and find the Nand partition with the biggest size
get_nand_part() {
  # Assume maximum number of mtd partition is less than 15
  local CNT=15
  local PART=0  # starting searching from partition 0.
  local PART_RANGE=''
  while [ $PART -le $CNT ]; do
    if [ -e /dev/mtd$PART ]; then
			TYPE=`find_part_type $PART`
      if [ $TYPE == "$DEVICE_TYPE" ]; then
        # find out part range; put them in string like ":1:2:3:4"
        PART_RANGE="$PART_RANGE:$PART"
      fi
    fi
    PART=`expr $PART + 1`
  done
  # by now, the part_range like ":1:2" should be found.
  LO=`echo $PART_RANGE | cut -d":" -f2`
  HI=`echo $PART_RANGE | rev| cut -d':' -f1 | rev`
  TEST_PART=`find_part_with_biggest_size $LO $HI`
	if [ -n "$TEST_PART" ]; then
  	echo $TEST_PART
	else
		test_print_wrg "WARNING!!!:Failed to get mtd partition number dynamically through sys entry, \
					so taking user defined partition number!"
	fi
}

PARTITION=`get_nand_part`

############################ USER-DEFINED Params ##############################
# Try to avoid defining values here, instead see if possible
# to determine the value dynamically
case $ARCH in
esac
case $DRIVER in
esac
case $SOC in
esac
case $MACHINE in
        # example to redefine the params based on platform
        am389x-evm)

                case $DEVICE_TYPE in
                        nand)
                                : ${PARTITION:='7'}
                                ;;
                        spi)
                                : ${PARTITION:='3'}
                                ;;
                esac

                ;;
        am387x-evm)

                case $DEVICE_TYPE in
                        nand)
                                : ${PARTITION:='9'}
                                ;;
                        spi)
                                : ${PARTITION:='4'}
                                ;;
                esac

                ;;
        am181x-evm)

                case $DEVICE_TYPE in
                        spi)
                                : ${PARTITION:='2'}
                                ;;
                esac

                ;;

esac

# If no ARCH/DRIVER/SOC/MACHINE specific value, take the value below
case $DEVICE_TYPE in
        nand)
                : ${PARTITION:='4'}
                ;;
        spi)
                : ${PARTITION:='4'}
                ;;
        nor)
                : ${PARTITION:='3'}
                ;;
        *)
                die "Wrong device_type input in $0 script"
                ;;
esac

################
echo "$PARTITION"

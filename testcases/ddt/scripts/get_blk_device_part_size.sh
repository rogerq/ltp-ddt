# !/bin/sh
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

# Get size of the specific partition
# Input: DEV_NODE	like /dev/mtdblock3, /dev/mmcblk0p1, /dev/sda1
#	DEVICE_TYPE	like 'nand', 'spi', 'mmc', 'usb'
# Output: DEVICE_PART_SIZE which is the size of the partition

source "common.sh"
source "blk_device_common.sh"

############################# Functions #######################################
usage()
{
cat <<-EOF >&2
        usage: ./${0##*/} [-n DEVICE_NODE] [-d DEVICE TYPE]
        -n DEV_NODE  	block device node like /dev/mtdblock4, /dev/sda1
        -d DEVICE_TYPE  device type like 'mtd', 'mmc', 'usb' etc
        -h Help         print this usage
EOF
exit 0
}

if [ $# -lt 1 ]; then
        echo "Error: Invalid Argument Count"
	usage
        exit 1
fi

while getopts  :n:d:h arg
do case $arg in
        n)      DEV_NODE="$OPTARG";;
        d)      DEVICE_TYPE="$OPTARG";;
        h)      usage;;
        :)      echo "$0: Must supply an argument to -$OPTARG." >&2
                exit 1
                ;;

        \?)     echo "Invalid Option -$OPTARG ignored." >&2
                usage
                exit 1
                ;;
esac
done

: ${DEV_NODE:='/dev/mtdblock3'}
: ${DEVICE_TYPE:='nand'}

SIZE=0
DEV_TYPE=`get_device_type_map.sh $DEVICE_TYPE` || die "error while translating device type"
case $DEV_TYPE in
	mtd)
		PART=`get_mtd_partnum_from_devnode.sh "$DEV_NODE"` || die "error getting partition number: $PART"
		SIZE=`get_mtd_size.sh "$PART"` || die "error while getting the mtd size: $SIZE"

		# different way to get size; but could not figure out if it is symlink	
		#for file in $(find /sys/class/mtd/mtd$PART -type f -name size); do
		#	test_print_trc "file is:$file"
		#	size=`cat $file`
		#done
	;;
	mmc)
		#SIZE=$((1*GB))
    SIZE=`get_part_size_of_devnode "$DEV_NODE"` || die "error getting partition size for mmc $DEV_NODE: $SIZE"
	;;
	usb)
		SIZE=$((1*GB))
	;;
	ata)
		SIZE=$((1*GB))
	;;
	sata)
		SIZE=$((1*GB))
	;;
	*)
		die "Device type is not found; Size for Device Partition is set to default value: $SIZE"
	;;
esac

# return size in MB
echo $(( $SIZE / $MB ))



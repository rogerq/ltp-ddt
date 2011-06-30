#! /bin/sh
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

# Perform umount etc on blk device like mtd, mmc, mount point 
# Input  

source "common.sh"
source "mtd_common.sh"

############################# Functions #######################################
usage()
{
cat <<-EOF >&2
        usage: ./${0##*/} [-n DEV_NODE] [-d DEVICE_TYPE]
	-n DEV_NODE	optional param; device node like /dev/mtdblock2; /dev/sda1
	-d DEVICE_TYPE  device type like 'nand', 'mmc', 'usb' etc
        -h Help         print this usage
EOF
exit 0
}

############################### CLI Params ###################################

while getopts  :n:d:h arg
do case $arg in
        n)      
		# optional param
		DEV_NODE="$OPTARG";;
        d)      DEVICE_TYPE="$OPTARG";;
        h)      usage;;
        :)      test_print_trc "$0: Must supply an argument to -$OPTARG." >&2
                exit 1
                ;;

        \?)     test_print_trc "Invalid Option -$OPTARG ignored." >&2
                usage
                exit 1
                ;;
esac
done

############################ DEFAULT Params #######################
if [ -z $DEV_NODE ]; then
	DEV_NODE=`get_blk_device_node.sh "$DEVICE_TYPE"` || die "error getting device node for $DEVICE_TYPE: $DEV_NODE"
fi
test_print_trc "DEV_NODE: $DEV_NODE"

############# Do the work ###########################################
test_print_trc "Umounting device"
do_cmd "umount $DEV_NODE"




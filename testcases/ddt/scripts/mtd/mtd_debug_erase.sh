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

# Perform mtd_debug erase 
# Input $CHAR_DEV_NODE	like '/dev/mtd3'

source "common.sh"
source "st_log.sh"
source "mtd_common.sh"

############################# Functions #######################################
usage()
{
cat <<-EOF >&2
        usage: ./${0##*/} [-n PART] [-d DEVICE_TYPE] [-l LEN] [-o OFFSET] [-h USAGE]
        -n DEV_NODE     optional param; partition number 
        -d DEVICE_TYPE  device type like 'nand', 'mmc', 'usb' etc
	-o OFFSET	offset for mtd_debug erase command 
	-l LEN		length for mtd_debug erase command 
        -h Help         print this usage
EOF
exit 0
}


############################### CLI Params ###################################

while getopts  :d:o:l:n:h arg
do case $arg in
        n)      
		# optional param
		PART="$OPTARG";;
        o)      OFFSET="$OPTARG";;
        l)      LEN="$OPTARG";;
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
if [ -z $PART ]; then
	PART=`get_mtd_partition_number.sh "$DEVICE_TYPE"` || die "error getting mtd partition number"
fi
MTD_CHAR_DEV_NODE="$MTD_CHAR_DEV$PART"
echo "MTD_CHAR_DEV_NODE: $MTD_CHAR_DEV$PART"

# get erase length 
ERASE_SIZE=`get_mtd_erase_size.sh $PART` || die "error getting whole mtd erase size for partition $PART"

: ${OFFSET:='0'}
: ${LEN:="$ERASE_SIZE"}

############# Do the work
do_cmd mtd_debug erase $MTD_CHAR_DEV_NODE $OFFSET $LEN


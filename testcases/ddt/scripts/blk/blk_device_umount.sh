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
  usage: ./${0##*/} [-m MNT_POINT]
	-m MNT_POINT  
  -h Help         print this usage
EOF
exit 0
}

############################### CLI Params ###################################

while getopts  :m:d:f:h arg
do case $arg in
        m)      
		            MNT_POINT="$OPTARG";;
        d)      DEVICE_TYPE="$OPTARG";;
        f)      FS_TYPE="$OPTARG";;
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
############# Do the work ###########################################
# TODO: don't hardcode ubi node and volume name
test_print_trc "Umounting device"
test_print_trc "MNT_POINT: $MNT_POINT"

do_cmd "umount $MNT_POINT"
do_cmd "rm -rf $MNT_POINT"





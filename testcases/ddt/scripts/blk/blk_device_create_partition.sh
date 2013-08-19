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

# @desc This script is used to create partition(s); remove the partition if it exists.
#	Only support to create one or two partitions
#	This script doesn't apply for MTD device
# @params n) sample parameter
#         
# @returns None
# @history 2011-03-24: First version

source "common.sh"
source "st_log.sh"

############################# Functions #######################################
usage()
{
cat <<-EOF >&2
        usage: ./${0##*/} [-n DEV_NODE] [-d DEVICE TYPE] [-p NUM_PARTS]
        -p NUM_PARTS	number of partitions need to be created	
        -n DEV_NODE 	device_node like /dev/sda1; /dev/mmcblk0p1 
	-d DEVICE_TYPE	device type like 'mmc', 'usb'
        -h Help         print this usage
EOF
exit 0
}

get_end_sector(){
	DEVNODE_ENTRY=$1
        echo -e "c\nu\np\nq\n" | fdisk $DEVNODE_ENTRY | grep "heads" | awk '{print $5}' | awk '{print $1}'
}

############################ CLI Params ###########################################
while getopts :d:n:p: arg
do case $arg in
        n)
                DEV_NODE=$OPTARG ;;
        p)
                NUM_PARTS=$OPTARG ;;
        d)
                DEVICE_TYPE=$OPTARG ;;
        \?)
		echo "Invalid Option -$OPTARG ignored." >&2
                usage
                exit 1
                ;;
esac
done

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
esac

############################ DEFAULT Params #######################
: {NUM_PARTS:='1'}
FDISK="fdisk"

########################## Do the work #########################################
DEVNODE_ENTRY=`get_devnode_entry.sh "$DEV_NODE" "$DEVICE_TYPE"` || die "error getting devnode entry for $DEV_NODE"
test_print_trc "Umount $DEV_NODE or $DEVNODE_ENTRY if it is mounted"
do_cmd "mount" | grep $DEV_NODE && do_cmd "umount $DEV_NODE"
do_cmd "mount" | grep $DEVNODE_ENTRY && do_cmd "umount $DEVNODE_ENTRY"
sleep 2

CYLINDER_END=`get_end_sector $DEVNODE_ENTRY`
echo "end: $CYLINDER_END"
CYLINDER_MIDDLE=$(($CYLINDER_END/2))
echo "mid: $CYLINDER_MIDDLE"
CYLINDER2_START=$(($CYLINDER_MIDDLE+1))
echo "start2: $CYLINDER2_START"

if [ "$NUM_PARTS" == '1' ]; then
	FDISK_DEL_OPTS="p\np\nd\nw\n"
	FDISK_CREATE_OPTS="p\np\nn\np\n1\n\n\nw\n"
elif [ "$NUM_PARTS" == '2' ]; then
	FDISK_DEL_OPTS="p\np\nd\n1\nd\nw\n"
	FDISK_CREATE_OPTS="p\np\nn\np\n1\n\n${CYLINDER_MIDDLE}\nn\np\n2\n${CYLINDER2_START}\n\nw\n"
fi
test_print_trc "Delete partition(s) on $DEVNODE_ENTRY"
echo -e $FDISK_DEL_OPTS | $FDISK $DEVNODE_ENTRY
sleep 3
test_print_trc "Create partition(s) on $DEVNODE_ENTRY"
echo -e $FDISK_CREATE_OPTS | $FDISK $DEVNODE_ENTRY

# End Of File

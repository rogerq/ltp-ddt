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
# @desc Helper for file system read/write performance 
#	This script does: mount->write->umount->mount->read for different buffer size.
# @params d) device type: nand, nor, spi 
#         f) Filesystem type (i.e. jffs2)
# @history 2011-03-05: First version

source "st_log.sh"
source "common.sh"

############################# Functions #######################################
usage()
{
cat <<-EOF >&2
	usage: ./${0##*/}  [-i IOCTL] [-a ACNTS] [-b BCNTS] [-c CCNTS] [-n NUMTC] 
	-i IOCTL	IOCTL like edma_async, edma_absync, edma_link_async, edma_link_absync, edma_chain_async, edma_chain_absync etc
	-c CNTS		optional param, acnt,bcnt and ccnt value like "0 0 0" 1 0 1" "4 4 64","16 16 256" etc
	-n NUMTC	optional param, number of transfer controllers like 2
	-h Help 	print this usage
EOF
exit 0
}



############################### CLI Params ###################################
if [ $# == 0 ]; then
	echo "Please provide options; see usage"
	usage
	exit 1
fi

while getopts  :i:a:b:c:n:h arg
do case $arg in
	i)	IOCTL="$OPTARG";;
	c)	CNTS="$OPTARG";;
	n)	NUMTC="$OPTARG";;
	h)	usage;;
	:)	test_print_trc "$0: Must supply an argument to -$OPTARG." >&2
		exit 1 
		;;

	\?)	test_print_trc "Invalid Option -$OPTARG ignored." >&2
		usage 
		exit 1
		;;
esac
done

########################### DYNAMICALLY-DEFINED Params ########################
: ${CNT1:='0 1 1'}
: ${CNT2:='0 0 0'}
: ${CNT3:='1 1 1'}
: ${CNT4:='256 1 1'}
: ${CNT5:='1024 1 1'}
: ${CNT6:='10240 1 1'}
: ${CNT7:='5 1 10240'}
: ${CNT8:='1024 2 10'}
: ${NUMTC:='2'}

########################### REUSABLE TEST LOGIC ###############################
# DO NOT HARDCODE any value. If you need to use a specific value for your setup
# use user-defined Params section above.
test_print_trc "STARTING  EDMA Test for $IOCTL"
test_print_trc "IOCTL_TYPE:${IOCTL}"
test_print_trc "NUMTC:${NUMTC}"
test_print_trc "Inserting timer module. Please wait..."
do_cmd lsmod | grep kSt_timer
if [ $? = 1 ]; then
	do_cmd insmod ddt/kSt_timer.ko
        sleep 5
else
	do_cmd echo Module already inserted
fi
test_print_trc "Inserting edma test module. Please wait..."
do_cmd lsmod | grep edma_test
if [ $? = 1 ]; then
	do_cmd insmod ddt/edma_test.ko
        sleep 5
else
	do_cmd echo Module already inserted
fi
if [ -z "$CNTS" ]; then
	for CNT in "$CNT1" "$CNT2" "$CNT3" "$CNT4" "$CNT5" "$CNT6" "$CNT7" "$CNT8" ; do
		set -- $CNT
		test_print_trc "ACNTS:$1"
		test_print_trc "BCNTS:$2"
		test_print_trc "CCNTS:$3"

		# run edma test
		do_cmd edma_tests -device /dev/edma -acnt $1 -bcnt $2 -ccnt $3 -numtcs $NUMTC -ioctl $IOCTL 
	done
else
	for CNT in "$CNTS"; do
		set -- $CNT
		test_print_trc "ACNTS:$1"
		test_print_trc "BCNTS:$2"
		test_print_trc "CCNTS:$3"
		# run edma test
		do_cmd edma_tests -device /dev/edma -acnt $1 -bcnt $2 -ccnt $3 -numtcs $NUMTC -ioctl $IOCTL 
	done
fi
test_print_trc "Removing edma test module. Please wait..."
do_cmd rmmod edma_test.ko
sleep 5
test_print_trc "Removing timer module. Please wait..."
do_cmd rmmod kSt_timer.ko
sleep 5



#! /bin/sh
###############################################################################
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
###############################################################################

# @desc Briefly describe what the script does.
# @params s) sample parameter
#         a) another sample parameter
# @returns Describes what the script returns if it does return a value
# @history 2011-03-05: First version

source "common.sh"  # Import do_cmd(), die() and other functions

############################# Functions #######################################
usage()
{
	cat <<-EOF >&2
	usage: ./${0##*/} -t TEST_TYPE
	-t TEST_TYPE	Test Type. Possible Values are:\
	control,out,iso-out,in,iso-in,out-perf,in-perf,iso-out-perf,\
	iso-in-perf,halt,unlink
	EOF
	exit 1
}

do_test ()
{
    do_cmd testusb -a -s $BUFLEN -c $COUNT $* 1>/tmp/testusb_$$.log
    T=`grep secs /tmp/testusb_$$.log | awk '{ print $4 }'`
    cat /tmp/testusb_$$.log
    test_print_trc "Test took $T secs"
    rm /tmp/testusb_$$.log
}


############################ Script Variables ##################################
# Default values
TYPE="invalid"
COUNT=1000
BUFLEN=2048

################################ CLI Params ####################################
while getopts  :t:h arg
do case $arg in
        t)      TYPE="$OPTARG";;
        h)      usage;;
        :)      die "$0: Must supply an argument to -$OPTARG.";;
        \?)     die "Invalid Option -$OPTARG ";;
esac
done

########################### REUSABLE TEST LOGIC ###############################
test_print_trc "TestName:: USB I/O Stress"

test_print_trc " ****************** TEST PARAMETERS ******************"
test_print_trc " TYPE		: $TYPE"

test_print_trc " *************** END OF TEST PARAMETERS ***************"

case "$TYPE" in
	out)
		test_print_trc "** Host Write (OUT) test cases:"
		test_print_trc "test 1: $COUNT OUT transfers, same size"
		do_test -t 1

		test_print_trc "test 3: $COUNT transfers, variable/short size"
		do_test -t 3 -v 421

		COUNT=100
		test_print_trc "test 17: $COUNT transfers, unaligned DMA map by core"
		do_test -t 17

		test_print_trc "test 19: $COUNT transfers, unaligned DMA map by usb_alloc_coherent"
		do_test -t 19

		COUNT=2000
		test_print_trc "test 5: $COUNT scatterlists, same size entries"
		do_test -t 5

		# try to trigger short OUT processing bugs
		test_print_trc "test 7a: $COUNT scatterlists, variable size/short entries"
		do_test -t 7 -v 579
		BUFLEN=4096
		test_print_trc "test 7b: $COUNT scatterlists, variable size/bigger entries"
		do_test -t 7 -v 41
		BUFLEN=64
		test_print_trc "test 7c: $COUNT scatterlists, variable size/micro entries"
		do_test -t 7 -v 63
		;;

	iso-out)
	    test_print_trc '** Host ISOCHRONOUS Write (OUT) test cases:'

	    # at peak iso transfer rates:
	    # - usb 2.0 high bandwidth, this is one frame.
	    # - usb 1.1, it's twenty-four frames.
	    BUFLEN=24500

	    COUNT=1000

# COUNT=10000

	    test_print_trc "test 15: $COUNT transfers, same size"
	    # do_test -t 15 -g 3 -v 0
	    BUFLEN=32768
	    do_test -t 15 -g 8 -v 0

	    # FIXME it'd make sense to have an iso OUT test issuing
	    # short writes on more packets than the last one

	    COUNT=100
	    test_print_trc "test 22: $COUNT transfers, non aligned"
	    do_test -t 22 -g 8 -v 0

	    ;;

	in)
	    test_print_trc '** Host Read (IN) test cases:'

	    # NOTE:  these "variable size" reads are just multiples
	    # of 512 bytes, no EOVERFLOW testing is done yet

	    test_print_trc "test 2: $COUNT transfers, same size"
	    do_test -t 2
	    test_print_trc "test 4: $COUNT transfers, variable size"
	    do_test -t 4

	    COUNT=100
	    test_print_trc "test 18: $COUNT transfers, unaligned DMA map by core"
	    do_test -t 18

	    test_print_trc "test 20: $COUNT transfers, unaligned DMA map by usb_alloc_coherent"
	    do_test -t 20

	    COUNT=2000
	    test_print_trc "test 6: $COUNT scatterlists, same size entries"
	    do_test -t 6
	    test_print_trc "test 8: $COUNT scatterlists, variable size entries"
	    do_test -t 8
	    ;;

	iso-in)
	    test_print_trc '** Host ISOCHRONOUS Read (IN) test cases:'

	    # at peak iso transfer rates:
	    # - usb 2.0 high bandwidth, this is one frame.
	    # - usb 1.1, it's twenty-four frames.
	    BUFLEN=24500

	    COUNT=1000

# COUNT=10000

	    test_print_trc "test 16: $COUNT transfers, same size"
	    # do_test -t 16 -g 3 -v 0
	    BUFLEN=32768
	    do_test -t 16 -g 8 -v 0

	    # FIXME since iso expects faults, it'd make sense
	    # to have an iso IN test issuing short reads ...

	    COUNT=100
	    test_print_trc "test 23: $COUNT transfers, unaligned"
	    do_test -t 23 -g 8 -v 0
	    ;;

	halt)
	    # NOTE:  sometimes hardware doesn't cooperate well with halting
	    # endpoints from the host side.  so long as mass-storage class
	    # firmware can halt them from the device, don't worry much if
	    # you can't make this test work on your device.
	    COUNT=1000
	    test_print_trc "test 13: $COUNT halt set/clear"
	    do_test -t 13
	    ;;

	unlink)
	    COUNT=2000
	    test_print_trc "test 11: $COUNT read unlinks"
	    do_test -t 11

	    test_print_trc "test 12: $COUNT write unlinks"
	    do_test -t 12
	    ;;

	out-perf)
	    test_print_trc "** Host Write (OUT) performance test case:"

	    # 100MB transfer
	    COUNT=100
	    BUFLEN=131072
	    SGLEN=8
	    do_test -t 5 -g $SGLEN
	    B1=`echo "scale=3; 100/$T" | bc`
	    test_print_trc "|PERFDATA|100MB|write(OUT)|throughput:"`printf '%.2f' $B1`"MB/s|"
	    ;;

	in-perf)
	    test_print_trc "** Host Read (IN) performance test case:"

	    # 100MB transfer
	    COUNT=100
	    BUFLEN=131072
	    SGLEN=8
	    do_test -t 6 -g $SGLEN
	    B1=`echo "scale=3; 100/$T" | bc`
	    test_print_trc "|PERFDATA|100MB|read(IN)|throughput:"`printf '%.2f' $B1`"MB/s|"
	    ;;

        iso-out-perf)
            test_print_trc "** Host Write (ISO OUT) performance test case:"

            # 100MB transfer
	    COUNT=100
	    BUFLEN=131072
	    SGLEN=8
            do_test -t 15 -g $SGLEN
            B1=`echo "scale=3; 100/$T" | bc`
            test_print_trc "|PERFDATA|100MB|write(ISO OUT)|throughput:"`printf '%.2f' $B1`"MB/s|"
            ;;

        iso-in-perf)
            test_print_trc "** Host Write (ISO IN) performance test case:"

            # 100MB transfer
	    COUNT=100
	    BUFLEN=131072
	    SGLEN=8
            do_test -t 16 -g $SGLEN
            B1=`echo "scale=3; 100/$T" | bc`
            test_print_trc "|PERFDATA|100MB|write(ISO IN)|throughput:"`printf '%.2f' $B1`"MB/s|"
            ;;

	*)
	    test_print_trc "Don't understand test type $TYPE"
	    usage
	    ;;

esac


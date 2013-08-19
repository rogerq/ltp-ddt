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

# This script is to use i2cset and i2cget to i2c read write
# Input  

source "common.sh"

############################# Functions #######################################
usage()
{
cat <<-EOF >&2
        usage: ./${0##*/} [-d SLAVE_DEVICE] [-l TEST_LOOP} 
        -d SLAVE_DEVICE  slave device name; it is optional; if not provided, the slave address will take the default one from 'get_i2c_slave_addr.sh' 
	-l TEST_LOOP	test loop for r/w. default is 1.
        -h Help         print this usage
EOF
exit 0
}

############################### CLI Params ###################################
while getopts  :d:l:h arg
do case $arg in
        d)      
		SLAVE_DEVICE="$OPTARG";;
	l) 	
		TEST_LOOP="$OPTARG";;
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
: ${TEST_LOOP:='1'}
test_print_trc "SLAVE_DEVICE: $SLAVE_DEVICE"
test_print_trc "if SLAVE_DEVICE is empty, slave device will use the one defined in get_i2c_slave_addr.sh"

############# Do the work ###########################################
test_print_trc "Running i2c setget test for $TEST_LOOP times"
x=0
while [ $x -lt $TEST_LOOP ]
do

	SLAVE_ADDRESS=`get_i2c_slave_addr.sh "$SLAVE_DEVICE"` || die "error getting slave address for i2c: $SLAVE_ADDRESS"
	REGOFFSET=`get_i2c_slave_regoffset.sh "$SLAVE_DEVICE"` || die "error getting slave regoffset" 
	REGVALUE=`get_i2c_slave_regvalue.sh "$SLAVE_DEVICE"` || die "error getting slave regvalue" 
	I2CBUS=`get_i2cbus_number.sh` || die "error getting i2cbus number: $I2CBUS" 
	do_cmd i2cset -y "$I2CBUS" "$SLAVE_ADDRESS" "$REGOFFSET" "$REGVALUE" 
	do_cmd "i2cget -y "$I2CBUS" "$SLAVE_ADDRESS" "$REGOFFSET" | grep "$REGVALUE""

	x=$((x+1))
done


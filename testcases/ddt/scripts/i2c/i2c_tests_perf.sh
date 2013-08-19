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

# This script is to use i2c_tests program to test performance; 
# For now, it only support eeprom
# Input  

source "common.sh"

############################# Functions #######################################
usage()
{
cat <<-EOF >&2
        usage: ./${0##*/} [-d SLAVE_DEVICE] [-B BUFFER_SIZES] [-p PAGE_SIZEr] [-s Total_Size] 
        -d SLAVE_DEVICE  slave device name; it is optional; if not provided, the slave address will take the default one from 'get_i2c_slave_addr.sh' 
	-B BUFFER_SIZES buffer size for i2c perf test. Ex. "4 8 16 32 64"
	-s TOTAL_SIZE	the size to be write and read
	-p PAGE_SIZE	page size
        -h Help         print this usage
EOF
exit 0
}

############################### CLI Params ###################################
while getopts  :d:B:s:p:h arg
do case $arg in
        d)      
		SLAVE_DEVICE="$OPTARG";;
        B)      
		BUFFER_SIZES="$OPTARG";;
        s)      
		TOTAL_SIZE="$OPTARG";;
        p)      
		PAGE_SIZE="$OPTARG";;
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

############# Do the work ###########################################
test_print_trc "Running i2c perf test for $TEST_LOOP times"
SLAVE_ADDRESS=`get_i2c_slave_addr.sh "$SLAVE_DEVICE"` || die "error getting slave address for i2c"
I2C_DEVNODE=`get_i2c_devnode.sh` || die "error getting i2c devnode";

for BUFF_SIZE in $BUFFER_SIZES; do
	do_cmd "i2c_tests -device "$I2C_DEVNODE" -address "$SLAVE_ADDRESS" -buffer_size "$BUFF_SIZE" -total_size $TOTAL_SIZE -write -page_size ${PAGE_SIZE} -performance "
	do_cmd "i2c_tests -device "$I2C_DEVNODE" -address "$SLAVE_ADDRESS" -buffer_size "$BUFF_SIZE" -total_size $TOTAL_SIZE -read -page_size ${PAGE_SIZE} -performance "
done


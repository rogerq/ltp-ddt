#!/bin/sh 
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

# @desc This script is used to run nand ecc tests
# @params 
#   * dev_node: optional; if not provided, choose one with the biggest size
#   * modify_rules: the rules used to inject error bits
#       * format: "<sector#>:byte_offset:mask"~"<oob>:offset:mask"
#       * sector, byte_offset are decimal. 'mask' could be decimal or hex
#       * byte_offset is the offset relative to each sector and it starts from 0
# Example:
#   nandecc_tests.sh "/dev/mtd6" "0:0:0xFE"~"1:0:0xF0"~"OOB:3:0x01"
# @returns 
# @history 2013-09-26: First version

source "common.sh"  
source "mtd_common.sh"

############################# Functions #######################################
usage()
{
cat <<-EOF >&2
    usage: ./${0##*/} [-n dev_node] [-r rules] 
    -n dev_node: optional; if not provided, choose one with the biggest size
    -r rules: the rules used to inject error bits
    -h Help         print this usage
EOF
exit 0
}

# Modify the nanddump file to flip bits based on $rules
# Input
#   $1: input file
#   $2: rules like "<sector#>:byte_offset:mask"~"<oob>:byte_offset:mask"
#       sector, byte_offset are decimal. 'mask' could be decimal or hex
#       byte_offset starts from 0
# Output
#   modified file
modify_nanddump_file()
{
  inputfile=$1
  outputfile=$2
  rules=$3

  local filesize=`get_filesize $1`
  cp $inputfile $outputfile

  ORIG_IFS=$IFS
  IFS="~"
  for rule in $rules
  do
    # rule is like "sector#:byte_offset:mask" or "oob:offset:mask"
    sector=`echo "$rule" | cut -d":" -f1 | tr [:upper:] [:lower:]`
    byte_offset=`echo "$rule" | cut -d":" -f2`
    mask=`echo "$rule" | cut -d":" -f3`
    if [ $sector = 'oob' ]; then
      offset=$(( $pagesize + $byte_offset ))
      if [ $offset -ge $filesize ]; then
        die "Offset to $inputfile should not be bigger than the filesize"
      fi
    else
      offset=$(( $sector * $sector_size + $byte_offset ))
      if [ $offset -ge $pagesize ];then
        die "Offset to $inputfile for non-oob data should not bigger than pagesize"
      fi
    fi

    # get the original byte and modify it
    orig_byte=`hexdump_onebyte $inputfile $offset`
    mod_byte_dec=$(( 0x$orig_byte ^ $mask ))
    mod_byte=`echo "obase=16; $mod_byte_dec" |bc`

    # write the modified byte to replace the original one
    replace_onebyte $outputfile $offset $mod_byte
    
  done
  IFS=$ORIG_IFS

}

################################ CLI Params ####################################
while getopts  :n:r:h arg
do case $arg in
    n)      dev_node="$OPTARG";;
    r)      rules="$OPTARG";;
    h)      usage;;
    :)      die "$0: Must supply an argument to -$OPTARG.";;
    \?)     die "Invalid Option -$OPTARG ";;
esac
done

############################ USER-DEFINED Params ###############################
# Try to avoid defining values here, instead see if possible
# to determine the value dynamically. ARCH, DRIVER, SOC and MACHINE are
# initilized and exported by runltp script based on platform option (-P)

# Define default values for variables being overriden
case $ARCH in
esac
case $DRIVER in
esac
case $SOC in
esac
case $MACHINE in
esac

########################### DYNAMICALLY-DEFINED Params #########################
if [ -z "$dev_node" ]; then
  part=`get_mtd_partition_number.sh "nand" `
  dev_node="/dev/mtd$part"
fi

pagesize=`get_pagesize $dev_node`
# TODO: After nand subpage support is added to kernel, remove the hard-coded code
sector_size=`get_subpagesize $dev_node`
sector_size=512
test_print_trc "pagesize: $pagesize"
test_print_trc "sector_size: $sector_size"

########################### TEST LOGIC ###############################
# create a random test file
testfile="testfile.bin"
do_cmd dd if=/dev/urandom of="$testfile" bs=1 count=$pagesize

# write to nand
test_print_trc "Write testfile to nand..."
do_cmd flash_erase -q "$dev_node" 0 0
do_cmd nandwrite -p "$dev_node" "$testfile"
do_cmd nanddump -p -l "$pagesize" "$dev_node"
orig_nanddump="$TMPDIR/testfile_nanddump.original"
orig_nanddump_w_oob="$TMPDIR/testfile_nanddump_w_oob.original"
do_cmd nanddump -l "$pagesize" -f "${orig_nanddump}" "$dev_node"
do_cmd nanddump -l "$pagesize" -o -f "$orig_nanddump_w_oob" "$dev_node"
 
# modify the nanddump file
test_print_trc "Modifying nand dump file to prepare for ecc test"
corrupted_nanddump="$TMPDIR/testfile_nanddump.corrupted"
modify_nanddump_file "$orig_nanddump_w_oob" "$corrupted_nanddump" "$rules"

# write the corrupted nand file back
test_print_trc "Write the corrupted nanddump file back..."
do_cmd flash_erase -q "$dev_node" 0 0
do_cmd nandwrite -n -o "$dev_node" "$corrupted_nanddump"

# Display the errors being injected
hexdump_original_w_oob="$TMPDIR/original_w_oob"
hexdump_corrupted_w_oob="$TMPDIR/corrupted_w_oob"
do_cmd "hexdump -C $orig_nanddump_w_oob > "$hexdump_original_w_oob" "
do_cmd "hexdump -C $corrupted_nanddump > "$hexdump_corrupted_w_oob" "
test_print_trc "The below are the errors in the nand page" 
test_print_trc "diff "$hexdump_original_w_oob" "$hexdump_corrupted_w_oob" "
diff "$hexdump_original_w_oob" "$hexdump_corrupted_w_oob"

sleep 5

# Verify steps
corrected_nanddump="$TMPDIR/testfile_nanddump.corrected"
do_cmd "nanddump -l "$pagesize" -f "$corrected_nanddump" "$dev_node" " 

test_print_trc "Check if the errors are got corrected"
hexdump_original="$TMPDIR/original"
hexdump_corrected="$TMPDIR/corrected"
do_cmd "hexdump -C $orig_nanddump > "$hexdump_original" "
do_cmd "hexdump -C $corrected_nanddump > "$hexdump_corrected" "
test_print_trc "diff "$hexdump_original" "$hexdump_corrected" "
diff "$hexdump_original" "$hexdump_corrected"
if [ $? -ne 0 ]; then
  die "Nand ECC Test failed. Not all errors are corrected"
else
  test_print_trc "Nand ECC Test Pass"
  exit 0
fi





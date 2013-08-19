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

# @desc Script to validate clock settings based on the Common Clk Framework.
# It compares the values clk_rate values found in the clk debugfs folder with
# the golden reference of each soc and the clocks values calculated based on
# the register settings. 
# @returns 0 if successful, not 0 otherwise
# @history 2013-08-13: First version

source "common.sh"  # Import do_cmd(), die() and other functions

############################# Functions #######################################
usage()
{
	echo "./clk_framework_test.sh"
	exit 1
}

############################ Script Variables ##################################
# Define default valus if possible

################################ CLI Params ####################################
# Please use getopts

############################ USER-DEFINED Params ###############################
# Try to avoid defining values here, instead see if possible
# to determine the value dynamically. ARCH, DRIVER, SOC and MACHINE are 
# initilized and exported by runltp script based on platform option (-P)

# Define default values for variables being overriden
CLK_FRAMEWORK_ROOT="/sys/kernel/debug/clk"

########################### DYNAMICALLY-DEFINED Params #########################
# Try to use /sys and /proc information to determine values dynamically.
# Alternatively you should check if there is an existing script to get the
# value you want


########################### REUSABLE TEST LOGIC ###############################
# DO NOT HARDCODE any value. If you need to use a specific value for your setup
# use USER-DEFINED Params section above.

# Uncomment the following line if you want the change the behavior of
# do_cmd to treat non-zero values as pass and zero as fail.
# inverted_return="true"

# Avoid using echo. Instead use print functions provided by st_log.sh
#test_print_trc "Sample trace log"

# Use do_cmd() (imported from common.sh) to execute your test steps.
# do_cmd() will check return code and fail the test is return code is non-zero.
#do_cmd echo "hello world!"

eval ref_tree=( `${SOC}_clk_framework_ref.sh` )
eval reg_tree=( `${SOC}_clk_framework_registers.sh` )

debugfs_tree=( `find $CLK_FRAMEWORK_ROOT -type d` )
debugfs_tree=( ${debugfs_tree[@]:1} )
debugfs_num=$((${#debugfs_tree[@]}-1))
ref_num=${#ref_tree[@]}
clk_fw_rc=0
if [ $ref_num -ne $debugfs_num ]
then
  test_print_trc "Number of clk tree nodes in reference $ref_num \
  and clk tree nodes in debugfs $debugfs_num do not match"
  clk_fw_rc=1
fi

for i in "${!ref_tree[@]}"
do
  ref_node_info=( $(echo ${ref_tree[$i]} | tr '\=' " ") )
  reg_node_info=( $(echo ${reg_tree[$i]} | tr '\=' " ") )
  debugfs_rate=$(cat $CLK_FRAMEWORK_ROOT/${ref_node_info[0]}/clk_rate)
  if [ "${ref_node_info[1]}" != "$debugfs_rate" ]
  then
    test_print_trc "Debugfs value $debugfs_rate and reference value \
    ${ref_node_info[1]} for node ${ref_node_info[0]} do not match"
    clk_fw_rc=1
  fi
  if [ "${reg_node_info[1]}" != "$debugfs_rate" ]
  then
    test_print_trc "Debugfs value $debugfs_rate and registers based value \
    ${reg_node_info[1]} for node ${ref_node_info[0]} do not match"
    clk_fw_rc=1
  fi
  if [ "${reg_node_info[1]}" != "${ref_node_info[1]}" ]
  then
    test_print_trc "Reference value "${ref_node_info[1]}" and registers based \
    value ${reg_node_info[1]} for node ${ref_node_info[0]} do not match"
    clk_fw_rc=1
  fi
done
exit $clk_fw_rc

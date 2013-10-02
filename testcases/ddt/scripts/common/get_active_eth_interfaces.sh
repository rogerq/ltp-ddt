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
source "common.sh"  # Import do_cmd(), die() and other functions

############################# Functions #######################################

########################### REUSABLE TEST LOGIC ###############################
# Usage information: get_active_eth_interfaces.sh
# Parameters: none
# Returns an array of active (operational state of up) eth interface names
################################################################################
################################################################################
# Cleanup function for use before the test exits                               
################################################################################
set +x                                                                               
# check for all eth interfaces supported and create an array                    
j=0                                                                             
for device in `find /sys/class/net/*eth*`                                       
do                                                                              
  interface=`echo $device | cut -c16-`                                          
  if [[ "`cat /sys/class/net/$interface/operstate`" == "up" ]]
  then
    int_name[j]=$interface                                                        
    j+=1
  fi                                                                          
done                                                                            
echo "${int_name[@]}" 

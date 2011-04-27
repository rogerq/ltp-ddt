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

# @desc Take an application as a parameter and execute the application.  This
#       script will also check if the application is in the default PATH
#       unless the -n option is used
# @params a) application to execute
#         n) skip the path check
#         i) invert the return check for applications that return
#            non-zero on success
# @returns This script returns a non-zero value on failure
# @history 2011-04-27: First version (Chase Maupin)

source "common.sh"  # Import do_cmd(), die() and other functions

############################# Functions #######################################
usage()
{
    echo "Usage:"
    echo "  $0 -a <application> [-nih]"
    echo ""
    echo "Parameters:"
    echo "  -a: Application to execute"
    echo "  -n: Skip PATH check"
    echo "  -i: Invert the return check for non-zero pass values"
    echo "  -h: This message"
    echo ""
    echo "NOTE: When specifying the application parameter it is possible to"
    echo "      pass complex strings such as:"
    echo "          \"/usr/bin/dhrystone_static 30000000 1000000\""
    echo "      In cases such as this the command to check for is the first"
    echo "      part of the application string but the application executed"
    echo "      is the entire string with parameters"
	exit 1
}

############################ Script Variables ##################################
# List the variables used in the script and set their defaults here
no_path_check="false"

################################ CLI Params ####################################
# Please use getopts
while getopts  :a:nih arg
do case $arg in
        a)      app="$OPTARG";;
        n)      no_path_check="true";;
        i)      inverted_return="true";;
        h)      usage;;
        :)      die "$0: Must supply an argument to -$OPTARG.";; 
        \?)     die "Invalid Option -$OPTARG ";;
esac
done

########################### REUSABLE TEST LOGIC ###############################
check_in_path() {
  if [ "$no_path_check" == "false" ]
  then
    # Only evaluate the first part of the application string in case
    # parameters are passed to the application.
    which $1
    if [ $? -ne 0 ]
    then
      test_print_trc "Command ($app) was not found in the PATH"
      exit 1
    fi
  fi
}

# Use do_cmd() (imported from common.sh) to execute your test steps.
# do_cmd() will check return code and fail the test is return code is non-zero.

check_mandatory_inputs app

check_in_path $app

do_cmd $app

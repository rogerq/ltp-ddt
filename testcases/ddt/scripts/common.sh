# /bin/sh
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
source "st_log.sh"

########### DEFINE CONSTANTS ############
KB=1024
MB=1048576
GB=$((1024*1024*1024))


########### FUNCTIONS #####################
# Default value for inverted_return is "false" but can
# be overridden by individual scripts.
inverted_return="false"

do_cmd() {
	CMD=$*
	test_print_trc "Inside do_cmd:CMD=$CMD"
	eval $CMD
	RESULT=$?
    if [ "$inverted_return" == "false" ]
    then
	    if [ $RESULT -ne 0 ]
	    then
		    test_print_err "$CMD failed. Return code is $RESULT"
		    exit $RESULT
	    fi
    else
        if [ $RESULT -eq 0 ]
        then
		    test_print_err "$CMD failed. Return code is $RESULT"
		    exit $RESULT
        fi
    fi
}
#do_cmd "mount | grep mtdblock4 || echo notmounted"

die() {
	test_print_err "FATAL: $*"
	exit 1
}



# Extract one return value from name=value list file
# First argument is the file with name=value list
# Second argument is the name of the value to return
get_return_value() {
	if [ $# -ne 2 ]; then
		die "Wrong number of arguments. \
                     Usage: get_return_value <file> <value name>"
	fi
	tmp_ifs=$IFS
        IFS=$'\n'
        file=$1
        name=$2
        for line in $(cat "$file") ; do
                TMPNAME=`echo $line | cut -d"=" -f1 | sed 's/^ *//g' | sed 's/ *$//g'`
                [ "$TMPNAME" == "$name" ] && TMPVAL=`echo $line | cut -d"=" -f2 | \
                sed 's/^ *//g' | sed 's/ *$//g'` && echo $TMPVAL && IFS=$tmp_ifs && exit 0
        done
        IFS=$tmp_ifs
        die "Value $name not found in $file"
}


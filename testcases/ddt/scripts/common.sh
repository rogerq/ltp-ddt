# /bin/sh
source "st_log.sh"

########### DEFINE CONSTANTS ############
MB=1048576
GB=$((1024*1024*1024))


########### FUNCTIONS #####################
do_cmd() {
	CMD=$*
	test_print_trc "Inside do_cmd:CMD=$CMD"
	eval $CMD
	RESULT=$?
	if [ $RESULT -ne 0 ]
	then
		test_print_err "$CMD failed. Return code is $RESULT"
		exit $RESULT
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


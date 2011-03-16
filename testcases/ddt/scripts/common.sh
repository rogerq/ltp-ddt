# /bin/sh
#source "$LTPROOT/scripts/ddt/st_log.sh"
source "st_log.sh"
########### CONSTANTS DEFINCE ############
MB=1048576
GB=$((1024*1024*1024))


########### FUNCTIONS #####################
do_cmd() {
	CMD=$*
	test_print_trc "Inside do_cmd:CMD=$CMD"
	eval $CMD
	RESULT=$?
	#echo "return code: $RESULT"
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
get_return_value() {
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
        exit 1
}


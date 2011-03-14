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


format_disk() {
	DEVICE=$1
	test_print_trc "Formatting $DEVICE"
	
}

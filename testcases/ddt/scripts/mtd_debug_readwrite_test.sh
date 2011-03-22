#! /bin/sh

# Perform mtd_debug read 
# Input $CHAR_DEV_NODE	like '/dev/mtd3'

source "common.sh"
source "st_log.sh"
source "mtd_common.sh"

############################### CLI Params ###################################

while getopts  :d:o:l:n:h arg
do case $arg in
        n)      
		# optional param
		PART="$OPTARG";;
        o)      OFFSET="$OPTARG";;
        l)      LEN="$OPTARG";;
        d)      DEVICE_TYPE="$OPTARG";;
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
if [ -z $PART ]; then
	PART=`get_mtd_partition_number.sh "$DEVICE_TYPE"` || die "error getting mtd partition number"
fi
MTD_CHAR_DEV_NODE="$MTD_CHAR_DEV$PART"
echo "MTD_CHAR_DEV_NODE: $MTD_CHAR_DEV$PART"

: ${OFFSET:='0'}
: ${LEN:='erase_size'}
: ${DST_FILE:='read.txt'}
: ${SRC_FILE:='write.txt'}

# If all/erase, take mtd size or erase size; otherwise, take whatever passed in. 
case $LEN in
	mtd_size)
		LEN=`get_mtd_size.sh $PART ` || die "error getting mtd size"
		test_print_trc "LEN is set to MTD size which is $LEN"
	;;
	erase_size)
		LEN=`get_mtd_erase_size.sh $PART` || die "error getting whole mtd erase size for partition $PART"
		test_print_trc "LEN is set to Erase size which is $LEN"
	;;
esac


############# Do the work ###########################################
do_cmd mtd_debug erase "$MTD_CHAR_DEV_NODE" "$OFFSET" "$LEN"
do_cmd dd if=/dev/zero of="$SRC_FILE" bs="$LEN" count=1
do_cmd mtd_debug write "$MTD_CHAR_DEV_NODE" "$OFFSET" "$LEN" "$SRC_FILE"
do_cmd mtd_debug read "$MTD_CHAR_DEV_NODE" "$OFFSET" "$LEN" "$DST_FILE"
do_cmd cmp -l "$SRC_FILE" "$DST_FILE"
do_cmd rm -f "$SRC_FILE" "$DST_FILE"

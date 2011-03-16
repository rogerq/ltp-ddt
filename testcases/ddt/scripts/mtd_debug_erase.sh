#! /bin/sh

# Perform mtd_debug erase 
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
        l)      ERASE_SIZE="$OPTARG";;
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
: ${ERASE_SIZE:='all'}

# get erase size if erase size is all
if [ $ERASE_SIZE == "all" ]; then
	ERASE_SIZE=`get_mtd_erase_size.sh $PART` || die "error getting whole mtd erase size for partition $PART"
fi

############################ USER-DEFINED Params ##############################
# Try to avoid defining values here, instead see if possible
# to determine the value dynamically
case $ARCH in
esac
case $DRIVER in
esac
case $SOC in
esac
case $MACHINE in
esac


############# Do the work
do_cmd mtd_debug erase $MTD_CHAR_DEV_NODE $OFFSET $ERASE_SIZE


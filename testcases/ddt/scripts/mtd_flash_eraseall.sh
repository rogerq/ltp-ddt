#! /bin/sh

# Perform flash_eraseall on mtd device  
# Input 

source "common.sh"
source "st_log.sh"
source "mtd_common.sh"

############################### CLI Params ###################################

while getopts  :d:s:c:n:h arg
do case $arg in
        n)      
		# optional param
		PART="$OPTARG";;
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
test_print_trc "MTD_CHAR_DEV_NODE: $MTD_CHAR_DEV$PART"



############# Do the work ###########################################
do_cmd flash_eraseall "$MTD_CHAR_DEV_NODE" 

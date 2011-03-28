#! /bin/sh

# Perform flash_erase on mtd device  
# Input 

source "common.sh"
source "st_log.sh"
source "mtd_common.sh"

############################# Functions #######################################
usage()
{
cat <<-EOF >&2
        usage: ./${0##*/} [-n PART] [-d DEVICE_TYPE]
        -n PART         optional param; partition number
        -d DEVICE_TYPE  device type like 'nand', 'mmc', 'usb' etc
	-c CNT		cnt for flash_erase command
	-s START	start for flash_erase command
        -h Help         print this usage
EOF
exit 0
}


############################### CLI Params ###################################

while getopts  :d:s:c:n:h arg
do case $arg in
        n)      
		# optional param
		PART="$OPTARG";;
        s)      START="$OPTARG";;
        c)      CNT="$OPTARG";;
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

: ${START:='0'}
: ${CNT:='1'}


############# Do the work ###########################################
do_cmd flash_erase "$MTD_CHAR_DEV_NODE" "$START" "$CNT"

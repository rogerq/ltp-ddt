# !/bin/sh

# Get size of the specific partition
# Input: DEV_NODE	like /dev/mtdblock3, /dev/mmcblk0p1, /dev/sda1
#	DEVICE_TYPE	like 'nand', 'spi', 'mmc', 'usb'
# Output: DEVICE_PART_SIZE which is the size of the partition

#source "$LTPROOT/scripts/ddt/st_log.sh"
source "st_log.sh"

size=0
while getopts  :n:d:h: arg
do case $arg in
        n)      DEV_NODE="$OPTARG";;
        d)      DEVICE_TYPE="$OPTARG";;
        h)      usage;;
        :)      echo "$0: Must supply an argument to -$OPTARG." >&2
                exit 1
                ;;

        \?)     echo "Invalid Option -$OPTARG ignored." >&2
                usage
                exit 1
                ;;
esac
done

: ${DEV_NODE:='/dev/mtdblock3'}
: ${DEVICE_TYPE:='nand'}

#. $LTPROOT/scripts/ddt/helper_device_type_map.sh $DEVICE_TYPE
do_cmd . helper_device_type_map.sh $DEVICE_TYPE

case $DEV_TYPE in
	mtd)
		#. $LTPROOT/scripts/ddt/helper_get_mtd_size.sh $DEV_NODE
		#size=`$LTPROOT/scripts/ddt/helper_get_mtd_size.sh "$DEV_NODE"`
		size=`helper_get_mtd_size.sh "$DEV_NODE"` || die "error while getting the mtd size."
	 	#size=$MTD_SIZE

		# different way to get size; but could not figure out if it is symlink	
		#for file in $(find /sys/class/mtd/mtd$PART -type f -name size); do
		#	test_print_trc "file is:$file"
		#	size=`cat $file`
	#        #        echo SIZE=$size
		#done
		;;
	storage_device)
		# TODO: figure out a way to find out what the size is
		size=2040109465
		if [ $DEVICE_TYPE == "usb" ]; then
			size=2040109465
		fi
		;;
	*)
		test_print_wrn "Device type is not found; Size for Device Partition is set to default value: $size"
		exit 1
		;;
esac

export DEVICE_PART_SIZE=$size



#! /bin/sh
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
# @desc Script to run gpio test 

#source "common.sh"
source "super-pm-tests.sh"

############################# Functions #######################################
usage()
{
cat <<-EOF >&2
  usage: ./${0##*/}  [-l TEST_LOOP] [-t SYSFS_TESTCASE] [-i TEST_INTERRUPT]
  -l TEST_LOOP  test loop
  -t SYSFS_TESTCASE testcase like 'out', 'in'
  -i TEST_INTERRUPT if test interrupt, default is 0.
  -h Help   print this usage
EOF
exit 0
}

gpio_sysentry_get_item()  {
  GPIO_NUM=$1
  ITEM=$2

  VAL=`cat /sys/class/gpio/gpio${GPIO_NUM}/${ITEM}`
  echo "$VAL"
}

gpio_sysentry_set_item() {
  if [ $# -lt 3 ]; then
    echo "Error: Invalid Argument Count"
    echo "Syntax: $0 <gpio_num> <item like 'direction', 'value', 'edge'> <item value>"
    exit 1
  fi

  GPIO_NUM=$1
  ITEM=$2
  ITEM_VALUE=$3

  ORIG_VAL=`gpio_sysentry_get_item ${GPIO_NUM} ${ITEM}`
  test_print_trc "The value was ${ORIG_VAL} before setting ${ITEM}" 

  do_cmd "echo "$ITEM_VALUE" > /sys/class/gpio/gpio${GPIO_NUM}/${ITEM}"
  VAL_SET=`gpio_sysentry_get_item ${GPIO_NUM} ${ITEM}`
  if [ "${VAL_SET}" != "${ITEM_VALUE}" ]; then
    die "Value for GPIO ${GPIO_NUM} was not set to ${ITEM_VALUE}"
  else
    test_print_trc "GPIO ${GPIO_NUM} was set to ${ITEM_VALUE}"
  fi
}



############################### CLI Params ###################################
while getopts  :l:t:i:h arg
do case $arg in
  l)  TEST_LOOP="$OPTARG";;
  t)  SYSFS_TESTCASE="$OPTARG";;
  i)  TEST_INTERRUPT="$OPTARG";;
  h)  usage;;
  :)  test_print_trc "$0: Must supply an argument to -$OPTARG." >&2
    exit 1 
    ;;

  \?)  test_print_trc "Invalid Option -$OPTARG ignored." >&2
    usage 
    exit 1
    ;;
esac
done

########################### DYNAMICALLY-DEFINED Params ########################
: ${TEST_LOOP:='1'}
: ${TEST_INTERRUPT:='0'}

########################### REUSABLE TEST LOGIC ###############################
# DO NOT HARDCODE any value. If you need to use a specific value for your setup
# use user-defined Params section above.
test_print_trc "STARTING GPIO Test... "
test_print_trc "TEST_LOOP:${TEST_LOOP}"
test_print_trc "SYSFS_TESTCASE:${SYSFS_TESTCASE}"

# GPIO_NUM_IN_BANKS is gpio_num_in_bank collection. The numbers corresponds to 
# bank number. Ex, GPIO_NUM_IN_BANKS="0 15 10 8" means gpio pins to be tested 
# are pin0 for bank 0;
# pin 15 for bank 1; pin 10 for bank 2 and so on.
case $MACHINE in
  am180x-evm) 
    GPIO_NUM_IN_BANKS="0 15 10 8 8 8 8 4 12"
  ;;
  am335x-evm)
    GPIO_NUM_IN_BANKS="31 8 0 2"
  ;;
  am335x-sk)
    GPIO_NUM_IN_BANKS="3 8 1 15"
  ;;
  omap5-evm)
    GPIO_NUM_IN_BANKS="0 15 10 8 8 8 8 4"
  ;;
  beagleboard)
    GPIO_NUM_IN_BANKS="26 6 6 19 0 1"
  ;;
  keystone-evm)
    GPIO_NUM_IN_BANKS="3 8"
  ;;
  *)
    die "The gpio numbers are not available for this platform $MACHINE"
esac

BANK_NUM=0
for GPIO_NUM_IN_BANK in $GPIO_NUM_IN_BANKS; do

  # get gpio number based on bank number and gpio number in bank
  GPIO_NUM=$((${BANK_NUM}*32+${GPIO_NUM_IN_BANK}))
  EXTRA_PARAMS=""
  case $MACHINE in
    am180x-evm)  
      GPIO_NUM=$((${BANK_NUM}*16+${GPIO_NUM_IN_BANK}))
      GPIO_PIN_STRING="DA850_GPIO${BANK_NUM}_${GPIO_NUM_IN_BANK}"
      EXTRA_PARAMS="gpio_pin_string=${GPIO_PIN_STRING}"
    ;;
    keystone-evm)
      GPIO_NUM=$((${BANK_NUM}*16+${GPIO_NUM_IN_BANK})) 
    ;;
  esac

  test_print_trc "BANK_NUM:${BANK_NUM}"
  test_print_trc "GPIO_NUM_in_Bank:${GPIO_NUM_IN_BANK}"
  test_print_trc "GPIO_NUM:${GPIO_NUM}"
  test_print_trc "GPIO_PIN_STRING:${GPIO_PIN_STRING}"

  if [ "$TEST_INTERRUPT" = "1" ]; then
    do_cmd lsmod | grep gpio_test
    if [ $? -eq 0 ]; then
      test_print_trc "Module already inserted; Removing the module"
      do_cmd rmmod gpio_test.ko
      sleep 2
    fi
  fi

  if [ -n "$SYSFS_TESTCASE" ]; then
    if [ -e /sys/class/gpio/gpio"$GPIO_NUM" ]; then
      do_cmd "echo ${GPIO_NUM} > /sys/class/gpio/unexport"
      do_cmd ls /sys/class/gpio
      sleep 1
    fi
  fi

  if [ "$TEST_INTERRUPT" = "1" ]; then
    test_print_trc "Inserting gpio test module. Please wait..."
    do_cmd "cat /proc/interrupts"
    # wait TIMEOUT for app to finish; if not finished by TIMEOUT, kill it
    # gpio_test module return sucessfully only after the interrupt complete.
    # do_cmd "timeout 30 insmod ddt/gpio_test.ko gpio_num=${GPIO_NUM} test_loop=${TEST_LOOP} ${EXTRA_PARAMS}"
    ( do_cmd insmod ddt/gpio_test.ko gpio_num=${GPIO_NUM} test_loop=${TEST_LOOP} ${EXTRA_PARAMS} ) & pid=$!
    sleep 5; kill -9 $pid
    wait $pid
    if [ $? -ne 0 ]; then
      die "No interrupt is generated and gpio interrupt test failed."  
    fi 
    do_cmd cat /proc/interrupts |grep -i gpio
    #do_cmd check_debugfs 

    test_print_trc "Removing gpio test module. Please wait..."
    do_cmd rmmod gpio_test.ko
    sleep 3
    do_cmd cat /proc/interrupts
  fi

  # run sys entry tests if asked
  if [ -n "$SYSFS_TESTCASE" ]; then
    test_print_trc "Running sysfs test..."
    
    # test loop
    i=0
    while [ $i -lt $TEST_LOOP ]; do 
      test_print_trc "===LOOP: $i==="
      do_cmd "echo ${GPIO_NUM} > /sys/class/gpio/export"
      do_cmd ls /sys/class/gpio
      if [ -e /sys/class/gpio/gpio"$GPIO_NUM" ]; then
        case "$SYSFS_TESTCASE" in
        neg_reserve)
          test_print_trc "Try to reserve the same gpio again"
          test_print_trc "echo ${GPIO_NUM} > /sys/class/gpio/export"
          echo ${GPIO_NUM} > /sys/class/gpio/export
          if [ $? -eq 0 ]; then
            die "gpio should not be able to reserve gpio ${GPIO_NUM} which is already being reserved"
          fi
          ;;
        out)
          gpio_sysentry_set_item "$GPIO_NUM" "direction" "out"  
          if [ $? -ne 0 ]; then
            die "gpio_sysentry_set_item failed to set ${GPIO_NUM} to out"
          fi
          gpio_sysentry_set_item "$GPIO_NUM" "value" "0"
          if [ $? -ne 0 ]; then
            die "gpio_sysentry_set_item failed to set ${GPIO_NUM} to 0"
          fi
          gpio_sysentry_set_item "$GPIO_NUM" "value" "1"
          if [ $? -ne 0 ]; then
            die "gpio_sysentry_set_item failed to set ${GPIO_NUM} to 1"
          fi
          ;;
        in)
          gpio_sysentry_set_item "$GPIO_NUM" "direction" "in" || die "gpio_sysentry_set_item failed to set ${GPIO_NUM} to in"
          VAL=`gpio_sysentry_get_item "$GPIO_NUM" "value"` || die "gpio_sysentry_set_item failed to get the value of ${GPIO_NUM} " 
          test_print_trc "The value is ${VAL} for $GPIO_NUM" 
          ;;
        edge)
          gpio_sysentry_set_item "$GPIO_NUM" "edge" "falling"
          if [ $? -ne 0 ]; then
            die "gpio_sysentry_set_item failed to set ${GPIO_NUM} to falling"
          fi
          gpio_sysentry_set_item "$GPIO_NUM" "edge" "rising"
          if [ $? -ne 0 ]; then
            die "gpio_sysentry_set_item failed to set ${GPIO_NUM} to rising"
          fi
          gpio_sysentry_set_item "$GPIO_NUM" "edge" "both"
          if [ $? -ne 0 ]; then
            die "gpio_sysentry_set_item failed to set ${GPIO_NUM} to both"
          fi
          ;;
        pm_context_restore)
          gpio_sysentry_set_item "$GPIO_NUM" "direction" "out" || die "gpio_sysentry_set_item failed to set ${GPIO_NUM} to out"
          gpio_sysentry_set_item "$GPIO_NUM" "value" "1" || die "gpio_sysentry_set_item failed to set ${GPIO_NUM} to 1"
          VAL_BEFORE=`gpio_sysentry_get_item "$GPIO_NUM" "value"` || die "gpio_sysentry_set_item failed to get the value of ${GPIO_NUM} " 
          test_print_trc "The value was ${VAL_BEFORE} for $GPIO_NUM before suspend" 
  
          simple_suspend 1
          
          # check if the value is still the same as the one before suspend
          VAL_AFTER=`gpio_sysentry_get_item "$GPIO_NUM" "value"` || die "gpio_sysentry_set_item failed to get the value of ${GPIO_NUM} " 
          test_print_trc "The value was ${VAL_AFTER} for $GPIO_NUM after suspend" 

          # compare 
          if [ $VAL_BEFORE -ne $VAL_AFTER ]; then
            die "The value for gpio $GPIO_NUM is different before and after suspend"
          else
            test_print_trc "The values are the same before and after"
          fi
          ;;
        esac
      else
        die "/sys/class/gpio/gpio${GPIO_NUM} does not exist!"
      fi

      # remove gpio sys entry
      do_cmd "echo ${GPIO_NUM} > /sys/class/gpio/unexport" 
      do_cmd "ls /sys/class/gpio/"

      i=`expr $i + 1`
    done  # while loop
  fi

  BANK_NUM=`expr $BANK_NUM + 1`
done

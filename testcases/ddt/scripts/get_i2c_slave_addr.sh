#!/bin/sh
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

# Get I2C slave device address for different platform
# By default, this script return a default addr for each platform so 
# the sanity test case can be generic 
# If passing in slave_device name, this script will return the addr for 
# this slave device.
#
# Input: (optional)slave_device;
# Output: slave_dev_addr

source "common.sh"

############################### CLI Params ###################################
#if [ $# -lt 1 ]; then
#        echo "Error: Invalid Argument Count"
#        echo "Syntax: $0 [slave_device] "
#        exit 1
#fi
if [ $# -ge 1 -a -n $1 ]; then
	SLAVE_DEVICE=$1
fi
: ${SLAVE_DEVICE:='default'}

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
        am17x-evm)

                case $SLAVE_DEVICE in
                        eeprom)
                                SLAVE_ADDR=0x50
                        ;;
			default)
                                SLAVE_ADDR=0x50
			;;
			*)
				die "Unknown slave device! Can not get slave address."
			;;
                esac
                ;;

        dm365-evm)

                case $SLAVE_DEVICE in
                        eeprom)
                                SLAVE_ADDR=0x50
                        ;;
			default)
                                SLAVE_ADDR=0x50
			;;
			*)
				die "Unknown slave device! Can not get slave address."
			;;
                esac
                ;;

        dm368-evm)

                case $SLAVE_DEVICE in
                        eeprom)
                                SLAVE_ADDR=0x50
                        ;;
			default)
                                SLAVE_ADDR=0x50
			;;
			*)
				die "Unknown slave device! Can not get slave address."
			;;
                esac
                ;;

        dm644x-evm)

                case $SLAVE_DEVICE in
                        eeprom)
                                SLAVE_ADDR=0x50
                        ;;
			default)
                                SLAVE_ADDR=0x50
			;;
			*)
				die "Unknown slave device! Can not get slave address."
			;;
                esac
                ;;

        dm646x-evm)
                case $SLAVE_DEVICE in
                        eeprom)
                                SLAVE_ADDR=0x50
                        ;;
			default)
                                SLAVE_ADDR=0x50
			;;
			*)
				die "Unknown slave device! Can not get slave address."
			;;
                esac
                ;;

        am387x-evm|dm385-evm)
                case $SLAVE_DEVICE in
                        eeprom)
                                SLAVE_ADDR=0x50
                        ;;
			default)
                                SLAVE_ADDR=0x50
			;;
			*)
				die "Unknown slave device! Can not get slave address."
			;;
                esac
                ;;

        am389x-evm)
                case $SLAVE_DEVICE in
                        eeprom)
                                SLAVE_ADDR=0x50
                        ;;
			default)
                                SLAVE_ADDR=0x50
			;;
			*)
				die "Unknown slave device! Can not get slave address."
			;;
                esac
                ;;

        am3517-evm)

                case $SLAVE_DEVICE in
			pmic)
                                SLAVE_ADDR=0x48  
			;;
			default)
                                SLAVE_ADDR=0x48  
			;;
			*)
				die "Unknown slave device! Can not get slave address."
			;;
                esac
		;;


        am37x-evm)

                case $SLAVE_DEVICE in
			default)
                                SLAVE_ADDR=0x5d 
			;;
			*)
				die "Unknown slave device! Can not get slave address."
			;;
                esac
		;;

        am180x-evm)

                case $SLAVE_DEVICE in
                        io_expander)
                                SLAVE_ADDR=0x21
                        ;;
			default)
                                SLAVE_ADDR=0x21
			;;
			*)
				die "Unknown slave device! Can not get slave address."
			;;
                esac
		;;
    am335x-evm)

          case $SLAVE_DEVICE in
              temp_sensor)
                  SLAVE_ADDR=0x48
              ;;
              eeprom)
                  SLAVE_ADDR=0x52 # LCD eeprom
              ;;
			        default)
                  SLAVE_ADDR=0x48
			        ;;
			        *)
				          die "Unknown slave device! Can not get slave address."
			        ;;
          esac
		;;


	*)
		die "Invalid Machine name! No I2C slave addr found"
                ;;
esac

echo $SLAVE_ADDR

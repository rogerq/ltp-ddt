#! /bin/sh
############################################################################### 
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
###############################################################################
# @history 2012-01-02: First version
# @desc Verification of PWM funtionality 
#       Following modules can be tested with this script
#		a. eCap
#		b. eHRPWM
#		c. Haptics motor controlled by eHRPWM
#		d. Backlight controlled by eCap
#       Results must be validated based on 
#		a. Output waveform shown on CRO in case of eCap and eHRPWM
#       b. LCD display brightness variation in case of backlight
#		c. Vibrator noise in case of Haptics

source "common.sh"  # Import do_cmd(), die() and other functions

############################# Functions #######################################

usage()
{
    cat <<-EOF >&2
	    usage: ./${0##*/} [-t device_type] [-d duty][-D duty_type] [-p period] [-P period_type] [-r polarity] [-T time_delay] [-v bright_val]
	    -t device_type      Type of device. ehrpwm,ecap,backlight
	    -d duty				Duty cycle value. e.g. 50 For % or 0.5 for Seconds
	    -D duty_type		Type of value that -d duty has. seconds or percentage
		-p period          Period value. e.g. 200 For Hz or 0.004 for seconds
		-P period_type     Type of value that -p period has. frequency or seconds
		-r polarity        Polarity of duty second. 0 for +ve 1 for -ve
		-T time_delay      Duration for which driver is run. e.g. 3 for seconds
		-v bright_val      Value to be set for device_type backlight
	EOF
	exit 0
}


################################ CLI Params ####################################
# Please use getopts
while getopts  :h:d:D:p:P:r:t:T:v: arg
do case $arg in
        h)      usage="$OPTARG";;
		d)		duty="$OPTARG";;
		p)		period="$OPTARG";;
		r)		polarity="$OPTARG";;
		t)		device_type="$OPTARG";;
		P)		period_type="$OPTARG";;
		D)		duty_type="$OPTARG";;
		T)		time_delay="$OPTARG";;
		v)		bright_val="$OPTARG";;
	    :)      die "$0: Must supply an argument to -$OPTARG.";;
	    \?)     die "Invalid Option -$OPTARG ";;
esac
done
# Define default values if possible
############################ Default Values for Params ###############################

: ${duty:='50'}
: ${period:='200'}
: ${polarity='0'}
: ${period_type='frequency'}
: ${duty_type='percentage'}
: ${time_delay='3'}
: ${bright_val='50'}

############################ USER-DEFINED Params ###############################
# Try to avoid defining values here, instead see if possible
# to determine the value dynamically. ARCH, DRIVER, SOC and MACHINE are 
# initilized and exported by runltp script based on platform option (-P)
case $ARCH in
esac
case $DRIVER in
esac
case $SOC in
esac
case $MACHINE in
	am335x-evm)
		ecap_instance=0
		ehrpwm_instance=2
		ehrpwm_channel=1
		;;
	am180x-evm)
		ecap_instance=2
		ehrpwm_instance=1
		ehrpwm_channel=1
		;;
esac
# Define default values for variables being overriden

########################### REUSABLE TEST LOGIC ###############################
# DO NOT HARDCODE any value. If you need to use a specific value for your setup
# use USER-DEFINED Params section above.

case $device_type in
	ecap)
		device="ecap.$ecap_instance"
	;;
	ehrpwm)
		device="ehrpwm.$ehrpwm_instance\\:$ehrpwm_channel"
	;;
	backlight)
		device="backlight"
	;;
	*)
		usage
esac

# Set brightness value in case device is backlight
	if [[ "$device_type" == "backlight" ]]  ;
	then
		do_cmd "echo $bright_val > /sys/class/backlight/pwm-backlight/brightness" || die "setting brightness failed"
		sleep $time_delay
		exit 0
	fi
# Do a request to aquire the device
	req_status=`do_cmd "cat /sys/class/pwm/$device/request"`
	case $req_status in
		*requested*)
			test_print_trc "Device already requested, Releasing device for re-request"
			do_cmd "echo 0 > /sys/class/pwm/$device/request" || die "Release device failed"
			do_cmd "echo 1 > /sys/class/pwm/$device/request" || die "Request failed"
			;;
		*free*)
			test_print_trc "Device is free, requesting the device"
			do_cmd "echo 1 > /sys/class/pwm/$device/request" || die "Request failed"
			;;
	esac

# Configure the device ecap or eHRPWM
	do_cmd "echo $polarity > /sys/class/pwm/$device/polarity" || die "Setting polarity failed"

	if test "$period_type" = "frequency" 
	then
		do_cmd "echo $period > /sys/class/pwm/$device/period_freq" || die "setting period_freq failed"
		do_cmd "cat /sys/class/pwm/$device/period_freq" || die "print period_freq failed"
	fi
	if test "$period_type" = "seconds" ;
	then
		period=`echo "$period * 1000000000" | bc -l`
		period=${period%.*}
		do_cmd "echo $period > /sys/class/pwm/$device/period_ns" || die "setting period_ns failed"
		do_cmd "cat /sys/class/pwm/$device/period_ns" || die "print period_ns failed"
	fi
	
	if test "$duty_type" = "percentage" ; 
	then
		do_cmd "echo $duty > /sys/class/pwm/$device/duty_percent" || die "setting duty_percent failed"
		do_cmd "cat /sys/class/pwm/$device/duty_percent" || die "print duty_percent failed"
	fi

	if test "$duty_type" = "seconds"  ;
	then
		duty=`echo "$duty * 1000000000" | bc -l`
		duty=${duty%.*}
		do_cmd "echo $duty > /sys/class/pwm/$device/duty_ns" || die "setting duty_ns failed"
		do_cmd "cat /sys/class/pwm/$device/duty_ns" || die "print duty_ns failed"
	fi
	test_print_trc "Starting device $device"
	do_cmd "echo 1 > /sys/class/pwm/$device/run" || die "Run failed" 

	sleep $time_delay

	test_print_trc "Stopping device $device"
	do_cmd "echo 0 > /sys/class/pwm/$device/run" || die "Stop failed"

# Release the device once done with test case
	test_print_trc "Releasing device $device"
	do_cmd "echo 0 > /sys/class/pwm/$device/request" || die "Release device failed"



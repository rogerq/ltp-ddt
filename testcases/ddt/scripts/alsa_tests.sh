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
# @desc Captures/Plays/loopbacks the audio for given parameters
# @params t) Test type     : Capture,playback,loopback
#         r) Sample rate   : Sample rate (44100,48000,88200,96000 etc)
#         f) Sample Format : Sample Format (S8,S16_LE,S24_LE,S32_LE)
#         p) Period Size   : Period Size (1,2,4,8,etc)
#         b) Buffer Size   : Buffer Size (64,512,32768,65536)
#         d) Duration      : Duration in Secs.
#         c) Channel	   : Channel.
#         F) File Name	   : File name to play from or capture to.
#         o) OpMode		   : OpMode (0->Blocking, 1->Non-Blocking)
#         a) Access Type   : Access Type ( 0->Non Interleaved, 1-> Interleaved, 2->Mmap )
#         D) Audio Device  : Audio Device.
#         R) Record Device : Audio Record Device.
#         P) Playback Device  : Audio PlaybackDevice.
#         l) Capture Log   : Whether to retain captured file or delete.
#         u) URL           : URL of sound file to be played back 
# @history 2011-04-07: First version
# @history 2011-05-13: Removed st_log.sh
# @history 2012-05-30: Added wget support 
source "common.sh"  # Import do_cmd(), die() and other functions

############################# Functions #######################################
usage()
{
	cat <<-EOF >&2
	usage: ./${0##*/} [-t TEST_TYPE] [-D DEVICE] [-R REC_DEVICE] [-P PLAY_DEVICE] [-r SAMPLE_RATE] [-f SAMPLE_FORMAT] [-p PERIOD_SIZE] [-b BUFFER_SIZE] [-c CHANNEL] [-o OpMODE] [-a ACCESS_TYPE] [-d DURATION]
	-t TEST_TYPE		Test Type. Possible Values are Capture,playback,loopback.
	-D DEVICE           Device Name like hw:0,0.
	-R REC_DEVICE           Device Name like hw:0,0.
	-P PLAY_DEVICE           Device Name like hw:0,0.
	-r SAMPLE_RATE		Sample Rate like 44100,48000,88200,96000 etc.
	-f SAMPLE_FORMAT	Sample Format like S8,S16_LE,S24_LE,S32_LE.
	-p PERIOD_SIZE		Period Size like 1,2,4,8,etc.
	-b BUFFER_SIZE		Buffer Size like 64,512,32768,65536.
	-c CHANNEL			Channel like 1,2.
	-F FILENAME			File Name to capture to or playback from	
	-o OpMODE			OpMode (0->Blocking, 1->Non-Blocking)
	-a ACCESS_TYPE		Access Type ( 0->Non Interleaved, 1-> Interleaved,2->Mmap )
	-d DURATION         Dutaion In Secs like 5,10 etc. 
	-l CAPTURELOG_FLAG  Whether to retain captured file or delete.( 1 -> To retain, 0 -> delete )
	-u URL				URL of sound file to be played back
	EOF
	exit 0
	
}

################################ CLI Params ####################################
# Please use getopts
while getopts  :t:r:f:F:p:b:l:d:c:o:a:D:R:P:u:h arg
do case $arg in
        t)      TYPE="$OPTARG";;
        r)      SAMPLERATE="$OPTARG";;        
        f)      SAMPLEFORMAT="$OPTARG";;        
        p)      PERIODSIZE="$OPTARG";;        
        b)      BUFFERSIZE="$OPTARG";;                
        d)      DURATION="$OPTARG";;                                
        c)      CHANNEL="$OPTARG";;                                
        o)      OPMODE="$OPTARG";;                                
        F)      FILE="$OPTARG";;                                        
        a)      ACCESSTYPE="$OPTARG";;                                
        D)      DEVICE="$OPTARG";;        
        R)      REC_DEVICE="$OPTARG";;        
        P)      PLAY_DEVICE="$OPTARG";;        
        l)      CAPTURELOGFLAG="$OPTARG";;                
	    u)      URL="$OPTARG";; 
        h)      usage;;
        :)      die "$0: Must supply an argument to -$OPTARG.";; 
        \?)     die "Invalid Option -$OPTARG ";;
esac
done

############################ Default Values for Params ###############################

: ${TYPE:='loopback'}
: ${FILE:='test.snd'}
: ${SAMPLERATE:='48000'}
: ${SAMPLEFORMAT:='S16_LE'}
: ${PERIODSIZE:='2048'}
: ${BUFFERSIZE:='32768'}
: ${DURATION:='10'}
: ${CHANNEL:='2'}
: ${OPMODE:='0'}
: ${ACCESSTYPE:='0'}
: ${DEVICE:='hw:0,0'}
: ${REC_DEVICE:='hw:0,0'}
: ${PLAY_DEVICE:='hw:0,0'}
: ${CAPTURELOGFLAG:='0'}
: ${URL:=''}
if [ $OPMODE -eq 0 ] ; then
	OPMODEARG=""
else
	OPMODEARG="-N"
fi

if [ $ACCESSTYPE -eq 0 ] ; then
	ACCESSTYPEARG=""
elif [ $ACCESSTYPE -eq 1 ] ; then
	ACCESSTYPEARG="-M"
fi

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
*am37x-evm|omap3evm|beagleboard)
	amixer cset name='Analog Left AUXL Capture Switch' 1
	amixer cset name='Analog Right AUXR Capture Switch' 1
	amixer cset name='HeadsetL Mixer AudioL1' on
	amixer cset name='HeadsetR Mixer AudioR1' on
	amixer cset name='Headset Playback Volume' 3
;;
*da850-omapl138-evm)
	amixer cset name='PCM Playback Volume' 127,127 
;;
esac

########################### REUSABLE TEST LOGIC ###############################
# DO NOT HARDCODE any value. If you need to use a specific value for your setup
# use USER-DEFINED Params section above.

# Print the test params.

test_print_trc " ****************** TEST PARAMETERS ******************"
test_print_trc " TYPE         : $TYPE"
test_print_trc " DEVICE       : $DEVICE"
test_print_trc " REC_DEVICE   : $REC_DEVICE"
test_print_trc " PLAY_DEVICE  : $PLAY_DEVICE"
test_print_trc " DURATION     : $DURATION"
test_print_trc " SAMPLERATE   : $SAMPLERATE"
test_print_trc " SAMPLEFORMAT : $SAMPLEFORMAT"
test_print_trc " PERIODSIZE   : $PERIODSIZE"
test_print_trc " BUFFERSIZE   : $BUFFERSIZE"
test_print_trc " CHANNEL      : $CHANNEL"
test_print_trc " OPMODE       : $OPMODE"
test_print_trc " FILE         : $FILE"
test_print_trc " ACCESSTYPE   : $ACCESSTYPE"
if test "$URL" != ''
then
test_print_trc " URL          : $URL"
fi
test_print_trc " *************** END OF TEST PARAMETERS ***************"


case "$TYPE" in
	
	capture)
		do_cmd arecord -D "$DEVICE" -f "$SAMPLEFORMAT" $FILE -d "$DURATION" -r "$SAMPLERATE" -c "$CHANNEL" "$ACCESSTYPEARG" "$OPMODEARG" --buffer-size=$BUFFERSIZE --period-size $PERIODSIZE
		;;		
	playback)
		if test "$URL" != ''
		then
			do_cmd wget $URL -O $FILE
		fi

		if [ ! -s $FILE ]
		then
			test_print_trc "$FILE Does not exists or has size zero. Using /dev/urandom as input file to generate noise"
			FILE="/dev/urandom"
		fi
		do_cmd aplay -D "$DEVICE" -f "$SAMPLEFORMAT" $FILE -d "$DURATION" -r "$SAMPLERATE" -f "$SAMPLEFORMAT" -c "$CHANNEL" "$ACCESSTYPEARG" "$OPMODEARG"  --buffer-size=$BUFFERSIZE --period-size $PERIODSIZE
		;;		
	loopback)
		do_cmd arecord -D "$REC_DEVICE" -f "$SAMPLEFORMAT" -d "$DURATION" -r "$SAMPLERATE" -c "$CHANNEL" "$ACCESSTYPEARG" "$OPMODEARG"  --buffer-size=$BUFFERSIZE --period-size $PERIODSIZE "|" aplay -D "$PLAY_DEVICE" -f "$SAMPLEFORMAT" -d "$DURATION" -r "$SAMPLERATE" -c "$CHANNEL" "$ACCESSTYPEARG" "$OPMODEARG"  --buffer-size=$BUFFERSIZE --period-size $PERIODSIZE
		;;		
esac	

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
#         l) Capture Log   : Whether to retain captured file or delete.



source "common.sh"
source "st_log.sh"
source "usb_audio_search_devices.sh"

############################# Functions #######################################
usage()
{
	cat <<-EOF >&2
	usage: ./${0##*/} [-t TEST_TYPE] [-D DEVICE][-r SAMPLE_RATE] [-f SAMPLE_FORMAT] [-p PERIOD_SIZE] [-b BUFFER_SIZE] [-c CHANNEL] [-o OpMODE] [-a ACCESS_TYPE] [-d DURATION]
	-t TEST_TYPE		Test Type. Possible Values are Capture,playback,loopback.
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
	EOF
	exit 0
	
}

################################ CLI Params ####################################
# Please use getopts
while getopts  :t:r:f:F:p:b:l:d:c:o:a:D:h arg
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
        l)      CAPTURELOGFLAG="$OPTARG";;                
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
: ${CAPTURELOGFLAG:='0'}

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
esac


########################### REUSABLE TEST LOGIC ###############################

# DO NOT HARDCODE any value. If you need to use a specific value for your setup
# use USER-DEFINED Params section above.


#initalize
Audio_Search_Device
DEVICE=$audiodev
if test "$audiodev" = "none";
then
	test_print_trc " ::"
	test_print_trc " :: USB Audio device not found. Exiting USB Audio tests..."
	test_print_trc " ::"
	do_cmd 'FAIL,USB Audio device not found'
	return
fi

test_print_trc "TestName:: USB Audio play"
test_print_trc "TestDesc:: Play a wav file on USB audio device"

# Print the test params.

test_print_trc " ****************** TEST PARAMETERS ******************"
test_print_trc " TYPE		: $TYPE"
test_print_trc " DEVICE		: $DEVICE"
test_print_trc " DURATION	: $DURATION"
test_print_trc " SAMPLERATE	: $SAMPLERATE"
test_print_trc " SAMPLEFORMAT	: $SAMPLEFORMAT"
test_print_trc " PERIODSIZE	: $PERIODSIZE"
test_print_trc " BUFFERSIZE	: $BUFFERSIZE"
test_print_trc " CHANNEL	: $CHANNEL"
test_print_trc " OPMODE		: $OPMODE"
test_print_trc " FILE		: $FILE"
test_print_trc " ACCESSTYPE	: $ACCESSTYPE"

test_print_trc " *************** END OF TEST PARAMETERS ***************"



case "$TYPE" in
	
	capture)
#		do_cmd arecord -D  "hw:$audiodev,0" -f "$SAMPLEFORMAT" $FILE -d "$DURATION" -r "$SAMPLERATE" -c "$CHANNEL" "$ACCESSTYPEARG" "$OPMODEARG" --buffer-size=$BUFFERSIZE --period-size $PERIODSIZE
		do_cmd arecord  -f "$SAMPLEFORMAT" $FILE -d "$DURATION" -r "$SAMPLERATE" -c "$CHANNEL" "$ACCESSTYPEARG" "$OPMODEARG" --buffer-size=$BUFFERSIZE --period-size $PERIODSIZE

		;;		
	playback)
#		do_cmd aplay -D  "hw:$audiodev,0" -f "$SAMPLEFORMAT" $FILE -d "$DURATION" -r "$SAMPLERATE" -f "$SAMPLEFORMAT" -c "$CHANNEL" "$ACCESSTYPEARG" "$OPMODEARG"  --buffer-size=$BUFFERSIZE --period-size $PERIODSIZE
		do_cmd aplay -f "$SAMPLEFORMAT" $FILE -d "$DURATION" -r "$SAMPLERATE" -f "$SAMPLEFORMAT" -c "$CHANNEL" "$ACCESSTYPEARG" "$OPMODEARG"  --buffer-size=$BUFFERSIZE --period-size $PERIODSIZE
 

		;;		
	loopback)
#		do_cmd arecord -D  "hw:$audiodev,0" -f "$SAMPLEFORMAT" -d "$DURATION" -r "$SAMPLERATE" -c "$CHANNEL" "$ACCESSTYPEARG" "$OPMODEARG"  --buffer-size=$BUFFERSIZE --period-size $PERIODSIZE "|" aplay -D "$DEVICE" -f "$SAMPLEFORMAT" -d "$DURATION" -r "$SAMPLERATE" -c "$CHANNEL" "$ACCESSTYPEARG" "$OPMODEARG"  --buffer-size=$BUFFERSIZE --period-size $PERIODSIZE
		do_cmd arecord  -f "$SAMPLEFORMAT" -d "$DURATION" -r "$SAMPLERATE" -c "$CHANNEL" "$ACCESSTYPEARG" "$OPMODEARG"  --buffer-size=$BUFFERSIZE --period-size $PERIODSIZE "|" aplay -f "$SAMPLEFORMAT" -d "$DURATION" -r "$SAMPLERATE" -c "$CHANNEL" "$ACCESSTYPEARG" "$OPMODEARG"  --buffer-size=$BUFFERSIZE --period-size $PERIODSIZE

		;;		
esac	


test_print_trc "This test requires manual intervention."
test_print_trc "It will be skipped."

test_print_end $TestId


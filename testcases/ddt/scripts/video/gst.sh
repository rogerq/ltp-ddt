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

# Test video functionality using gstreamer
# Input: File to be downloaded and test file name
# Output: Make the input stream available for running test

source "st_log.sh"
source "common.sh"

############################# Functions #######################################
usage()
{
cat <<-EOF >&2
        usage: ./${0##*/} [-d DOWNLOAD] -c <CODEC> -r <RESOLUTION>
        -d DOWNLOAD	stream to be downloaded from smruti.india.ti.com
        -c CODEC 	codec h264, mpeg4, mpeg2, mpeg4aac
	-r RESOLUTION	resolution of stream
        -h Help         print this usage
EOF
exit 0
}
############################ Script Variables ##################################
# Define default valus if possible

############################ CLI Params ###########################################
while getopts :d:c:r: arg
do case $arg in
        d)
                DOWNLOAD=$OPTARG ;;
        c)
                CODEC=$OPTARG ;;
        r)
                RESOLUTION=$OPTARG ;;
        \?)
		echo "Invalid Option -$OPTARG ignored." >&2
                usage
                exit 1
                ;;
esac
done

############################ USER-DEFINED Params ###############################
case $CODEC in
   264)
      FILE="/usr/share/ti/video/HistoryOfTI-$RESOLUTION.264"
      GSTCMD='gst-launch-0.10 filesrc location="$FILE" ! h264parse ! ffdec_h264 !\
		 ffmpegcolorspace ! fbdevsink device=/dev/fb';;
   mpeg2)
      FILE="/usr/share/ti/video/HistoryOfTI-$RESOLUTION.m2v"
      GSTCMD='gst-launch-0.10 filesrc location="$FILE" ! mpegvideoparse ! \
		ffdec_mpeg2video ! ffmpegcolorspace ! fbdevsink device=/dev/fb';;
   mpeg4)
      FILE="/usr/share/ti/video/HistoryOfTI-$RESOLUTION.m4v"
      GSTCMD='gst-launch-0.10 filesrc location="$FILE" ! mpeg4videoparse ! \
		ffdec_mpeg4 ! ffmpegcolorspace ! fbdevsink device=/dev/fb';;
   mpeg4aac)
      FILE="/usr/share/ti/video/HistoryOfTIAV-$RESOLUTION.mp4"
      GSTCMD='gst-launch-0.10 filesrc location="$FILE" ! qtdemux name=demux demux.audio_00 \
		! faad ! alsasink sync=false demux.video_00 ! queue ! ffdec_mpeg4 ! \
		ffmpegcolorspace ! fbdevsink device=/dev/fb';;
esac

########################### DYNAMICALLY-DEFINED Params #########################
# Try to use /sys and /proc information to determine values dynamically.
# Alternatively you should check if there is an existing script to get the
# value you want

########################### REUSABLE TEST LOGIC ###############################
if [ -e "$FILE" ];then
   echo "Demo file $FILE exists"
else
   FILE="/usr/share/ti/video/$DOWNLOAD"
   echo "Running $FILE"
   if [ ! -s "$FILE" ];then
      echo "Downloading stream $DOWNLOAD from SMRUTI"
      do_cmd 'wget http://smruti.india.ti.com/streams/download/"$DOWNLOAD" -O "/usr/share/ti/video/$DOWNLOAD"'\
 || die "Stream not available!"
   fi
fi

do_cmd "$GSTCMD"



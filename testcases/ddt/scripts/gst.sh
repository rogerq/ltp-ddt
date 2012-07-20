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

DOWNLOAD=$1
CODEC=$2

case $CODEC in
   264)
      FILE="HistoryOfTI-480p.264";;
   mpeg2)
      FILE="HistoryOfTI-480p.m2v";;
   mpeg4)
      FILE="HistoryOfTI-480p.m4v";;
esac
echo $FILE
if [ -f "/usr/share/ti/video/$FILE" ];
then
   echo "Demo file exists"
   `cp /usr/share/ti/video/"$FILE" testfile`
fi

if [ ! -s testfile ]
then
   echo "Downloading stream from SMRUTI"
   `wget http://smruti.india.ti.com/streams/download/"$DOWNLOAD" -O testfile`
fi


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

# Search for HID event number

source "common.sh"
source "st_log.sh"


############################# Functions #######################################

############################### CLI Params ###################################

############################ DEFAULT Params #######################

############# Do the work ###########################################

HID_Get_Event()
{
device=$1
events=`ls -d $device/input/input*`
for event in $events
do
	inputevent=`basename "$event" | cut -c6-7`
	echo INPUT event is $hidevent
done
events=`ls -d $device/input/input$inputevent/event*`
for event in $events
do
	hidevent=`basename "$event" | cut -c6-7`
	echo HID event is $hidevent
done
}

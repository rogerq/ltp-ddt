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

# search for USB audio devices

source "common.sh"
source "st_log.sh"


############################# Functions #######################################

############################### CLI Params ###################################

############################ DEFAULT Params #######################

############# Do the work ###########################################

if [ "$1" = "play" ]; then
 cmd="aplay -l"
fi

if [ "$1" = "record" -o "$1" = "loopback" ];
then
  cmd="arecord -l"
fi

audiodev=`$cmd | egrep "^card\s+[[:digit:]]+.+USB\s+Audio"| head -n 1 | cut -d':' -f 1 | cut -d' ' -f 2`
if [ "$audiodev" = "" ]; then
	test_print_trc " ::"
	test_print_trc " :: No USB Audio $1 device found. Exiting Audio tests..."
	test_print_trc " ::"	
 	exit 1 
elif [ "$1" = "loopback" ]; then
	cmd="aplay -l"
	audioplay=`$cmd | egrep "^card\s+[[:digit:]]+.+USB\s+Audio"| head -n 1 | cut -d':' -f 1 | cut -d' ' -f 2`
audiodev=$audiodev$audioplay
echo $audiodev
else
  echo $audiodev
fi


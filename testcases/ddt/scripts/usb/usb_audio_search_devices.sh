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

# search for USB audio devices - first parameter is operation such as 
# play, record and loopback, second parameter is name of driver such as
# usb, usbxhci

source "common.sh"
source "st_log.sh"


############################# Functions #######################################

############################### CLI Params ###################################

############################ DEFAULT Params #######################



############# Do the work ###########################################
usb_interface=$2
playcmd="aplay -l"
reccmd="arecord -l"
audioplaydev=`$playcmd | egrep "^card\s+[[:digit:]]+.+USB\s+Audio" | cut -d':' -f 1 | cut -d' ' -f 2`
audiorecdev=`$reccmd | egrep "^card\s+[[:digit:]]+.+USB\s+Audio" | cut -d':' -f 1 | cut -d' ' -f 2`
if [ "$1" = "record" -o "$1" = "loopback" ];
then
  for reccard in $audiorecdev
  do
      for playcard in $audioplaydev
      do
        if [[ $reccard = $playcard ]]
        then
            #echo "Found a match $reccard and $playcard"
           audiodev=$reccard
           break
        fi 
      done
  done
else
  for playcard in $audioplaydev                                                   
  do                                                                            
      for reccard in $audiorecdev                                             
      do                                                                        
        if [[ $playcard = $reccard ]]                                           
        then                                                                    
            #echo "Found a match $reccard and $playcard"                        
           audiodev=$playcard                                                    
           break                                                                
        fi                                                                      
      done                                                                      
  done      
fi
# if udev is supported, using output to confirm controller that is to be used
udev_present=""
udev_present=`ps|grep -m 1 udevd|grep -v grep`
if  [ -n "$udev_present" ];
then
    udev_tree_search=""
    usb_cnt_interface=`get_usb_controller_name.sh "$usb_interface"`
    udev_tree_search=`udevadm info -a -n /\dev/\snd/\controlC$audiodev|grep -m 1 "$usb_cnt_interface"`
       if [ -z "$udev_tree_search" ]; then
            test_print_trc " ::"
            test_print_trc " :: No USB Audio $2 device found. Exiting Audio tests..."
            test_print_trc " ::"	
            exit 1 
       fi
fi
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

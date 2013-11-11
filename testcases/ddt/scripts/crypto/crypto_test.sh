#!/bin/sh
# crypto_test.sh
# verifies whether log from tcrypt contains only permissible errors
# usage:  crypto_test.sh 

FILENAME='/var/log/messages'
fail_flag=0
searchstring="user.err"
while read LINE
do
  case $LINE in

 *"$searchstring"*) 
                    case $LINE in
                    *"lrw"*)  echo "LRW failure";;
                    *"xts"*) echo "XTS failure";;
                    *"cfb"*) echo "CFB failure";;
                    *"ofb"*) echo "OFB failure";;
                    *"rfc3686"*) echo "RFC3686 failure";;
                    *"request size is not exact amount"*) echo "Input padding failure";;
                    *"ctr"*) echo "CTR failures";;
                    *) echo "TRUE failure"
                       echo $LINE
                       fail_flag=1
                    ;;
                    esac
;;
 *) ;;

 esac
done < $FILENAME
exit $fail_flag

#!/bin/sh

VAR=`cat /proc/interrupts | grep 'timer' | awk -F '[:I]' '{print $2}'`
echo $VAR
sleep 10
VAR10=`cat /proc/interrupts | grep 'timer' | awk -F '[:I]' '{print $2}'`
echo $VAR10
EXP=`zcat /proc/config.gz | grep 'CONFIG_HZ' | awk -F '[:=]' '{print $2}'`
echo $EXP
EXP=`echo "($EXP * 10)" | bc`
EXPPER=`echo "($EXP * 0.1)" | bc` #Calculating 10% of expected increase
ACTPER=`expr $VAR10-$VAR`

echo "The difference is $ACTPER"

if [ $ACTPER '<' $EXPPER ]; then
   exit 0
fi
exit 1

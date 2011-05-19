#! /bin/sh
#ssh root@158.218.103.126 "iperf -s -u -w 128k"

for LABEL in 1M_1500 5M_1500 10M_1500 25M_1500 50M_1500 100M_1500 1M_1000 5M_1000 10M_1000 25M_1000 50M_1000 100M_1000 1M_500 5M_500 10M_500 25M_500 50M_500 1M_200 5M_200 10M_200 1M_64 5M_64 10M_64
do
  BW=`echo ${LABEL} | cut -d "_" -f 1`
  SZ=`echo ${LABEL} | cut -d "_" -f 2`
  echo "Running IPERF PC TO DUT WITH BW=${BW} and PACKET_SIZE=${SZ}"
  iperf -c 158.218.103.126 -l ${SZ} -f M -u -t 30 -b ${BW} -w 128k 
done

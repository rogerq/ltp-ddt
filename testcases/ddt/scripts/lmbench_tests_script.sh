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

# source for getting log routines

source "st_log.sh"

# initialze counters
initalize()
{
	PASS_COUNT=0;
	FAIL_COUNT=0;
}

# prints the sumary result
print_summary()
{
	TOTAL_COUNT=$(($PASS_COUNT+$FAIL_COUNT))

	test_print_trc "-------- TEST SUMMARY  --------"
	test_print_trc " Total Test Case executed: $TOTAL_COUNT"
	test_print_trc " Total Test Case Passed  : $PASS_COUNT"
	test_print_trc " Total Test Case failed  : $FAIL_COUNT"
	test_print_trc "-------- TEST SUMMARY  --------"
}

#update result count
update_result()
{
	if [ $1 -eq 0 ] ; then
		PASS_COUNT=$(( $PASS_COUNT+1 ));
	else
		FAIL_COUNT=$(( $FAIL_COUNT+1 ));
	fi
	RESULT=0	
}

# lmbench tests
lmbench_tests()
{
test_print_trc " ***** STARTING LMBENCH SCRIPT ***** "

	initalize
	RESULT=0
	
	test_print_trc " CREATING THE FILE OF 10MB SIZE FOR BENCHMARKING"
	dd if=/dev/zero of=test1.txt bs=1M count=10

	test_print_trc " ***** STARTING BANDWIDTH BENCHMARKS ***** "
	test_print_trc " MEMORY BANDWIDTH BENCHMARKS "

	test_print_start bw_mem
	test_print_trc " Parameters      : "
	test_print_trc " Operation       - rd"
	test_print_trc " Memory Blk Size - 1 MB"
	bw_mem 1M rd
	RESULT=$(( $RESULT + $? ));
	if [ $RESULT -eq 0 ] ; then
		test_print_result PASS bw_mem
	else
		test_print_result FAIL bw_mem
	fi	
	test_print_end bw_mem
	update_result $RESULT 
	
	test_print_start bw_mem
	test_print_trc " Parameters      : "
	test_print_trc " Operation       - wr"
	test_print_trc " Memory Blk Size - 1 MB"
	bw_mem 1M wr
	RESULT=$(( $RESULT + $? ));
	if [ $RESULT -eq 0 ] ; then
		test_print_result PASS bw_mem
	else
		test_print_result FAIL bw_mem
	fi	
	test_print_end bw_mem
	update_result $RESULT 	
	
	test_print_start bw_mem
	test_print_trc " Parameters      : "
	test_print_trc " Operation       - rdwr"
	test_print_trc " Memory Blk Size - 1 MB"
	bw_mem 1M rdwr
	RESULT=$(( $RESULT + $? ));
	if [ $RESULT -eq 0 ] ; then
		test_print_result PASS bw_mem
	else
		test_print_result FAIL bw_mem
	fi	
	test_print_end bw_mem
	update_result $RESULT 	

	test_print_start bw_mem
	test_print_trc " Parameters      : "
	test_print_trc " Operation       - cp"
	test_print_trc " Memory Blk Size - 1 MB"
	bw_mem 1M cp
	RESULT=$(( $RESULT + $? ));
	if [ $RESULT -eq 0 ] ; then
		test_print_result PASS bw_mem
	else
		test_print_result FAIL bw_mem
	fi	
	test_print_end bw_mem
	update_result $RESULT 		

	test_print_start bw_mem
	test_print_trc " Parameters      : "
	test_print_trc " Operation       - fwr"
	test_print_trc " Memory Blk Size - 1 MB"
	bw_mem 1M fwr
	RESULT=$(( $RESULT + $? ));
	if [ $RESULT -eq 0 ] ; then
		test_print_result PASS bw_mem
	else
		test_print_result FAIL bw_mem
	fi	
	test_print_end bw_mem
	update_result $RESULT 	

	test_print_start bw_mem
	test_print_trc " Parameters      : "
	test_print_trc " Operation       - frd"
	test_print_trc " Memory Blk Size - 1 MB"
	bw_mem 1M frd
	RESULT=$(( $RESULT + $? ));
	if [ $RESULT -eq 0 ] ; then
		test_print_result PASS bw_mem
	else
		test_print_result FAIL bw_mem
	fi	
	test_print_end bw_mem
	update_result $RESULT 	

	test_print_start bw_mem
	test_print_trc " Parameters      : "
	test_print_trc " Operation       - fcp"
	test_print_trc " Memory Blk Size - 1 MB"
	bw_mem 1M fcp
	RESULT=$(( $RESULT + $? ));
	if [ $RESULT -eq 0 ] ; then
		test_print_result PASS bw_mem
	else
		test_print_result FAIL bw_mem
	fi	
	test_print_end bw_mem
	update_result $RESULT 	

	test_print_start bw_mem
	test_print_trc " Parameters      : "
	test_print_trc " Operation       - bzero"
	test_print_trc " Memory Blk Size - 1 MB"
	bw_mem 1M bzero
	RESULT=$(( $RESULT + $? ));
	if [ $RESULT -eq 0 ] ; then
		test_print_result PASS bw_mem
	else
		test_print_result FAIL bw_mem
	fi	
	test_print_end bw_mem
	update_result $RESULT 	

	test_print_start bw_mem
	test_print_trc " Parameters      : "
	test_print_trc " Operation       - bcopy"
	test_print_trc " Memory Blk Size - 1 MB"
	bw_mem 1M bcopy
	RESULT=$(( $RESULT + $? ));
	if [ $RESULT -eq 0 ] ; then
		test_print_result PASS bw_mem
	else
		test_print_result FAIL bw_mem
	fi	
	test_print_end bw_mem
	update_result $RESULT 	

	test_print_trc " BANDWIDTH OF PIPE COMMUNICATION BETWEEN PARENT AND CHILD "
	test_print_start bw_unix
	test_print_trc " Parameters      : none"
	bw_unix	
	RESULT=$(( $RESULT + $? ));
	if [ $RESULT -eq 0 ] ; then
		test_print_result PASS bw_unix
	else
		test_print_result FAIL bw_unix
	fi	
	test_print_end bw_unix
	update_result $RESULT 	
	
	test_print_trc " BANDWIDTH OF FILE READ"
	test_print_start bw_file_rd
	test_print_trc " Parameters      : "
	test_print_trc " Size            - 1MB"
	test_print_trc " operation       - open2close"
	test_print_trc " File            - test1.txt"	
	bw_file_rd 1M open2close test1.txt	
	RESULT=$(( $RESULT + $? ));
	if [ $RESULT -eq 0 ] ; then
		test_print_result PASS bw_file_rd
	else
		test_print_result FAIL bw_file_rd
	fi	
	test_print_end bw_file_rd
	update_result $RESULT 		

	test_print_start bw_file_rd
	test_print_trc " Parameters      : "
	test_print_trc " Size            - 1MB"
	test_print_trc " operation       - io_only"
	test_print_trc " File            - test1.txt"	
	bw_file_rd 1M io_only test1.txt	
	RESULT=$(( $RESULT + $? ));
	if [ $RESULT -eq 0 ] ; then
		test_print_result PASS bw_file_rd
	else
		test_print_result FAIL bw_file_rd
	fi	
	test_print_end bw_file_rd
	update_result $RESULT 		

	test_print_trc " BANDWIDTH OF CREATING A MEMORY MAPPING TO A FILE AND READING"
	test_print_start bw_mmap_rd	
	test_print_trc " Parameters      : "	
	test_print_trc " Size            - 1MB"
	test_print_trc " operation       - open2close"
	test_print_trc " File            - test1.txt"
	bw_mmap_rd 1M open2close test1.txt
	RESULT=$(( $RESULT + $? ));
	if [ $RESULT -eq 0 ] ; then
		test_print_result PASS bw_mmap_rd
	else
		test_print_result FAIL bw_mmap_rd
	fi	
	test_print_end bw_mmap_rd
	update_result $RESULT 		
	
	test_print_start bw_mmap_rd		
	test_print_trc " Parameters      : "	
	test_print_trc " Size            - 1MB"
	test_print_trc " operation       - mmap_only"
	test_print_trc " File            - test1.txt"
	bw_mmap_rd 1M mmap_only test1.txt
	RESULT=$(( $RESULT + $? ));
	if [ $RESULT -eq 0 ] ; then
		test_print_result PASS bw_mmap_rd
	else
		test_print_result FAIL bw_mmap_rd
	fi	
	test_print_end bw_mmap_rd
	update_result $RESULT 			
	

	test_print_trc " BANDWIDTH OF CREATING A UNIX PIPE BETWEEN TWO PROCESS AND MOVEING 50 MB DATA THROUGH PIPE IN 64 KB CHUNKS"
	test_print_start bw_pipe		
	test_print_trc " Parameters      : none"
	bw_pipe 
	RESULT=$(( $RESULT + $? ));
	if [ $RESULT -eq 0 ] ; then
		test_print_result PASS bw_pipe
	else
		test_print_result FAIL bw_pipe
	fi	
	test_print_end bw_pipe
	update_result $RESULT 	

	test_print_trc " BANDWIDTH OF TCP/IP SOCKET ( DATA IS MOVED IN 48KB CHUNKS )"
	test_print_start bw_tcp			
	test_print_trc " Parameters      : "
	test_print_trc " hostname        - localhost"
	test_print_trc " transfersize    - none(default 3MB )"
	bw_tcp localhost
	RESULT=$(( $RESULT + $? ));
	if [ $RESULT -eq 0 ] ; then
		test_print_result PASS bw_tcp
	else
		test_print_result FAIL bw_tcp
	fi	
	test_print_end bw_tcp
	update_result $RESULT 		

test_print_trc " ***** EXITING THE BANDWIDTH BENCHMARKING ***** "

test_print_trc " ***** STARTING LATENCY BENCHMARKS ***** "

	test_print_trc " LATENCY OF INTERPROCESS CONNECTION VIA TCP/IP "
	test_print_start lat_connect			
	test_print_trc " Parameters      : "
	test_print_trc " hostname        - localhost"
	lat_connect localhost
	RESULT=$(( $RESULT + $? ));
	if [ $RESULT -eq 0 ] ; then
		test_print_result PASS lat_connect
	else
		test_print_result FAIL lat_connect
	fi	
	test_print_end lat_connect
	update_result $RESULT 	

	test_print_trc " LATENCY OF FILE CREATION AND DELETION "
	test_print_start lat_fs					
	test_print_trc " Parameters      : "
	test_print_trc " dir             - none(default current working directory)"
	lat_fs
	RESULT=$(( $RESULT + $? ));
	if [ $RESULT -eq 0 ] ; then
		test_print_result PASS lat_fs
	else
		test_print_result FAIL lat_fs
	fi	
	test_print_end lat_fs
	update_result $RESULT 	

	test_print_trc " LATENCY OF CONTEXT SWITCHING "
	test_print_start lat_ctx					
	test_print_trc " Parameters      : "
	test_print_trc " procs           - 2"
	test_print_trc " size_in_kbytes  - 128K"
	lat_ctx -s 128K processes 2
	RESULT=$(( $RESULT + $? ));
	if [ $RESULT -eq 0 ] ; then
		test_print_result PASS lat_ctx
	else
		test_print_result FAIL lat_ctx
	fi	
	test_print_end lat_ctx
	update_result $RESULT 		

	test_print_start lat_ctx					
	test_print_trc " Parameters      : "
	test_print_trc " procs           - 2"
	test_print_trc " size_in_kbytes  - 256K"
	lat_ctx -s 256K processes 2
	RESULT=$(( $RESULT + $? ));
	if [ $RESULT -eq 0 ] ; then
		test_print_result PASS lat_ctx
	else
		test_print_result FAIL lat_ctx
	fi	
	test_print_end lat_ctx
	update_result $RESULT 			

	test_print_start lat_ctx					
	test_print_trc " Parameters      : "
	test_print_trc " procs           - 4"
	test_print_trc " size_in_kbytes  - 128K"
	lat_ctx -s 128K processes 4
	RESULT=$(( $RESULT + $? ));
	if [ $RESULT -eq 0 ] ; then
		test_print_result PASS lat_ctx
	else
		test_print_result FAIL lat_ctx
	fi	
	test_print_end lat_ctx
	update_result $RESULT 				

	test_print_start lat_ctx					
	test_print_trc " Parameters      : "
	test_print_trc " procs           - 4"
	test_print_trc " size_in_kbytes  - 256K"
	lat_ctx -s 256K processes 4
	RESULT=$(( $RESULT + $? ));
	if [ $RESULT -eq 0 ] ; then
		test_print_result PASS lat_ctx
	else
		test_print_result FAIL lat_ctx
	fi	
	test_print_end lat_ctx
	update_result $RESULT 					

	test_print_trc " LATENCY OF COMMAND "
	test_print_start lat_cmd					
	test_print_trc " Parameters      : "
	test_print_trc " command         - ls"
#	lat_cmd ls
	RESULT=$(( $RESULT + $? ));
	if [ $RESULT -eq 0 ] ; then
		test_print_result PASS lat_cmd
	else
		test_print_result FAIL lat_cmd
	fi	
	test_print_end lat_cmd
	update_result $RESULT 						
	
	test_print_trc " LATENCY OF COMMAND "
	test_print_start lat_cmd					
	test_print_trc " Parameters      : "
	test_print_trc " command         - pwd"
#	lat_cmd pwd
	RESULT=$(( $RESULT + $? ));
	if [ $RESULT -eq 0 ] ; then
		test_print_result PASS lat_cmd
	else
		test_print_result FAIL lat_cmd
	fi	
	test_print_end lat_cmd
	update_result $RESULT 					
	
	test_print_trc " LATENCY OF MEMORY READ "
	test_print_start lat_mem_rd					
	test_print_trc " Parameters      : "
	test_print_trc " size_in_mbytes  - 1M"
	test_print_trc " stride          - 128"
	lat_mem_rd 1M 128
	RESULT=$(( $RESULT + $? ));
	if [ $RESULT -eq 0 ] ; then
		test_print_result PASS lat_mem_rd
	else
		test_print_result FAIL lat_mem_rd
	fi	
	test_print_end lat_mem_rd
	update_result $RESULT 						
	
	test_print_trc " LATENCY OF MAPPING AND UNMAPPING "
	test_print_start lat_mmap					
	test_print_trc " Parameters      : "
	test_print_trc " size			 - 1M"
	test_print_trc " file            - test1.txt"
	lat_mmap 1M test1.txt
	RESULT=$(( $RESULT + $? ));
	if [ $RESULT -eq 0 ] ; then
		test_print_result PASS lat_mmap
	else
		test_print_result FAIL lat_mmap
	fi	
	test_print_end lat_mmap
	update_result $RESULT 						
		

	test_print_trc " LATENCY OF BASIC CPU OPERATIONS "
	test_print_start lat_ops						
	test_print_trc " Parameters      : none"
	lat_ops
	RESULT=$(( $RESULT + $? ));
	if [ $RESULT -eq 0 ] ; then
		test_print_result PASS lat_ops
	else
		test_print_result FAIL lat_ops
	fi	
	test_print_end lat_ops
	update_result $RESULT 	

	test_print_trc " LATENCY OF PAGE FAULT "
	test_print_start lat_pagefault							
	test_print_trc " Parameters      : "
	test_print_trc " file      	     - test1.txt"
	lat_pagefault test1.txt
	RESULT=$(( $RESULT + $? ));
	if [ $RESULT -eq 0 ] ; then
		test_print_result PASS lat_pagefault
	else
		test_print_result FAIL lat_pagefault
	fi	
	test_print_end lat_pagefault
	update_result $RESULT 	

	test_print_trc " LATENCY INTERPROCESS COMMUNICATION THROUGH PIPES "
	test_print_start lat_pipe							
	test_print_trc " Parameters      : none"
	lat_pipe
	RESULT=$(( $RESULT + $? ));
	if [ $RESULT -eq 0 ] ; then
		test_print_result PASS lat_pipe
	else
		test_print_result FAIL lat_pipe
	fi	
	test_print_end lat_pipe
	update_result $RESULT 		

	test_print_trc " LATENCY OF PROCESS CREATION "
	test_print_start lat_proc							
	test_print_trc " Parameters      : "
	test_print_trc " type      	     - fork"	
	lat_proc fork
	RESULT=$(( $RESULT + $? ));
	if [ $RESULT -eq 0 ] ; then
		test_print_result PASS lat_proc
	else
		test_print_result FAIL lat_proc
	fi	
	test_print_end lat_proc
	update_result $RESULT 			
	
	test_print_start lat_proc							
	test_print_trc " Parameters      : "
	test_print_trc " type      	     - exec"	
	lat_proc exec
	RESULT=$(( $RESULT + $? ));
	if [ $RESULT -eq 0 ] ; then
		test_print_result PASS lat_proc
	else
		test_print_result FAIL lat_proc
	fi	
	test_print_end lat_proc
	update_result $RESULT 				

	test_print_start lat_proc							
	test_print_trc " Parameters      : "
	test_print_trc " type      	     - shell"	
	lat_proc shell
	RESULT=$(( $RESULT + $? ));
	if [ $RESULT -eq 0 ] ; then
		test_print_result PASS lat_proc
	else
		test_print_result FAIL lat_proc
	fi	
	test_print_end lat_proc
	update_result $RESULT 					

	test_print_start lat_proc							
	test_print_trc " Parameters      : "
	test_print_trc " type      	     - procedure"	
	lat_proc procedure
	RESULT=$(( $RESULT + $? ));
	if [ $RESULT -eq 0 ] ; then
		test_print_result PASS lat_proc
	else
		test_print_result FAIL lat_proc
	fi	
	test_print_end lat_proc
	update_result $RESULT 					
		
	test_print_trc " LATENCY OF SEMAPHORE "
	test_print_start lat_sem								
	test_print_trc " Parameters      : none"
	lat_sem
	RESULT=$(( $RESULT + $? ));
	if [ $RESULT -eq 0 ] ; then
		test_print_result PASS lat_sem
	else
		test_print_result FAIL lat_sem
	fi	
	test_print_end lat_sem
	update_result $RESULT 		

	test_print_trc " LATENCY OF INTERPROCESS COMMUNICATION VIA TCP/IP "
	test_print_start lat_tcp									
	test_print_trc " Parameters      : "
	test_print_trc " hostname        - localhost"
	lat_tcp localhost
	RESULT=$(( $RESULT + $? ));
	if [ $RESULT -eq 0 ] ; then
		test_print_result PASS lat_tcp
	else
		test_print_result FAIL lat_tcp
	fi	
	test_print_end lat_tcp
	update_result $RESULT 	
	
	test_print_trc " LATENCY OF INTERPROCESS COMMUNICATION VIA UDP "
	test_print_start lat_udp									
	test_print_trc " Parameters      : "
	test_print_trc " hostname        - localhost"
	lat_udp localhost
	RESULT=$(( $RESULT + $? ));
	if [ $RESULT -eq 0 ] ; then
		test_print_result PASS lat_udp
	else
		test_print_result FAIL lat_udp
	fi	
	test_print_end lat_udp
	update_result $RESULT 		


	test_print_trc " LATENCY OF PERFORM SELECT "
	test_print_start lat_select									
	test_print_trc " Parameters      : "
	test_print_trc " type      		 - tcp"
	lat_select tcp
	RESULT=$(( $RESULT + $? ));
	if [ $RESULT -eq 0 ] ; then
		test_print_result PASS lat_select
	else
		test_print_result FAIL lat_select
	fi	
	test_print_end lat_select
	update_result $RESULT 			
	
	test_print_trc " LATENCY TO INSTALL AND CATCH SIGNALS "
	test_print_start lat_sig										
	test_print_trc " Parameters      : "
	test_print_trc " type            - install"
	lat_sig  install
	RESULT=$(( $RESULT + $? ));
	if [ $RESULT -eq 0 ] ; then
		test_print_result PASS lat_sig
	else
		test_print_result FAIL lat_sig
	fi	
	test_print_end lat_sig
	update_result $RESULT 	

	test_print_start lat_sig										
	test_print_trc " Parameters      : "
	test_print_trc " type            - catch"
	lat_sig  catch
	RESULT=$(( $RESULT + $? ));
	if [ $RESULT -eq 0 ] ; then
		test_print_result PASS lat_sig
	else
		test_print_result FAIL lat_sig
	fi	
	test_print_end lat_sig
	update_result $RESULT 	
		
	test_print_start lat_sig										
	test_print_trc " Parameters      : "
	test_print_trc " type            - prot"
	lat_sig  prot test1.txt
	RESULT=$(( $RESULT + $? ));
	if [ $RESULT -eq 0 ] ; then
		test_print_result PASS lat_sig
	else
		test_print_result FAIL lat_sig
	fi	
	test_print_end lat_sig
	update_result $RESULT 	

	test_print_trc " LATENCY OF INTERPROCESS COMMUNICATION VIA UNIX SOCKETS "			
	test_print_start lat_unix										
	test_print_trc " Parameters      : none"
	lat_unix
	RESULT=$(( $RESULT + $? ));
	if [ $RESULT -eq 0 ] ; then
		test_print_result PASS lat_unix
	else
		test_print_result FAIL lat_unix
	fi	
	test_print_end lat_unix
	update_result $RESULT 		

	test_print_trc " LATENCY OF SYS CALL "
	test_print_start lat_syscall										
	test_print_trc " Parameters      : "
	test_print_trc " syscall         - fstat"
	test_print_trc " file            - test1.txt"
	lat_syscall fstat test1.txt
	RESULT=$(( $RESULT + $? ));
	if [ $RESULT -eq 0 ] ; then
		test_print_result PASS lat_syscall
	else
		test_print_result FAIL lat_syscall
	fi	
	test_print_end lat_syscall
	update_result $RESULT 			
	
	test_print_start lat_syscall										
	test_print_trc " Parameters      : "
	test_print_trc " syscall         - stat"
	test_print_trc " file            - test1.txt"
	lat_syscall stat test1.txt
	RESULT=$(( $RESULT + $? ));
	if [ $RESULT -eq 0 ] ; then
		test_print_result PASS lat_syscall
	else
		test_print_result FAIL lat_syscall
	fi	
	test_print_end lat_syscall
	update_result $RESULT 			
		
	test_print_start lat_syscall										
	test_print_trc " Parameters      : "
	test_print_trc " syscall         - open"
	test_print_trc " file            - test1.txt"
	lat_syscall open test1.txt
	RESULT=$(( $RESULT + $? ));
	if [ $RESULT -eq 0 ] ; then
		test_print_result PASS lat_syscall
	else
		test_print_result FAIL lat_syscall
	fi	
	test_print_end lat_syscall
	update_result $RESULT 			
		
	test_print_start lat_syscall										
	test_print_trc " Parameters      : "
	test_print_trc " syscall         - write"
	test_print_trc " file            - test1.txt"
	lat_syscall write test1.txt
	RESULT=$(( $RESULT + $? ));
	if [ $RESULT -eq 0 ] ; then
		test_print_result PASS lat_syscall
	else
		test_print_result FAIL lat_syscall
	fi	
	test_print_end lat_syscall
	update_result $RESULT 			
		
	test_print_start lat_syscall										
	test_print_trc " Parameters      : "
	test_print_trc " syscall         - read"
	test_print_trc " file            - test1.txt"
	lat_syscall read test1.txt
	RESULT=$(( $RESULT + $? ));
	if [ $RESULT -eq 0 ] ; then
		test_print_result PASS lat_syscall
	else
		test_print_result FAIL lat_syscall
	fi	
	test_print_end lat_syscall
	update_result $RESULT 				

	test_print_start lat_syscall										
	test_print_trc " Parameters      : "
	test_print_trc " syscall         - null"
	lat_syscall null
	RESULT=$(( $RESULT + $? ));
	if [ $RESULT -eq 0 ] ; then
		test_print_result PASS lat_syscall
	else
		test_print_result FAIL lat_syscall
	fi	
	test_print_end lat_syscall
	update_result $RESULT 				
		
test_print_trc " ***** EXITING THE LATENCY BENCHMARKING ***** "
test_print_trc " DELETING THE FILE "
rm test1.txt
test_print_trc " done."
test_print_trc ""

test_print_trc " ***** EXITING THE LMBENCH SCRIPT ***** "
}
# prints help options
help ()
{
      echo "--- HELP OPTIONS  ---"
      echo "$0 "
}
# parse the command line arguments

args=`getopt -u -a -o hv  --long version,help,id:,plat: --  "$@"`

# getopt fails, set help option
if [ $? -ne 0 ] ; then
        H="help";
fi

# update the refined command line argumetn list
eval set -- $args

# parse the command line argument and update the shell variables
# so that functions above can use them

for i in $@ ; do
case "$i" in

        -h) shift; H="help" ;;
        --help) shift; H="help" ;;
        --version) shift; V="version" ;;
        -v) shift; V="version" ;;
esac
done


# update the refined command line argumetn list once more
eval set -- $args

# call appropriate functions
if [ "$H" = "help" ] ;  then
        $H
elif [ "$V" =  "version" ] ; then
        test_print_version
else
        lmbench_tests
        print_summary
	if [ $FAIL_COUNT > 0 ]; then
	        exit $FAIL_COUNT
	else
        	exit 0
	fi
fi

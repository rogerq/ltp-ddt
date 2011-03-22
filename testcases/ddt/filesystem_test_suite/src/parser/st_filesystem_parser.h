/*
 * st_filesystem_parser.h
 *
 * This file contains command line option definions
 *
 * Copyright (C) 2009 Texas Instruments Incorporated - http://www.ti.com/
 *
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
*/  
    
#ifndef _ST_filesystem_PARSER_H_
#define _ST_filesystem_PARSER_H_
    
/*
 * Define Options here, so that it would be easy to add new features/options.
 */ 
    
/* Name of the file to read from or write to  */ 
#define OPTION_FILE			1
    
/* Total file size in MB */ 
#define OPTION_FILE_SIZE		2
    
/* application buffer size in bytes */ 
#define OPTION_BUFFER_SIZE		3
    
/*I/O mode is read */ 
#define OPTION_READ     		4
    
/* I/O mode is write */ 
#define OPTION_WRITE	        	5
    
/* Performance calculation */ 
#define OPTION_PERFORMANCE		6
    
/* Test case id string for logging purpose */ 
#define OPTION_TESTCASE_ID      	7

/* path of the file to copy from */
#define OPTION_SRC     			8

/* path of the file write to  */
#define OPTION_DST     			9

/* Duration in sec  */
#define OPTION_DURATION     		10

/* I/O mode is copy */
#define OPTION_COPY            		11


    
/* Displays the version of LFTB package */ 
#define OPTION_VERSION       	'v'
    
/* Displays the help */ 
#define OPTION_HELP        	'h'
    
#define CPULOAD "cpuload"
    
#define THROUGHPUT "throughput"
    
#endif	/* #ifndef _ST_filesystem_PARSER_H_ */
    

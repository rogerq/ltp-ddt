/*
 * Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation version 2.
 *
 * This program is distributed "as is" WITHOUT ANY WARRANTY of any
 * kind, whether express or implied; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
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

/* Total srcfile size in MB */ 
#define OPTION_SRCFILE_SIZE		12		
    
/* Displays the version of LFTB package */ 
#define OPTION_VERSION       	'v'
    
/* Displays the help */ 
#define OPTION_HELP        	'h'
    
#define CPULOAD "cpuload"
    
#define THROUGHPUT "throughput"
    
#endif	/* #ifndef _ST_filesystem_PARSER_H_ */
    

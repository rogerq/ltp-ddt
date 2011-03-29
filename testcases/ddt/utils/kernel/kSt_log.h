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

#ifndef _ST_LOG_H_
#define _ST_LOG_H_

#include <linux/delay.h>


#define PERFLOG(...) printk("%s:%s:%d:", __FILE__, __FUNCTION__, __LINE__); \
                     printk(__VA_ARGS__)

/* 
 *to enable START, END, TEST RESULT, WARNING and ERROR MACROS 
 */
#define ST_ENABLE_PRINT

/* 
 *this is not enabled by default - uncomment this to enable TRC 
 */

#define ST_ENABLE_TRC

/*
 *this is not enabled by default - uncomment this to enable DEBUG 
 */

/*
 *#define ST_ENABLE_DEBUG
 */


/* 
 *This is mandatory and is always enabled 
 */
#define TEST_PRINT(...)	printk(__VA_ARGS__); \
	                 	printk ("\n");\
	                 	ssleep(1);

/*
 *TEST Prints enabled by default 
 */
#ifdef ST_ENABLE_PRINT

/* Used to represent the beginning of the test case. The parameter to this
 * function call will be a macro representing the testcase description.
 * For eg: TEST_PRINT_TST_START(LINUXI2C0250) ; 
 * The test case id LINUXI2C0250 is a macro.
 */

#define TEST_PRINT_TST_START(...)	printk ("\n\n|TEST START|"); \
	                        		printk(__VA_ARGS__); \
	                        		printk ("|\n");\
									ssleep(1);


/* 
 *Enable the warning logs 
 */
#define TEST_PRINT_WRG(...)			printk ("\n\n|WARNING|"); \
                         			printk("Warning Line: %d, File: %s - ", \
                                	__LINE__, __FILE__); \
                         			printk(__VA_ARGS__); \
									printk("|\n");\
									ssleep(1);

/* 
 *Enable the error logs 
 */
#define TEST_PRINT_ERR(...)			printk ("\n\n|ERROR|"); \
               						printk("*ERROR* Line: %d, File: %s - ", \
	                            	__LINE__, __FILE__); \
	                     			printk(__VA_ARGS__); \
									printk("|\n");\
									ssleep(1)

/* Used to represent the PASS result of the test case. The parameter to this
 * function call will be a macro representing the testcase description and test
 * result.  
 * For eg: TEST_PRINT_TST_RESULT_PASS(LINUXI2C0250) ; 
 */
#define TEST_PRINT_TST_RESULT(result,...)	if (result)\
                                         		printk ("|TEST RESULT|FAIL|"); \
                                        	else\
                                            	printk ("|TEST RESULT|PASS|"); \
    	                                 	printk (__VA_ARGS__);\
		                                 	printk ("|\n");\
	    	                             	ssleep(1)

/* Used to represent the end of the test case. The parameter to this
 * function call will be a macro representing the testcase description.
 * For eg: TEST_PRINT_TST_END(LINUXI2C0250) ; 
 */
#define TEST_PRINT_TST_END(...)		printk ("|TEST END|"); \
							  		printk(__VA_ARGS__);\
	                        		printk ("|\n");\
	                        		ssleep(1)

#endif /*ST_ENABLE_PRINT */

#ifdef ST_ENABLE_TRC
#define TEST_PRINT_TRC(...)			printk ("|TRACE LOG|"); \
                        			printk(__VA_ARGS__);\
	                     			printk ("|\n");\
	                     			ssleep(1)
#else
#define TEST_PRINT_TRC(x)
#endif /*ST_ENABLE_TRC */

#ifdef ST_ENABLE_DEBUG
#define DEBUG_PRINT(...)			printk ("|DEBUG|"); \
                        			printk(__VA_ARGS__);\
	                     			printk ("|\n");\
	                     			ssleep(1)
#else
#define DEBUG_PRINT(...)
#endif /*ST_ENABLE_TRC */

#endif  /* _ST_LOG_H_ */

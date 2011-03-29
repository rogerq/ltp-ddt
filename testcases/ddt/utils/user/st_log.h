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

#include <st_defines.h>
#include <sys/time.h>

#define PERFLOG(...) printf("%s:%s:%d:", __FILE__, __FUNCTION__, __LINE__); \
                     printf(__VA_ARGS__)

/* to enable START, END, TEST RESULT, WARNING and ERROR MACROS */
#define ST_ENABLE_PRINT

/* this is not enabled by default - uncomment this to enable TRC0 */
#define ST_ENABLE_TRC

/* This is mandatory and is always enabled */
#define TEST_PRINT(...)		printf(__VA_ARGS__); \
				printf ("\n"); \
	                 	sleep(1);

/****************** TEST Prints enabled by default ************/
#ifdef ST_ENABLE_PRINT

/* Used to represent the beginning of the test case. The parameter to this
 * function call will be a macro representing the testcase description.
 * For eg: TEST_PRINT_TST_START(LINUXI2C0250) ;
 * The test case id LINUXI2C0250 is a macro.
 */

#define TEST_PRINT_TST_START(...)	printf ("\n\n|TEST START|"); \
					printf(__VA_ARGS__); \
					printf ("|\n"); \
					sleep(1);

/* Enable the warning logs */
#define TEST_PRINT_WRG(...)		printf ("\n\n|WARNING|"); \
					printf("Warning Line: %d, File: %s - ", \
						__LINE__, __FILE__); \
					printf(__VA_ARGS__); \
					printf("|\n"); \
					sleep(1);

/* Enable the error logs */
#define TEST_PRINT_ERR(...)		printf ("\n\n|ERROR|"); \
					printf("*ERROR* Line: %d, File: %s - ", \
						__LINE__, __FILE__); \
					printf(__VA_ARGS__); \
					printf("|\n"); \
					sleep(1);

/*
 * Used to represent the PASS result of the test case.
 * Arg:
 *	result - 0 = PASS, !0 (non-zero) = FAIL
 *	id - string respecting testcase id
 */
#define TEST_PRINT_TST_RESULT(result, id)	\
	if (result) \
		printf ("|TEST RESULT|FAIL|%s|", id); \
	else \
		printf ("|TEST RESULT|PASS|%s|", id); \
	printf ("\n"); \
	sleep(1);

/*
 * Used to represent the end of the test case.
 */
#define TEST_PRINT_TST_END(...)		printf ("|TEST END|"); \
					printf(__VA_ARGS__); \
					printf ("|\n");\
					sleep(1);

#endif /*ST_ENABLE_PRINT */

#ifdef ST_ENABLE_TRC
#define TEST_PRINT_TRC_NOSLEEP(...)	printf ("|TRACE LOG|"); \
					printf(__VA_ARGS__);\
					printf ("|\n");

#define TEST_PRINT_TRC(...)		printf ("|TRACE LOG|"); \
					printf(__VA_ARGS__);\
					printf ("|\n");\
					usleep(100);
#else
#define TEST_PRINT_TRC(x)
#endif	/*ST_ENABLE_TRC */

#endif	/* _ST_LOG_H_ */

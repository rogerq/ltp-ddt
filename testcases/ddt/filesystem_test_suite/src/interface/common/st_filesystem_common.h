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

#ifndef _ST_FILESYSTEM_COMMON_H_
#define _ST_FILESYSTEM_COMMON_H_

/*Generic header files */
#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <fcntl.h>

/*Testcode related Header files*/
#include <st_defines.h>
#include <st_log.h>

/*Structure for holding the test options */
struct st_filesystem_testparams {
	/* i/o direction read/write */
	char iomode;
	/* Name of the file to read from or write to */
	char *filename;
	/* Buffer size */
	int buffer_size;
	/* File size */
	float file_size;
	/* srcFile size */
	float srcfile_size;
	/* Test name */
	char *test_name;
	/* throughput enable flag */
	int throughput_flag;
	/* cpuload enable flag */
	int cpuload_flag;
        /* Path of the file to read for copy */
        char *src;
        /* Path of the file write to for copy*/
        char *dst;
	/* duration */
	int duration;
};
void st_perror(const char *s);
Int32 st_read(IN Int32 fd, IN void *buf, IN Int32 count);
Int32 st_write(IN Int32 fd, IN const void *buf, IN Int32 count);
Int32 st_open(IN const Int8 * pathname, IN Int32 oflag);
Int32 st_close(IN Int32 fd);
void st_init_filesystem_test_params();
void st_display_filesystem_test_suite_help(void);
void st_print_filesystem_test_params(struct st_filesystem_testparams
				     *testoptions, char *test_id);
int st_filesystem_performance_read_test(struct st_filesystem_testparams *info,
					char *test_id);
int st_filesystem_performance_write_test(struct st_filesystem_testparams *info,
					 char *test_id);
int st_filesystem_performance_copy_test(struct st_filesystem_testparams *info,
                                         char *test_id);

#endif /*ST_FILESYSTEM_COMMON_H_ */

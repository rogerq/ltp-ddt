/*
 * st_i2c_common.h
 *
 * This file contains common definions
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

#ifndef _ST_I2C_COMMON_H_
#define _ST_I2C_COMMON_H_

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
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <limits.h>

/*Testcode related Header files*/
#include <st_defines.h>
#include <st_log.h>
#include <st_timer.h>

/*
 *Structure for holding the test options 
 */
struct st_i2c_testparams {
	/* i/o direction read/write */
	char iomode;
	/* i2c module Device node */
	char *device_node;
	/* Device to write or read from: */
	int device;
	/* hardware device i2c address */
	unsigned int address;
	/* Buffer size */
	unsigned int buffer_size;
	/* File size */
	unsigned int file_size;
	/* Page size */
	unsigned int page_size;
	/* Test name */
	char *test_name;
	/* throughput enable flag */
	int throughput_flag;
	/* cpuload enable flag */
	int cpuload_flag;

};

void st_display_i2c_test_suite_help(void);

void st_print_i2c_test_params(struct st_i2c_testparams *testoptions,
			      char *test_id);

void st_init_i2c_test_params(void);

int st_i2c_eeprom_read_test(struct st_i2c_testparams *info, char *test_id);

int st_i2c_eeprom_write_test(struct st_i2c_testparams *info, char *test_id);

#endif /*ST_I2C_COMMON_H_ */

/*
 * st_filesystem_read_from_file.c
 *
 * This file contains testcase for read from a file
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

#include <sys/time.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

/* Include package headers here */
#include <st_timer.h>
#include <st_log.h>
#include <st_cpu_load.h>
#include "st_filesystem_common.h"

/*
 * Function		- st_filesystem_performance_read_test 
 * Functionality	- This function recieves the test params and read 
 * 					from the file
 * Input Params	-  info,test_id
 * Return Value	-  0: SUCCESS, -1: FAILURE
 * Note			-  None
 */
int st_filesystem_performance_read_test(struct st_filesystem_testparams *info,
					char *test_id)
{
	int fdes = 0;
	int result = SUCCESS;
	int res_close = SUCCESS;
	char *buff_ptr = NULL;
	char *file_ptr = NULL;
	int i = 0;
	int read_ret = 0;
	int bsize = 0;
	int totalsize = 0;
	int loopcount = 0;
	int remainder = 0;
	int totbytread = 0;
	struct timeval start_time;
	unsigned long elapsed_usecs = 0;
	struct proc_stat cpu_status_id;
	float percentage_cpu_load = 0;

	file_ptr = info->filename;
	totalsize = info->file_size * 1024 * 1024;
	bsize = info->buffer_size;

	loopcount = totalsize / bsize;
	remainder = totalsize % bsize;
	/* Allocate memory for the buff_ptr, size = bsize */
	buff_ptr = (char *)malloc(bsize * (sizeof(char)));
	if (NULL == buff_ptr) {
		perror("\n malloc");
		result = FAILURE;
		goto end;
	}

	/* Perform the read operation */
	fdes = open((const char *)file_ptr, O_RDONLY);
	if (-1 == fdes) {
		perror("\n open");
		TEST_PRINT_ERR("file open failed ");
		result = FAILURE;
		goto free_mem;
	}

	if (info->cpuload_flag) {
		/* Start CPU Load calcaulation */
		start_cpuload_measurement(&cpu_status_id);
	}
	if (info->throughput_flag) {
		/* Start Timer */
		start_timer(&start_time);
	}

	for (i = 0; i < loopcount; i++) {
		read_ret = read(fdes, buff_ptr, bsize);
		totbytread = totbytread + read_ret;
		if (bsize != read_ret) {
			perror("\n read");
			TEST_PRINT_ERR("file read failed ");
			result = FAILURE;
			goto close_file;
		}
	}

	if (remainder) {
		read_ret = read(fdes, buff_ptr, remainder);
		totbytread = totbytread + read_ret;
		if (remainder != read_ret) {
			perror("\n read");
			TEST_PRINT_ERR("file read failed ");
			result = FAILURE;
			goto close_file;
		}
	}
      close_file:
	res_close = fsync(fdes);
	if (-1 == res_close) {
		perror("\n fsync");
		TEST_PRINT_ERR("file fsync failed ");
		result = FAILURE;
	}

	if (info->throughput_flag && result == SUCCESS) {
		/* Stop the Timer and get the usecs elapsed */
		elapsed_usecs = stop_timer(&start_time);
		TEST_PRINT_TRC("fileread | Durartion in usecs | %ld",
			       elapsed_usecs);
		TEST_PRINT_TRC("fileread | Mega Bytes/Sec | %lf",
			       (float)(((float)totalsize /
					(float)elapsed_usecs)));
	}
	if (info->cpuload_flag && result == SUCCESS) {

		/* Get CPU Load figures */
		percentage_cpu_load = stop_cpuload_measurement(&cpu_status_id);
		if ((percentage_cpu_load >= 0) && (percentage_cpu_load <= 100))
			TEST_PRINT_TRC
			    ("fileread | percentage cpu load | %.2f%%",
			     percentage_cpu_load);
	}
	res_close = close(fdes);
	if (-1 == res_close) {
		perror("\n close");
		TEST_PRINT_ERR("file close failed ");
		result = FAILURE;
	}


      free_mem:
	/* Free  memory for the buff_ptr */
	if (NULL != buff_ptr) {
		free(buff_ptr);
	}

      end:
	return result;
}

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

/* Include standard headers here */
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
 * Function		- st_filesystem_performance_write_test 
 * Functionality	- This function recieves the test params and write 
 * 					to the file
 * Input Params	-  info,test_id
 * Return Value	-  0: SUCCESS, -1: FAILURE
 * Note			-  None
 */

int st_filesystem_performance_write_test(struct st_filesystem_testparams *info,
					 char *test_id)
{
	int fdes = 0;
	int srcfdes = 0;
	char *buff_ptr = NULL;
        char *srcfile_ptr = NULL;
	char *file_ptr = NULL;
	int result = SUCCESS;
	int res_close = SUCCESS;
	int read_ret = 0;
	int write_ret = 0;
	int i = 0;
	int j = 0;
	int bsize = 0;
	int totalsize = 0;
	int srcfile_size = 0;
	int loopcount = 0;
	int remainder = 0;
	int totbytwrite = 0;
	struct timeval start_time;
	unsigned long elapsed_usecs = 0;
	struct proc_stat cpu_status_id;
	float percentage_cpu_load = 0;
	float throughput = 0;

	file_ptr = info->filename;
        srcfile_ptr = info->src;
	totalsize = info->file_size * 1024 * 1024;
	srcfile_size = info->srcfile_size * 1024 * 1024;
	//srcfile_size = 10 * 1024 * 1024;
	bsize = info->buffer_size;

	if ((totalsize % srcfile_size) != 0) {
		TEST_PRINT_ERR("totalsize is not divided by srcfile_size. ");
		result = FAILURE;
		goto end;
	}
	loopcount = srcfile_size / bsize;
	remainder = srcfile_size % bsize;

	srcfdes = open((const char *)srcfile_ptr, O_RDONLY);
	if (-1 == srcfdes) {
		perror("\n open");
		TEST_PRINT_ERR("src file open failed ");
		result = FAILURE;
		//srcfileopenflag = 1;
		goto end;
	}

	//map srcfile
        buff_ptr = mmap(NULL, srcfile_size, PROT_READ, MAP_PRIVATE, srcfdes, 0);
	if((void*)buff_ptr == (void*) -1) {
		printf("MMAP failed\n");
		goto end;
	}
	TEST_PRINT_TRC("mmap srcfile ");

	fdes = open((const char *)file_ptr, O_WRONLY | O_CREAT);
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

	for (j = 0; j < totalsize/srcfile_size; j++){

	for (i = 0; i < loopcount; i++) {
		write_ret = write(fdes, buff_ptr+i*bsize, bsize);
		totbytwrite = totbytwrite + write_ret;
		if (bsize != write_ret) {
			perror("\n write");
			TEST_PRINT_ERR("file write failed ");
			result = FAILURE;
			goto close_file;

		}
	}

	if (remainder) {
		write_ret = write(fdes, buff_ptr+loopcount*bsize, remainder);
		totbytwrite = totbytwrite + write_ret;
		if (remainder != write_ret) {
			perror("\n write");
			TEST_PRINT_ERR("file write failed ");
			result = FAILURE;
			goto close_file;

		}
	}
	}
      close_file:
	res_close = fsync(fdes);
	if (-1 == res_close) {
		perror("\n fsynce");
		TEST_PRINT_ERR("file fsync failed ");
		result = FAILURE;

	}

	if (info->throughput_flag) {
		/* Stop the Timer and get the usecs elapsed */
		elapsed_usecs = stop_timer(&start_time);
		throughput = (float)(((float)totalsize / (float)elapsed_usecs));
		TEST_PRINT_TRC("filewrite | Durartion in usecs | %ld",
			       elapsed_usecs);
		TEST_PRINT_TRC("filewrite | Mega Bytes/Sec | %lf",
				throughput);
	}
	if (info->cpuload_flag) {
		/* Get CPU Load figures */
		percentage_cpu_load = stop_cpuload_measurement(&cpu_status_id);
		if ((percentage_cpu_load >= 0) && (percentage_cpu_load <= 100))
			TEST_PRINT_TRC
			    ("filewrite | percentage cpu load | %.2f%%",
			     percentage_cpu_load);
	}
	res_close = close(fdes);
	if (-1 == res_close) {
		perror("\n close");
		TEST_PRINT_ERR("file close failed ");
		result = FAILURE;

	}

	TEST_PRINT_TRC("|PERFDATA|bsize:%d|iomode:write|throughput:%.2lfMB/S|cpuload:%.2f%%|", bsize, throughput, percentage_cpu_load); 

      free_mem:
	munmap(buff_ptr, srcfile_size);

      end:

	return result;
}

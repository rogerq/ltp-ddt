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

 /*Function to compare two files for data integrity */
int st_filesystem_dateintegrity_test(char *file1, char *file2);
/*
 * Function		- st_filesystem_performance_copy_test 
 * Functionality	- This function recieves the test params and copies 
 * 				from src to dst
 * Input Params	-  info,test_id
 * Return Value	-  0: SUCCESS, -1: FAILURE
 * Note			-  None
 */
int st_filesystem_performance_copy_test(struct st_filesystem_testparams *info,
					 char *test_id)
{

	int srcfdes = 0;
	int dstfdes = 0;
	int result = SUCCESS;
	int res_close = SUCCESS;
	char *buff_ptr = NULL;
	char *srcfile_ptr = NULL;
	char *dstfile_ptr = NULL;
	int i = 0;
	int read_ret = 0;
	int write_ret = 0;
	int bsize = 0;
	int totalsize = 0;
	int loopcount = 0;
	int remainder = 0;
	int dstfileopenflag = 0;
	int srcfileopenflag = 0;
	struct timeval duration_starttime;
	struct timeval throughput_starttime;
	unsigned long elapsed_usecs = 0;
	unsigned long elapsed_secs = 0;
	float throughput = 0;
	int throughputcnt = 0;
	int res_integrity = 0;
	int copycnt = 0;

	srcfile_ptr = info->src;
	dstfile_ptr = info->dst;
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
	/* Get the currnt time for the duration */
	start_timer(&duration_starttime);

	while(elapsed_secs < info->duration)
	{
		/* Open the files  for copy operation */
		srcfdes = open((const char *)srcfile_ptr, O_RDONLY);
		if (-1 == srcfdes) {
			perror("\n open");
			TEST_PRINT_ERR("src file open failed ");
			result = FAILURE;
			srcfileopenflag = 1;
			goto free_mem;
		}
		dstfdes = open((const char *)dstfile_ptr, (O_WRONLY | O_CREAT));
		if (-1 == dstfdes) {
			perror("\n open");
			TEST_PRINT_ERR("dst file open failed ");
			result = FAILURE;
			dstfileopenflag = 1;
			goto free_mem;
		}
		if (info->throughput_flag) {
			/* Get the currnt time for the throughput  */
			start_timer(&throughput_starttime);
		}
		for (i = 0; i < loopcount; i++) {
			read_ret = read(srcfdes, buff_ptr, bsize);
			if (bsize != read_ret) {
				perror("\n read");
				TEST_PRINT_ERR("file read failed ");
				result = FAILURE;
				goto free_mem;	
			}
			write_ret = write(dstfdes, buff_ptr, bsize);
			if (bsize != write_ret) {
				perror("\n write");
				TEST_PRINT_ERR("file write failed ");
				result = FAILURE;
				goto free_mem;
			}
		}
		if(remainder) {
			read_ret = read(srcfdes, buff_ptr, remainder);
			if (remainder != read_ret) {
				perror("\n read");
				TEST_PRINT_ERR("file read failed ");
				result = FAILURE;
				goto free_mem;
			}
			write_ret = write(dstfdes, buff_ptr, remainder);
			if (remainder != write_ret) {
				perror("\n write");
				TEST_PRINT_ERR("file write failed ");
				result = FAILURE;
				goto free_mem;
			}	
		}
		res_close = fsync(srcfdes);
		if (-1 == res_close) {
			perror("\n fsync");
			TEST_PRINT_ERR("file fsync failed ");
			result = FAILURE;
			goto free_mem;
		}
                res_close = fsync(dstfdes);
                if (-1 == res_close) {
                        perror("\n fsync");
                        TEST_PRINT_ERR("file fsync failed ");
                        result = FAILURE;
                        goto free_mem;
                }
		if (info->throughput_flag) {
			/* Stop the Timer and get the usecs elapsed */
			elapsed_usecs = stop_timer(&throughput_starttime);
			throughput += (float)((float)totalsize /(float)elapsed_usecs);
			throughputcnt++;
		}	
	        res_close = close(srcfdes);
	        if (-1 == res_close) {
	                perror("\n close");
	                TEST_PRINT_ERR("file close failed ");
	                result = FAILURE;
			goto free_mem;
	        }
		srcfileopenflag = 0;
	        res_close = close(dstfdes);
	        if (-1 == res_close) {
	                perror("\n close");
	                TEST_PRINT_ERR("file close failed ");
	                result = FAILURE;
			goto free_mem;
	        }
		copycnt++;
		TEST_PRINT_TRC("Copying of file  succeeded for %d Time",copycnt);
		dstfileopenflag = 0;
		res_integrity = st_filesystem_dateintegrity_test(srcfile_ptr,dstfile_ptr);
		if( SUCCESS != res_integrity ) {
			TEST_PRINT_ERR("Data integrity test failed ");
			result = FAILURE;
			goto free_mem;
		}	
		TEST_PRINT_TRC("Data integrity succeeded");
		/* Stop the Timer and get the secs elapsed for duration */		
		elapsed_usecs = stop_timer(&duration_starttime);
		elapsed_secs  = (elapsed_usecs/1000000);		
	}
	TEST_PRINT_TRC("Time Out ... Elapsed %lu Secs",elapsed_secs);
	if (info->throughput_flag) {
		/* Calculate the average throughput */
		TEST_PRINT_TRC("filecopy | Mega Bytes/Sec | %lf",
				(float)(throughput/(float)(throughputcnt)));
	}
	TEST_PRINT_TRC("|PERFDATA|bsize:%d|iomode:copy|throughput:%.2lfMB/S|", bsize, (float)(throughput/(float)(throughputcnt)));
	
free_mem:
	if(dstfileopenflag) {
		res_close = close(dstfdes);
		if (-1 == res_close) {
			perror("\n close");
			TEST_PRINT_ERR("file close failed ");
			result = FAILURE;
		}
	}
	if(srcfileopenflag) {
	        res_close = close(srcfdes);
	        if (-1 == res_close) {
	                perror("\n close");
	                TEST_PRINT_ERR("file close failed ");
	                result = FAILURE;
	        }
	}
	if (NULL != buff_ptr) {
		free(buff_ptr);
	}
end:
	return result;
}

/*
 * Function		- st_filesystem_dateintegrity_test
 * Functionality	- This function compares the srcfile and dstfile 
				for data integrity.
 * Input Params		- file1 and file2 files for compare
 * Return Value		- SUCCESS / FAILURE
 * Note			- None
 */
int st_filesystem_dateintegrity_test(char *file1, char *file2)
{
	FILE *srcfdes = NULL;
	FILE *dstfdes = NULL;
	int result = SUCCESS;
	int res_close = SUCCESS;
	int ch1 = 0;
	int ch2 = 0;


	/* Open the files  for data integrity */
	srcfdes = fopen((const char *)file1, "r");
	if (NULL == srcfdes) {
		perror("\n open");
		TEST_PRINT_ERR("Data Integrity : Src file open failed ");
		result = FAILURE;
		goto endfunc;
	}
	dstfdes = fopen((const char *)file2, "r");
	if (NULL == dstfdes) {
		perror("\n open");
		TEST_PRINT_ERR("Data Integrity : Dst file open failed ");
		result = FAILURE;
		goto closefile1;
	}
	while( ch1 != EOF || ch2!= EOF) {
		ch1 = fgetc(srcfdes);
		ch2 = fgetc(dstfdes);
		if( ch1 != ch2 ) {
			result = FAILURE;
			break;
		}		
	}
	res_close = fclose(dstfdes);
	if (0 != res_close) {
		perror("\n close");
		TEST_PRINT_ERR("file close failed ");
		result = FAILURE;
	}
closefile1:
	res_close = fclose(srcfdes);
	if (0 != res_close) {
		perror("\n close");
		TEST_PRINT_ERR("file close failed ");
		result = FAILURE;
	}
endfunc:
	
	return result;	
}


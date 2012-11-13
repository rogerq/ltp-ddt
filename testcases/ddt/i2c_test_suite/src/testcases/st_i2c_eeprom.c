/*
 * st_i2c_eeprom.c
 *
 * This file implements the i2c Write Throughput test case
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

/*******************************************************************************/
#include <sys/time.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
/*
 *Include package headers here 
 */
#include <st_timer.h>
#include <st_log.h>
#include <st_cpu_load.h>
#include "st_i2c_common.h"

#define ADDR_SIZE			 	2
#define BYTE_TO_BIT_CONV        8
#define BIT_TO_KBIT_CONV        1024
#define DELAY_MICROSECS 		4000
#define ADDR_MSB				0x00
#define ADDR_LSB				0x00

/*
 * Function             - st_i2c_eeprom_read_test 
 * Functionality        - This function recieves the test params and write 
 *                        to the eeprom
 * Input Params         -  info,test_id
 * Return Value         -  0: SUCCESS, -1: FAILURE
 * Note                 -  None
 */
int st_i2c_eeprom_read_test(struct st_i2c_testparams *info, char *test_id)
{
	int fdes = 0;
	int result = SUCCESS;
	int ret = SUCCESS;
	char *file_ptr = NULL;
	unsigned int i = 0;
	unsigned int bsize = 0;
	unsigned int totalsize = 0;
	unsigned int loopcount = 0;
	unsigned int remainder = 0;
	int count = 0;
	struct timeval start_time;
	unsigned long elapsed_usecs = 0;
	struct proc_stat cpu_status_id;
	float percentage_cpu_load = 0;
	unsigned long actual_time = 0;
	float calc;
	short int page_no;
	struct i2c_msg msgs[2];
	struct i2c_rdwr_ioctl_data data;
	unsigned char bufAddr[ADDR_SIZE];
	unsigned char *buf = NULL;
	int i_res;
	file_ptr = info->device_node;
	totalsize = info->file_size;
	bsize = info->buffer_size;
	loopcount = totalsize / bsize;
	remainder = totalsize % bsize;
	buf = (unsigned char *)malloc(bsize * (sizeof(char)));
	if (NULL == buf) {
		perror("\n malloc");
		TEST_PRINT_ERR("Mem allocation failed ");
		result = FAILURE;
		goto end;
	}
	for (i = 0u; i < bsize; i++) {
		buf[i] = 'A' + (count % 26);
		count++;
	}
	fdes = open((const char *)file_ptr, O_RDWR);
	if (-1 == fdes) {
		perror("\n open");
		TEST_PRINT_ERR("file open failed ");
		result = FAILURE;
		goto free_mem;
	}

	/* 
	 *First two bytes should be the eeprom register location 
	 */
	bufAddr[0] = ADDR_MSB;
	bufAddr[1] = ADDR_LSB;
	msgs[0].addr = info->address;
	msgs[0].flags = 0;
	msgs[0].len = ADDR_SIZE;
	msgs[0].buf = bufAddr;
	msgs[1].addr = info->address;
	msgs[1].flags = I2C_M_RD;
	msgs[1].len = info->buffer_size;
	msgs[1].buf = buf;
	data.msgs = msgs;
	data.nmsgs = 2;
	if (info->cpuload_flag) {
		start_cpuload_measurement(&cpu_status_id);
	}
	if (info->throughput_flag) {
		start_timer(&start_time);
	}
	for (i = 0; i < loopcount; i++) {
		i_res = ioctl(fdes, I2C_RDWR, &data);
		if (i_res < 0) {
			perror("ioctl");
			TEST_PRINT_ERR("ioctl failed ");
			result = FAILURE;
			goto close_file;
		}
		page_no = bufAddr[1] | (bufAddr[0] << 8);
		page_no += bsize;
		bufAddr[0] = (page_no >> 8) & 0xFF;
		bufAddr[1] = page_no & 0xFF;
		usleep(DELAY_MICROSECS);
	}
	if (remainder) {
		msgs[1].len = remainder;
		i_res = ioctl(fdes, I2C_RDWR, &data);
		if (i_res < 0) {
			perror("ioctl");
			TEST_PRINT_ERR("ioctl failed ");
			result = FAILURE;
		}
	}
      close_file:
	ret = close(fdes);
	if (-1 == ret) {
		perror("\n close");
		TEST_PRINT_ERR("file close failed ");
		result = FAILURE;
	}
	if (info->throughput_flag && result == SUCCESS) {
		elapsed_usecs = stop_timer(&start_time);
		actual_time = (elapsed_usecs - (loopcount * DELAY_MICROSECS));
		calc =
		    (((float)(totalsize) * 1000000) / (float)(elapsed_usecs));
		calc = (calc * BYTE_TO_BIT_CONV) / BIT_TO_KBIT_CONV;

		TEST_PRINT_TRC("I2C | Read | Duration in uSec: %ld",
			       actual_time);
		TEST_PRINT_TRC("I2C | Read | Kbits/Sec: %lf", calc);
	}
	if (info->cpuload_flag && result == SUCCESS) {
		percentage_cpu_load = stop_cpuload_measurement(&cpu_status_id);
		if ((percentage_cpu_load >= 0) && (percentage_cpu_load <= 100))
			TEST_PRINT_TRC
			    ("I2C | Read | percentage cpu load | %.2f%%",
			     percentage_cpu_load);
	}
  TEST_PRINT_TRC("|PERFDATA|bsize:%d|iomode:read|throughput:%.2lfKbits/S|cpuload:%.2f%%|", bsize, calc, percentage_cpu_load);
  free_mem:
	if (NULL != buf) {
		free(buf);
	}
      end:
	return result;
}

/*
 * Function             - st_i2c_eeprom_write_test 
 * Functionality        - This function recieves the test params and write 
 *                        to the eeprom
 * Input Params         -  info,test_id
 * Return Value         -  0: SUCCESS, -1: FAILURE
 * Note                 -  None
 */
int st_i2c_eeprom_write_test(struct st_i2c_testparams *info, char *test_id)
{
	int fdes = 0;
	int result = SUCCESS;
	int ret = SUCCESS;
	int addr_fmt = 0;
	int slave_addr = info->address;
	char *write_buf = NULL;
	unsigned char *read_buf = NULL;
	char *file_ptr = NULL;
	int write_ret = 0;
	unsigned int i = 0, j = 0;
	unsigned int bsize = 0;
	unsigned int totalsize = 0;
	unsigned int alloc_size = 0;
	unsigned int loopcount = 0;
	unsigned int remainder = 0;
	int totbytwrite = 0;
	int count = 0;
	struct timeval start_time;
	unsigned long elapsed_usecs = 0;
	struct proc_stat cpu_status_id;
	float percentage_cpu_load = 0;
	unsigned long actual_time = 0;
	float calc;
	short int page_no;
	unsigned char bufAddr[ADDR_SIZE];
	struct i2c_msg msgs[2];
	struct i2c_rdwr_ioctl_data data;
	int i_res;
	file_ptr = info->device_node;
	totalsize = info->file_size;
	if (info->buffer_size > info->page_size) {
		TEST_PRINT_TRC("buffer size should be less than page size");
		TEST_PRINT_TRC("buffer size requested 	| %d",
			       info->buffer_size);
		TEST_PRINT_TRC("buffer size used 		| %d",
			       info->page_size);
		info->buffer_size = info->page_size;
	}
	alloc_size = info->buffer_size + ADDR_SIZE;
	bsize = info->buffer_size;
	loopcount = totalsize / bsize;
	remainder = totalsize % bsize;
	write_buf = (char *)malloc(alloc_size * (sizeof(char)));
	if (NULL == write_buf) {
		perror("\n malloc");
		TEST_PRINT_ERR("Mem allocation failed ");
		result = FAILURE;
		goto end;
	}
	read_buf = (unsigned char *)malloc(bsize * (sizeof(char)));
	if (NULL == read_buf) {
		perror("\n malloc");
		TEST_PRINT_ERR("Mem allocation failed ");
		result = FAILURE;
		goto free_writebuf;
	}

	/* 
	 *Initalize source and destination buffers 
	 *First two bytes should be the eeprom register location 
	 */
	write_buf[0] = ADDR_MSB;
	write_buf[1] = ADDR_LSB;
	for (i = 2u; i < alloc_size; i++) {
		write_buf[i] = 'A' + (count % 26);
		count++;
	}
	fdes = open((const char *)file_ptr, O_RDWR);
	if (-1 == fdes) {
		perror("\n open");
		TEST_PRINT_ERR("file open failed ");
		result = FAILURE;
		goto free_readbuf;
	}
	ret = ioctl(fdes, I2C_TENBIT, addr_fmt);
	if (ret < 0) {
		perror("ioctl");
		TEST_PRINT_ERR("ioctl failed ");
		result = FAILURE;
		goto close_file;
	}
	ret = ioctl(fdes, I2C_SLAVE, slave_addr);
	if (ret < 0) {
		perror("ioctl");
		TEST_PRINT_ERR("ioctl failed ");
		result = FAILURE;
		goto close_file;
	}
	if (info->cpuload_flag) {
		start_cpuload_measurement(&cpu_status_id);
	}
	if (info->throughput_flag) {
		start_timer(&start_time);
	}
	for (i = 0; i < loopcount; i++) {
		write_ret = write(fdes, write_buf, alloc_size);
		totbytwrite = totbytwrite + write_ret;
		if (alloc_size != write_ret) {
			perror("write");
			TEST_PRINT_ERR("write failed ");
			result = FAILURE;
			goto close_file;
		}
		page_no = write_buf[1] | (write_buf[0] << 8);
		page_no += bsize;
		write_buf[0] = (page_no >> 8) & 0xFF;
		write_buf[1] = page_no & 0xFF;
		usleep(DELAY_MICROSECS);
	}
	if (remainder) {
		remainder += ADDR_SIZE;
		write_ret = write(fdes, write_buf, remainder);
		totbytwrite = totbytwrite + write_ret;
		if (remainder != write_ret) {
			perror("write");
			TEST_PRINT_ERR("write failed ");
			result = FAILURE;
			goto close_file;
		}
	}
	if (info->throughput_flag && result == SUCCESS) {
		elapsed_usecs = stop_timer(&start_time);
		actual_time = (elapsed_usecs - (loopcount * DELAY_MICROSECS));
		calc =
		    (((float)(totalsize) * 1000000) / (float)(elapsed_usecs));
		calc = (calc * BYTE_TO_BIT_CONV) / BIT_TO_KBIT_CONV;
	}
	if (info->cpuload_flag && result == SUCCESS) {
		percentage_cpu_load = stop_cpuload_measurement(&cpu_status_id);
	}
	/* 
	 *Check the data integrity -- read and compare 
	 * First two bytes should be the eeprom register location 
	 */
	TEST_PRINT_TRC(" ---checking the data integrity ---");
	bufAddr[0] = ADDR_MSB;
	bufAddr[1] = ADDR_LSB;
	msgs[0].addr = info->address;
	msgs[0].flags = 0;
	msgs[0].len = ADDR_SIZE;
	msgs[0].buf = bufAddr;
	msgs[1].addr = info->address;
	msgs[1].flags = I2C_M_RD;
	msgs[1].len = info->buffer_size;
	msgs[1].buf = read_buf;
	data.msgs = msgs;
	data.nmsgs = 2;
	usleep(DELAY_MICROSECS);
	for (i = 0; i < loopcount; i++) {
		i_res = ioctl(fdes, I2C_RDWR, &data);
		if (i_res < 0) {
			perror("ioctl");
			TEST_PRINT_ERR("read failed ");
			result = FAILURE;
			goto close_file;
		}
		page_no = bufAddr[1] | (bufAddr[0] << 8);
		page_no += bsize;
		bufAddr[0] = (page_no >> 8) & 0xFF;
		bufAddr[1] = page_no & 0xFF;
		usleep(DELAY_MICROSECS);
		for (j = 0; j < info->buffer_size; j++) {
			if (read_buf[j] != write_buf[j + 2]) {
				TEST_PRINT_ERR(" I2C | Write |: "
					       "Data write-read matching failed at %u from the start location",
					       ((i * bsize) + j));
				result = FAILURE;
				goto close_file;
			}
		}
		if (j == info->buffer_size) {
			result = SUCCESS;
		}
	}
	if (remainder) {
		msgs[1].len = remainder;
		i_res = ioctl(fdes, I2C_RDWR, &data);
		if (i_res < 0) {
			perror("ioctl");
			TEST_PRINT_ERR("ioctl failed ");
			result = FAILURE;
			goto close_file;
		}
		for (j = 0; j < remainder; j++) {
			if (read_buf[j] != write_buf[j + 2]) {
				TEST_PRINT_ERR(" I2C | Write |: "
					       "Data write-read matching failed at %u from the start location",
					       ((bsize * loopcount) + j));
				result = FAILURE;
				goto close_file;
			}
		}
		if (j == remainder) {
			result = SUCCESS;
		}
	}
      close_file:
	ret = close(fdes);
	if (-1 == ret) {
		TEST_PRINT_ERR("file close failed ");
		result = FAILURE;
	}
	if (info->throughput_flag && result == SUCCESS) {
		TEST_PRINT_TRC("I2C | Write | Duration in uSec: %ld",
			       actual_time);
		TEST_PRINT_TRC("I2C | Write | Kbits/Sec: %lf", calc);
	}
	if (info->cpuload_flag && result == SUCCESS) {
		if ((percentage_cpu_load >= 0) && (percentage_cpu_load <= 100)) {
			TEST_PRINT_TRC
			    ("filewrite | percentage cpu load | %.2f%%",
			     percentage_cpu_load);
		}
	}
  TEST_PRINT_TRC("|PERFDATA|bsize:%d|iomode:write|throughput:%.2lfKbits/S|cpuload:%.2f%%|", bsize, calc, percentage_cpu_load);
  free_readbuf:
	if (NULL != read_buf) {
		free(read_buf);
	}
  free_writebuf:
	if (NULL != write_buf) {
		free(write_buf);
	}
      end:
	return result;
}

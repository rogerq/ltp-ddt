/*
 * st_edma_testcase.c
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
/* Include package headers here */
#include <st_timer.h>
#include <st_log.h>
#include <st_cpu_load.h>
#include "st_edma_common.h"

#define ST_edma_WRITEBUFF_LEN 1

/*
 * Function             - st_edma_ioctl_test 
 * Functionality        - This function recieves the test params and calls 
 *                        the appropariate ioctl and prints the status
 *                        accordingly
 * Input Params         -  info,test_id
 * Return Value         -  0: SUCCESS, -1: FAILURE
 * Note                 -  None
 */
int st_edma_ioctl_test(struct st_edma_testparams *info, char *test_id)
{
	int result = SUCCESS;
	int fd = 0;


	do {
		fd = open(info->device, O_RDWR);
		if (FAILURE == fd) {
			perror("\n open");
			TEST_PRINT_ERR("file open failed ");
			result = FAILURE;
			break;
		}
		result = ioctl(fd, info->ioctl, info);
		if (FAILURE == result) {
			perror("IOCTL FAILED");
			TEST_PRINT_ERR("ioctl failed ");
			result = FAILURE;
		}
		result = close(fd);
		if (FAILURE == result) {
			perror("\n close");
			TEST_PRINT_ERR("file close failed ");
			result = FAILURE;
			break;
		}
	} while (0);
	return result;
}



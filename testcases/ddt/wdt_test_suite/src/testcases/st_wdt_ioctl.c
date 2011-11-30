/*
 * st_wdt_ioctl.c
 *
 * This file implements the wdt ioctl test case
 *
 * Copyright (C) 2010 Texas Instruments Incorporated - http://www.ti.com/ 
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

/*************************************************************************/
#include <sys/time.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
/* Include package headers here */
#include <st_log.h>
#include "st_wdt_common.h"

#define ST_WDT_TIMEOUT_VAL	120

/* Table which holds the support info of the WDT*/
extern struct supportinfo_table supportinfo_table[];

/****************************************************************************
 * Function		- st_wdt_ioctl_test 
 * Functionality	- This function recieves the test params and calls 
 *                        the appropariate ioctl and prints the status
 *                        accordingly
 * Input Params		- info,test_id
 * Return Value		- 0: SUCCESS, -1: FAILURE
 * Note			- None
 ****************************************************************************/
int st_wdt_ioctl_test(struct st_wdt_testparams *info, char *test_id,int fileDesc)
{
	int result = SUCCESS;
	int retVal = 0;
	int i = 0;
	int timeout_val = 0;
	int readtimeout_val = 0;
	int defaulttimeut_val = ST_WDT_TIMEOUT_VAL;

	do {
		if (FAILURE == fileDesc) {
			TEST_PRINT_ERR("file open failed ");
			result = FAILURE;
			break;
		}
		if (WDIOC_SETTIMEOUT == info->ioctl) {
			/* 1. Read the time out first
			   2. Set the time out to predefined value 
			   3. Get the timeout and verify.
			   4. Put back the time out vale read earlier 
			 */
			retVal =  ioctl(fileDesc, WDIOC_GETTIMEOUT, 
					&(timeout_val));
			if (FAILURE == retVal) {
				st_perror("IOCTL FAILED");
				result = FAILURE;
				break;
			}
			retVal =  ioctl(fileDesc, WDIOC_SETTIMEOUT,&defaulttimeut_val);
			if (FAILURE == retVal) {
				st_perror("IOCTL FAILED");
				result = FAILURE;
				break;
			}
			retVal =
			    ioctl(fileDesc, WDIOC_GETTIMEOUT,
				  &(readtimeout_val));
			if (FAILURE == retVal) {
				st_perror("IOCTL FAILED");
				result = FAILURE;
				break;
			}
			if (defaulttimeut_val != readtimeout_val) {
				TEST_PRINT_ERR
					("SETTIMEOUT IOCTL: Time set" 
					"and read did not match");
				result = FAILURE;
			}
		} else {
			retVal =
			    ioctl(fileDesc, info->ioctl, &(info->ioctl_arg));
			if (FAILURE == retVal) {
				st_perror("IOCTL FAILED");
				result = FAILURE;
				break;
			} else {				
				switch(info->ioctl) {
				case WDIOC_GETSUPPORT:
					TEST_PRINT_TRC("Return Value : 0x%x",
					(unsigned int)info->ioctl_arg);
					TEST_PRINT_TRC("Following Features Are"
					" Supported By The Driver : ");
					while (NULL != 
					      supportinfo_table[i].descr) {
						if (supportinfo_table[i].
						    supportinfo ==
						    (info->ioctl_arg &
						     supportinfo_table[i].
						     supportinfo)) {
							TEST_PRINT_TRC("%s",
							supportinfo_table[i].
							descr);
						}
						i++;
					}				
					break;	
				case WDIOC_GETTIMELEFT:
					TEST_PRINT_TRC("TIME LEFT IS : %ld",
						       info->ioctl_arg);
					break;	
				case WDIOC_GETSTATUS :
				case WDIOC_GETBOOTSTATUS :
					TEST_PRINT_TRC
					("STATUS VALUE : %ld \nStatus Is : ",
					info->ioctl_arg);

					while (NULL !=
					       supportinfo_table[i].descr) {
						if (supportinfo_table[i].
						    supportinfo ==
						    (info->
						     ioctl_arg &
						     supportinfo_table[i].
						     supportinfo)) {
							TEST_PRINT_TRC("%s",
							supportinfo_table[i].
							descr);
						}
						i++;
					}				
					break;	
				case WDIOC_GETTIMEOUT: 
					TEST_PRINT_TRC("TIME OUT VALUE : %ld",
						       info->ioctl_arg);				
					break;	
				case WDIOC_GETPRETIMEOUT: 
					TEST_PRINT_TRC("PRETIMEOUT VALUE : %ld",
						       info->ioctl_arg);				
					break;	
				case WDIOC_GETTEMP:
					TEST_PRINT_TRC("TEMP VALUE : %ld",
						       info->ioctl_arg);
					break;	
				}
			}
		}
	} while (0);
	return result;
}

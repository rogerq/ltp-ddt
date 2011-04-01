/*
 * st_rtc_common.h
 *
 * This file contains common definions
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

#ifndef _ST_RTC_COMMON_H_
#define _ST_RTC_COMMON_H_

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
#include <linux/rtc.h>
#include <limits.h>

/*Testcode related Header files*/
#include <st_defines.h>
#include <st_log.h>
#include <st_fileapi.h>
#include <st_timer.h>

#define ST_DEVICE_NAME_LEN  	50
#define ST_IOCTL_NAME_LEN   	50
#define ST_TESTCASEID_LEN   	100
#define DEFAULT_IOCTLTEST_NAME	"readtime"

enum rtc_ioctl_testcase {
	ST_RTC_READ_TIME,
	ST_RTC_SETGET_TIME,
	ST_RTC_UIE_ONOFF,
	ST_RTC_ALM
};

/*Structure for holding the test options */
struct st_rtc_testparams {

	/* Device name  */
	char device[ST_DEVICE_NAME_LEN];

	/* Test name id */
	char test_name[ST_TESTCASEID_LEN];

	/* IOCTL test case */
	enum rtc_ioctl_testcase ioctl_testcase;

	/* IOCTL tesatcase argument */
	long ioctl_testcasearg;

	/* Loop Count : */
	int loop;
};

/* Structure used to define the ioctl table */
struct rtc_ioctl_table {
	char *ioctl_testcasename;
	enum rtc_ioctl_testcase ioctl_testcase;
	long ioctl_testcasearg;
	int (*ioctl_testcase_func) (int, long);
	char *ioctl_info;
};

/* Function Declaratinos */

/****************************************************************************
 * Function                 - st_display_rtc_test_suite_help
 * Functionality            - This function displays the help/usage
 * Input Params             - None
 * Return Value             - None
 * Note                         - None
 ****************************************************************************/
void st_display_rtc_test_suite_help(void);

/****************************************************************************
 * Function                 - st_print_rtc_test_params
 * Functionality            - This function prints the test option values
 * Input Params             - Test params structure
 * Return Value             - None
 * Note                         - None
 ****************************************************************************/
void st_print_rtc_test_params(struct st_rtc_testparams *testoptions,
			      char *test_id);

/****************************************************************************
 * Function         - st_init_rtc_test_params
 * Functionality            - This function initilaizes the default values for
 * various test case options(which will be exposed as command line arguments)
 * Input Params             -  None
 * Return Value             -  None
 * Note                         -  None
 ****************************************************************************/
void st_init_rtc_test_params(void);

/****************************************************************************
 * Function             - st_rtc_ioctl_test
 * Functionality        - This function recieves the test params and calls
 *                        the appropariate ioctl
 * Input Params         -  info,test_id
 * Return Value         -  0: SUCCESS, -1: FAILURE
 * Note                 -  None
 ****************************************************************************/
int st_rtc_ioctl_test(struct st_rtc_testparams *info, char *test_id);

/****************************************************************************
 * Function             - st_rtc_setgettime
 * Functionality        - This function sets the RTC time and and verifies
 *                        by getting the time and displaying it
 * Input Params         -  fileDesc,ioctlarg
 * Return Value         -  0: SUCCESS, -1: FAILURE
 * Note                 -  None
 ****************************************************************************/
int st_rtc_setgettime(int fileDesc, long ioctlarg);

/****************************************************************************
 * Function             - st_rtc_uieonoff
 * Functionality        - This function verified whether interrutps are 
 *                        when UIE is ON.
 * Input Params         -  fileDesc,ioctlarg
 * Return Value         -  0: SUCCESS, -1: FAILURE
 * Note                 -  None
 ****************************************************************************/
int st_rtc_uieonoff(int fileDesc, long ioctlarg);

/****************************************************************************
 * Function             - st_rtc_setgetalarm
 * Functionality        - This function verifies the alarm functionality of the 
 *                        RTC.
 * Input Params         -  fileDesc,ioctlarg
 * Return Value         -  0: SUCCESS, -1: FAILURE
 * Note                 -  None
 ****************************************************************************/
int st_rtc_setgetalarm(int fileDesc, long ioctlarg);

/****************************************************************************
 * Function             - st_rtc_gettime
 * Functionality        - This function reads the RTC time and displays it.
 * Input Params         -  fileDesc,ioctlarg
 * Return Value         -  0: SUCCESS, -1: FAILURE
 * Note                 -  None
 ****************************************************************************/
int st_rtc_gettime(int fileDesc, long ioctlarg);

#endif /*ST_RTC_COMMON_H_ */

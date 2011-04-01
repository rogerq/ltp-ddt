/*
 * st_rtc_ioctl.c
 *
 * This file implements the rtc ioctl test case
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
#include "st_rtc_common.h"

#define		ST_RTC_MONJAN		0
#define		ST_RTC_MONFEB		1
#define		ST_RTC_MONMAR		2
#define		ST_RTC_MONAPR		3
#define		ST_RTC_MONMAY		4
#define		ST_RTC_MONJUN		5
#define		ST_RTC_MONJULY		6
#define		ST_RTC_MONAUG		7
#define		ST_RTC_MONSEP		8
#define		ST_RTC_MONOCT		9
#define		ST_RTC_MONNOV		10
#define		ST_RTC_MONDEC		11

#define 	ST_RTC_DAYSINJAN  	31
#define 	ST_RTC_DAYSINAPR  	30
#define 	ST_RTC_DAYSINFEB  	28

#define		ST_RTC_DEFAULT_SEC	57
#define		ST_RTC_DEFAULT_MIN	59
#define		ST_RTC_DEFAULT_HOUR	23
#define		ST_RTC_DEFAULT_DAY	31
#define		ST_RTC_DEFAULT_MONTH	11
#define		ST_RTC_DEFAULT_YEAR	(100)

static Bool st_rtc_compare_date(struct rtc_time *time1, struct rtc_time *time2);
static void st_rtc_add_secs_todate(struct rtc_time *rtc_time, int secs);

extern struct rtc_ioctl_table ioctl_table[];

/****************************************************************************
 * Function		- st_rtc_ioctl_test 
 * Functionality	- This function recieves the test params and calls 
 *			  the appropariate ioctl and prints the status 
 *			  accordingly
 * Input Params		- info,test_id
 * Return Value		- 0: SUCCESS, -1: FAILURE
 * Note			- None
 ****************************************************************************/
int st_rtc_ioctl_test(struct st_rtc_testparams *info, char *test_id)
{

	int result = SUCCESS;
	int fileDesc = 0;
	int retVal = 0;
	int i = 0;

	do {
		fileDesc = st_open(info->device, O_RDWR);
		if (FAILURE == fileDesc) {
			TEST_PRINT_ERR("file open failed ");
			result = FAILURE;
			break;
		}
		while ((ioctl_table[i].ioctl_testcase != info->ioctl_testcase)
		       && (ioctl_table[i].ioctl_testcasename != NULL)) {
			i++;
		}
		if (NULL == ioctl_table[i].ioctl_testcasename) {
			TEST_PRINT_ERR("Test Case Not Found");
			result = FAILURE;
		} else {
			retVal = (ioctl_table[i].ioctl_testcase_func) 
				(fileDesc, info->ioctl_testcasearg);
			result = retVal;
		}
		retVal = st_close(fileDesc);
		if (FAILURE == retVal) {
			TEST_PRINT_ERR("file close failed ");
			result = FAILURE;
			break;
		}
	} while (0);
	return result;
}

/****************************************************************************
 * Function		- st_rtc_compare_date
 * Functionality	- This function compares the time1 and time2
 * Input Params		-  time1,time2
 * Return Value		-  1: SUCCESS, 0: FAILURE
 * Note			-  None
 ****************************************************************************/
static Bool st_rtc_compare_date(struct rtc_time *time1, struct rtc_time *time2)
{
	if ((time1->tm_sec != time2->tm_sec) ||
	    (time1->tm_min != time2->tm_min) ||
	    (time1->tm_hour != time2->tm_hour) ||
	    (time1->tm_mday != time2->tm_mday) ||
	    (time1->tm_mon != time2->tm_mon) ||
	    (time1->tm_year != time2->tm_year))
		return FALSE;
	else
		return TRUE;
}

/****************************************************************************
 * Function             - st_rtc_setgettime
 * Functionality        - This function sets the RTC time and and verifies
 *                        by getting the time and displaying it
 * Input Params         -  fileDesc,ioctlarg
 * Return Value         -  0: SUCCESS, -1: FAILURE
 * Note                 -  None
 ****************************************************************************/
int st_rtc_setgettime(int fileDesc, long ioctlarg)
{
	int result = SUCCESS;
	int count = 5;
	struct rtc_time rtc_time1;
	struct rtc_time rtc_time2;
	struct rtc_time rtc_time3;
	int retVal = 0;

	/* 1. Read the current time.
	   2. Set the time.
	   3. Get the time for multiple times and verify wheather it's chaning 
		or not
	   4. Set back the time read intially */
	do {
		retVal = ioctl(fileDesc, RTC_RD_TIME, &(rtc_time1));
		if (FAILURE == retVal) {
			st_perror("IOCTL FAILED");
			result = FAILURE;
			break;
		}
		rtc_time2.tm_mday = ST_RTC_DEFAULT_DAY;
		rtc_time2.tm_mon = ST_RTC_DEFAULT_MONTH;
		rtc_time2.tm_year = ST_RTC_DEFAULT_YEAR;
		rtc_time2.tm_hour = ST_RTC_DEFAULT_HOUR;
		rtc_time2.tm_min = ST_RTC_DEFAULT_MIN;
		rtc_time2.tm_sec = ST_RTC_DEFAULT_SEC;

		TEST_PRINT_TRC("Setting the Default date : %d-%d-%d",
			       rtc_time2.tm_mday, (rtc_time2.tm_mon + 1),
			       (rtc_time2.tm_year + 1900));
		TEST_PRINT_TRC("Setting the Default Time : %02d-%02d-%02d",
			       rtc_time2.tm_hour, (rtc_time2.tm_min),
			       (rtc_time2.tm_sec));

		retVal = ioctl(fileDesc, RTC_SET_TIME, &(rtc_time2));
		if (FAILURE == retVal) {
			st_perror("IOCTL FAILED");
			result = FAILURE;
			break;
		}
		TEST_PRINT_TRC_NOSLEEP("Setting the Time is Successful, "
			"Please verify wheather Date and Time is changing");
		while (count) {

			sleep(1);
			/* Read the time and display */
			retVal = ioctl(fileDesc, RTC_RD_TIME, &(rtc_time3));
			if (FAILURE == retVal) {
				st_perror("IOCTL FAILED");
				result = FAILURE;
			} else {
				TEST_PRINT_TRC_NOSLEEP(" Date is : %d-%d-%d",
						       rtc_time3.tm_mday,
						       (rtc_time3.tm_mon + 1),
						       (rtc_time3.tm_year +
							1900));
				TEST_PRINT_TRC_NOSLEEP
				    (" Time is : %02d-%02d-%02d",
				     rtc_time3.tm_hour, (rtc_time3.tm_min),
				     (rtc_time3.tm_sec));
			}
			count--;
		}
		/* Since their is 5 Sec delay, increment time2 by 5 secs 
			and compare */
		st_rtc_add_secs_todate(&rtc_time2, 5);
		if (!st_rtc_compare_date(&rtc_time3, &rtc_time2)) {
			TEST_PRINT_ERR("Date Set and Get Didn't match");
			result = FAILURE;
			break;
		}
		TEST_PRINT_TRC("Date Set and Get Matched");
		/* Set back the read date and time */
		retVal = ioctl(fileDesc, RTC_SET_TIME, &(rtc_time1));
		if (FAILURE == retVal) {
			st_perror("IOCTL FAILED");
			result = FAILURE;
		}
	} while (FALSE);

	return (result);
}

/****************************************************************************
 * Function		- st_rtc_uieonoff
 * Functionality	- This function verified whether interrutps are 
 *                        when UIE is ON.
 * Input Params		-  fileDesc,ioctlarg
 * Return Value		-  0: SUCCESS, -1: FAILURE
 * Note			-  None
 ****************************************************************************/
int st_rtc_uieonoff(int fileDesc, long ioctlarg)
{
	int result = SUCCESS;
	int count = 5;
	int retVal = 0;
	unsigned long data;

	/* 1. Enable the Update intererrupt.
	   2. Call the read, which will block untill interrupt is gererated 
		for several times
	   3. Disable the interrupt. */

	retVal = ioctl(fileDesc, RTC_UIE_ON, 0);
	if (FAILURE == retVal) {
		st_perror("IOCTL FAILED");
		result = FAILURE;
	}
	TEST_PRINT_TRC
	    (" Counting 5 Update interrupts by reading from RTC device");
	for (count = 0; count < 5; count++) {
		retVal = read(fileDesc, &data, sizeof(unsigned long));
		if (FAILURE == retVal) {
			st_perror("Read FAILED");
			result = FAILURE;
		}
		TEST_PRINT_TRC(" Update Interrupt Received %d time.",
			       (count + 1));
	}
	retVal = ioctl(fileDesc, RTC_UIE_OFF, 0);
	if (FAILURE == retVal) {
		st_perror("IOCTL FAILED");
		result = FAILURE;
	}
	return (result);
}

/****************************************************************************
 * Function		- st_rtc_setgetalarm
 * Functionality	- This function verifies the alarm functionality of 
			  the RTC.
 * Input Params		-  fileDesc,ioctlarg
 * Return Value		-  0: SUCCESS, -1: FAILURE
 * Note			-  None
 ****************************************************************************/
int st_rtc_setgetalarm(int fileDesc, long ioctlarg)
{
	int result = SUCCESS;
	struct rtc_time rtc_time1;
	struct rtc_time rtc_time2;
	struct rtc_time rtc_time3;
	int retVal = 0;
	unsigned long data;

	/* 1. Geth the currnet time and set the alarm for user specified 
		num of secs from curent time
	   2. Set the alarm.
	   3. Get the alarm and compare.
	   4. Enable the alarm interrupt.
	   5. Wait for the alarm interrupt.
	   6. Disable the alarm interrupt. */

	do {
		retVal = ioctl(fileDesc, RTC_RD_TIME, &(rtc_time1));
		if (FAILURE == retVal) {
			st_perror("IOCTL FAILED");
			result = FAILURE;
			break;
		}
		TEST_PRINT_TRC("Current date is : %d-%d-%d",
			       rtc_time1.tm_mday, (rtc_time1.tm_mon + 1),
			       (rtc_time1.tm_year + 1900));
		TEST_PRINT_TRC("Current time is : %02d-%02d-%02d",
			       rtc_time1.tm_hour, (rtc_time1.tm_min),
			       (rtc_time1.tm_sec));
		TEST_PRINT_TRC
		    ("Setting the alarm after %ld Secs from Current Time",
		     ioctlarg);

		/* Add the seconds to time and set the alarm to that time */
		st_rtc_add_secs_todate(&rtc_time1, ioctlarg);
		retVal = ioctl(fileDesc, RTC_ALM_SET, &(rtc_time1));
		if (FAILURE == retVal) {
			st_perror("IOCTL FAILED");
			result = FAILURE;
			break;
		}
		retVal = ioctl(fileDesc, RTC_ALM_READ, &(rtc_time2));
		if (FAILURE == retVal) {
			st_perror("IOCTL FAILED");
			result = FAILURE;
			break;
		}
		if (FALSE == (st_rtc_compare_date(&rtc_time1, &rtc_time2))) {
			TEST_PRINT_ERR
			    ("Alarm Set Time And Get Time Didn't Match")
			    result = FAILURE;
			break;
		}
		TEST_PRINT_TRC_NOSLEEP("Alarm Set, Read Matched.")
		    TEST_PRINT_TRC_NOSLEEP("Alarm Set To : %02d-%02d-%02d",
					   rtc_time1.tm_hour,
					   (rtc_time1.tm_min),
					   (rtc_time1.tm_sec));
		retVal = ioctl(fileDesc, RTC_AIE_ON, 0);
		if (FAILURE == retVal) {
			st_perror("IOCTL FAILED");
			result = FAILURE;
			break;
		}
		retVal = read(fileDesc, &data, sizeof(unsigned long));
		if (FAILURE == retVal) {
			st_perror("Read FAILED");
			result = FAILURE;
			break;
		}
		TEST_PRINT_TRC_NOSLEEP("Alarm Rang.")
		    retVal = ioctl(fileDesc, RTC_RD_TIME, &(rtc_time3));
		if (FAILURE == retVal) {
			st_perror("IOCTL FAILED");
			result = FAILURE;
		}
		TEST_PRINT_TRC_NOSLEEP("Current time is : %02d-%02d-%02d",
				       rtc_time3.tm_hour, (rtc_time3.tm_min),
				       (rtc_time3.tm_sec));
		retVal = ioctl(fileDesc, RTC_AIE_OFF, 0);
		if (FAILURE == retVal) {
			st_perror("IOCTL FAILED");
			result = FAILURE;
		}
		/* Compare the alarm set time and current time */
		if (!st_rtc_compare_date(&rtc_time1, &rtc_time3)) {
			TEST_PRINT_ERR
			    ("Alarm Time and Current Time Did Not Match");
			result = FAILURE;
			break;
		}
		TEST_PRINT_TRC("Alarm Time and Current Time Matches");

	} while (FALSE);

	return (result);
}

/****************************************************************************
 * Function		- st_rtc_gettime
 * Functionality	- This function reads the RTC time and displays it.
 * Input Params		-  fileDesc,ioctlarg
 * Return Value		-  0: SUCCESS, -1: FAILURE
 * Note			-  None
 ****************************************************************************/
int st_rtc_gettime(int fileDesc, long ioctlarg)
{
	struct rtc_time rtc_time;
	int retVal = 0;
	int result = SUCCESS;

	retVal = ioctl(fileDesc, RTC_RD_TIME, &(rtc_time));
	if (FAILURE == retVal) {
		st_perror("IOCTL FAILED");
		result = FAILURE;
	}
	TEST_PRINT_TRC("Current date is : %d-%d-%d",
		       rtc_time.tm_mday, (rtc_time.tm_mon + 1),
		       (rtc_time.tm_year + 1900));
	TEST_PRINT_TRC("Current time is : %02d-%02d-%02d", rtc_time.tm_hour,
		       (rtc_time.tm_min), (rtc_time.tm_sec));

	return (result);
}

/****************************************************************************
 * Function             - st_rtc_add_secs_todate
 * Functionality        - This function Adds the Secs to date.
 * Input Params         -  date,Number of secs
 * Return Value         -  none
 * Note                 -  None
 ****************************************************************************/
static void st_rtc_add_secs_todate(struct rtc_time *rtc_time, int secs)
{

	rtc_time->tm_sec += secs;
	if (rtc_time->tm_sec >= 60) {
		rtc_time->tm_min += (rtc_time->tm_sec / 60);
		rtc_time->tm_sec %= 60;
	}
	if (rtc_time->tm_min >= 60) {
		rtc_time->tm_hour += (rtc_time->tm_min / 60);
		rtc_time->tm_min %= 60;
	}
	if (rtc_time->tm_hour >= 24) {
		rtc_time->tm_mday += (rtc_time->tm_hour / 24);
		rtc_time->tm_hour = (rtc_time->tm_hour % 24);
	}
	switch (rtc_time->tm_mon) {
	case ST_RTC_MONJAN:
	case ST_RTC_MONMAR:
	case ST_RTC_MONMAY:
	case ST_RTC_MONJULY:
	case ST_RTC_MONAUG:
	case ST_RTC_MONOCT:
	case ST_RTC_MONDEC:

		if (rtc_time->tm_mday > ST_RTC_DAYSINJAN) {
			rtc_time->tm_mon +=
			    (rtc_time->tm_mday / ST_RTC_DAYSINJAN);
			rtc_time->tm_mday =
			    rtc_time->tm_mday % ST_RTC_DAYSINJAN;
		}
		break;
	case ST_RTC_MONAPR:
	case ST_RTC_MONJUN:
	case ST_RTC_MONSEP:
	case ST_RTC_MONNOV:

		if (rtc_time->tm_mday > ST_RTC_DAYSINAPR) {
			rtc_time->tm_mon +=
			    (rtc_time->tm_mday / ST_RTC_DAYSINAPR);
			rtc_time->tm_mday =
			    rtc_time->tm_mday % ST_RTC_DAYSINAPR;
		}
		break;
	case ST_RTC_MONFEB:
		if (rtc_time->tm_mday > ST_RTC_DAYSINFEB) {
			rtc_time->tm_mon +=
			    (rtc_time->tm_mday / ST_RTC_DAYSINFEB);
			rtc_time->tm_mday =
			    rtc_time->tm_mday % ST_RTC_DAYSINFEB;
		}
		break;
	}

	if (rtc_time->tm_mon >= 12) {
		rtc_time->tm_year += (rtc_time->tm_mon / 12);
		rtc_time->tm_mon = (rtc_time->tm_mon % 12);
	}
}

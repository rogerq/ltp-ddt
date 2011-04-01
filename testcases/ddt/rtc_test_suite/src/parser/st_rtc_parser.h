/*
 * st_rtc_parser.h
 *
 * This file contains command line option definions
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

#ifndef _ST_WDT_PARSER_H_
#define _ST_WDT_PARSER_H_

/*
 * Define Options here, so that it would be easy to add new features/options.
 */

/* Device name on which rtc test is to be run */
#define OPTION_DEVICE_NAME	1

/* execute the IOCTL test case */
#define OPTION_IOCTL		2

/* IOCTL name to execute */
#define OPTION_IOCTL_ARG	3

/* Loop Count i.e how many time to execute the particular test */
#define OPTION_LOOP     	4

/* Test case id string for logging purpose */
#define OPTION_TESTCASE_ID      5

/* Displays the version of LFTB package */
#define OPTION_VERSION       	'v'
/* Displays the help */
#define OPTION_HELP        	'h'

#define DEFAULT_IOCTL_NAME      "gettime"

/* Table which holds the ioctl info */
struct rtc_ioctl_table ioctl_table[] = {
	{"readtime", ST_RTC_READ_TIME, 0, st_rtc_gettime, "RTC_RD_TIME"},
	{"setgettime", ST_RTC_SETGET_TIME, 0, st_rtc_setgettime,
	 "RTC_SET_TIME, RTC_RD_TIME"},
	{"updateint", ST_RTC_UIE_ONOFF, 0, st_rtc_uieonoff,
	 "RTC_UIE_ON,RTC_UIE_OFF"},
	{"alarm", ST_RTC_ALM, 20, st_rtc_setgetalarm,
	 "RTC_ALM_SET, RTC_ALM_READ, RTC_ALM_ON, RTC_ALM_OFF"},
	{"NULL", 0, 0, NULL, NULL}
};

#endif /* #ifndef _ST_WDT_PARSER_H_ */

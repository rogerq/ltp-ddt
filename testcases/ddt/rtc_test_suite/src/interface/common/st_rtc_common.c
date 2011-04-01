/*
 * st_rtc_common.c
 *
 * This file contains the interface specific funtions
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

/* Generic header files */
#include <st_defines.h>
/*Testcode header file*/
#include "st_rtc_common.h"
/* Storage structures, enums, macros defined */
#define DEFAULT_DEVICE			"/dev/rtc0"
#define	DEFAULT_LOOP_COUNT  		1
#define DEFAULT_IOCTLTESTCASE_ARG     	20
#define DEFAULT_IOCTL_TESTCASE		ST_RTC_READ_TIME
/* Global Variables */
struct st_rtc_testparams testoptions;
extern struct rtc_ioctl_table ioctl_table[];

/****************************************************************************
 * Function			- st_init_rtc_test_params
 * Functionality		- This function initilaizes the default values 
 				  for various test case options(which will be 
				  exposed as command line arguments)
 * Input Params			-  None
 * Return Value			-  None
 * Note				-  None
 ****************************************************************************/
void st_init_rtc_test_params(void)
{
	strcpy(testoptions.device, DEFAULT_DEVICE);
	testoptions.ioctl_testcase = DEFAULT_IOCTL_TESTCASE;
	testoptions.loop = DEFAULT_LOOP_COUNT;
	testoptions.ioctl_testcasearg = DEFAULT_IOCTLTESTCASE_ARG;
}

/****************************************************************************
 * Function             	- st_display_rtc_test_suite_help
 * Functionality        	- This function displays the help/usage
 * Input Params         	- None
 * Return Value         	- None
 * Note                 		- None
 ****************************************************************************/
void st_display_rtc_test_suite_help(void)
{
	printf("rtc_tests -device [-ioctltest] [-ioctltestarg] [-loop] "
		"[-id] [-h/elp] [-v/ersion]\n\n"
		"-device      :  Device node interface name "
		"example -/dev/rtc0\n"
		"-ioctltest   :  IOCTL test to execute\n"
		"                Possible values-readtime,setgettime,\n"
                "                updateint,alarm\n"
		"-ioctltestarg:  Argument to IOCTL test\n"
		"-loop        :  No of times to execute the test\n"
		"-id          :  Test case id string for testers reference\n"
		"                logging purpose\n"
		"-help        :  Displays the help/usage\n"
		"-version     :  Version of RTC Test suite\n");
}

/****************************************************************************
 * Function             	- st_print_rtc_test_params
 * Functionality        	- This function prints the test option values
 * Input Params         	- Test params structure
 * Return Value         	- None
 * Note                 		- None
 ****************************************************************************/
void st_print_rtc_test_params(struct st_rtc_testparams *testoptions,
			      char *test_id)
{

	int i = 0;
	TEST_PRINT_TRC("******** RTC Testcase  parameters  ******** ");
	TEST_PRINT_TRC("Device         : %s", testoptions->device);
	TEST_PRINT_TRC("Loop Count     : %d", testoptions->loop);
	while (ioctl_table[i].ioctl_testcase != testoptions->ioctl_testcase) {
		i++;
		continue;
	}
	TEST_PRINT_TRC("Ioctl TestCase : %s",
		       ioctl_table[i].ioctl_testcasename);
	TEST_PRINT_TRC("Test Coverage  : %s", ioctl_table[i].ioctl_info);
	TEST_PRINT_TRC("Ioctl Arg      : %ld ", testoptions->ioctl_testcasearg);
	TEST_PRINT_TRC(" ************* End of Test params ************* ");
}

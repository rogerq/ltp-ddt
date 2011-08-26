/*
 * st_wdt_common.c
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
#include "st_wdt_common.h"
/* Storage structures, enums, macros defined */
#define DEFAULT_IOMODE        ST_WDT_IOMODE_NONE
#define DEFAULT_DEVICE        "/dev/watchdog"
#define DEFAULT_IOCTL         ST_WDT_IOCTL_NONE
#define DEFAULT_LOOP_COUNT    1
#define DEFAULT_IOCTL_ARG     0

/* Global Variables */
struct st_wdt_testparams testoptions;
extern struct wdt_ioctl_table ioctl_table[];

/****************************************************************************
 * Function			- st_init_wdt_test_params
 * Functionality        	- This function initilaizes the default values 
 *				  for various test case options(which will 
 * 				  be exposed as command line arguments)
 * Input Params			- None
 * Return Value			- None
 * Note				- None
 ****************************************************************************/
void st_init_wdt_test_params(void)
{
	strcpy(testoptions.device , DEFAULT_DEVICE);
	testoptions.ioctl = DEFAULT_IOCTL;
	testoptions.loop = DEFAULT_LOOP_COUNT;
	testoptions.ioctl_arg = DEFAULT_IOCTL_ARG;
	testoptions.iomode = DEFAULT_IOMODE;
}

/****************************************************************************
 * Function			- st_display_wdt_test_suite_help
 * Functionality		- This function displays the help/usage
 * Input Params			- None
 * Return Value			- None
 * Note				- None
 ****************************************************************************/
void st_display_wdt_test_suite_help(void)
{
	printf( "wdt_tests -device [-write] [-ioctl] [-ioctlarg] [-loop] "
		"[-id] [-h/elp] [-v/ersion] \n\n"
		"-device      :  Device node interface name example"
		" -/dev/watchdog\n"
		"-write       :  I/O direction mode is write\n"
		"                no argument value required\n"
		"-ioctl       :  IOCTL to execute\n"
		"                Possible values-keepalive,\n"
		"                settimeout,gettimeout,\n"
		"                getsupport,setoptions,\n"
		"                getstatus,gettimeleft,\n"
		"                getbootstatus,getpretimeout,\n"
		"                setpretimeout,tgettemp\n"
		"-ioctlarg    :  Argument value to the IOCTL tests\n"
                "-loop        :  No of times to execute the test\n"
		"-id          :  Test case id string for testers reference\n"
		"                logging purpose\n"
		"-h/elp       :  Displays the help/usage\n"
		"-v/ersion    :  Version of WDT Test suite\n");
}

/****************************************************************************
 * Function             	- st_print_wdt_test_params
 * Functionality        	- This function prints the test option values
 * Input Params         	- Test params structure
 * Return Value         	- None
 * Note                 		- None
 ****************************************************************************/
void st_print_wdt_test_params(struct st_wdt_testparams *testoptions,
			      char *test_id)
{

	int i = 0;
	TEST_PRINT_TRC("******** WDT Testcase  parameters  ******** ");
	TEST_PRINT_TRC("Device         : %s", testoptions->device);
	TEST_PRINT_TRC("Loop Count     : %d", testoptions->loop);

	if (ST_WDT_IOMODE_NONE != testoptions->iomode) {
		TEST_PRINT_TRC("I/O Operation  : Write");
	}
	if (DEFAULT_IOCTL != testoptions->ioctl) {
		TEST_PRINT_TRC("Operation      : Ioctl");
		while (ioctl_table[i].ioctl != testoptions->ioctl) {
			i++;
			continue;
		}
		TEST_PRINT_TRC("Ioctl Name     : %s",
			       ioctl_table[i].ioctl_dispname);
		TEST_PRINT_TRC("Ioctl Arg      : %ld ", testoptions->ioctl_arg);
	}
	TEST_PRINT_TRC(" ************* End of Test params ************* ");
}


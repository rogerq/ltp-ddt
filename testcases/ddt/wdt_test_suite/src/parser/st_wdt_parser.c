/*
 * st_wdt_parser.c
 *
 * This file contains wdt parser and gives user selectable test cases
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

/*Testcode related header files */
#include "st_wdt_common.h"
#include "st_wdt_parser.h"

char testcaseid[ST_TESTCASEID_LEN] = "wdt_tests";

/* Test case options structure */
extern struct st_wdt_testparams testoptions;

/*Function to display test suite version */
static void st_display_wdt_testsuite_version(void);

/* Function to get the hex value from the str */
unsigned long st_get_unsigned(const char *str);

/* Macro Definations */
#define DEFAULT_LOOP_COUNT    1
extern int fileDes;

/****************************************************************************
 * Function		- st_process_wdt_test_options 
 * Functionality	- This function parses the command line options and 
			- values passed for the options
 * Input Params		- argc,argv
 * Return Value		- None
 * Note			- None
 ****************************************************************************/
static int st_process_wdt_test_options(int argc, char **argv)
{
	int error = TRUE;
	int timeout=0;
	int version = FALSE;
	int help = FALSE;
	int is_ioctl = FALSE;
	int is_write = FALSE;
	int ret_val1 = SUCCESS;
	int ret_val2 = SUCCESS;
	int result = SUCCESS;
	int loop_count = 0;
	int i = 0;
	int fileDesc = 0;
	char ioctl_name[ST_IOCTL_NAME_LEN];
	for (;;) {
		int option_index = 0;

    /** Options for getopt - New test case options added need to be
         * populated here*/
		static struct option long_options[] = {
			{"device", required_argument, NULL, OPTION_DEVICE_NAME},
			{"ioctl", optional_argument, NULL, OPTION_IOCTL},
			{"ioctlarg", optional_argument, NULL, OPTION_IOCTL_ARG},
			{"loop", optional_argument, NULL, OPTION_LOOP},
			{"id", optional_argument, NULL, OPTION_TESTCASE_ID},
			{"write", no_argument, NULL, OPTION_WRITE},
			{"version", no_argument, NULL, OPTION_VERSION},
			{"help", no_argument, NULL, OPTION_HELP},
			{NULL, 0, NULL, 0}
		};
		int c = getopt_long_only(argc, argv, "vh", long_options,
					 &option_index);
		if (c == -1) {
			break;
		}
		if(c == '?') {
			error = TRUE;
			break;
		}
		switch (c) {
		case OPTION_DEVICE_NAME:
				error = FALSE;
				if (optarg != NULL) {
					strcpy(testoptions.device, optarg);
				}
			break;
		case OPTION_TESTCASE_ID:
			if (optarg != NULL) {
				strcpy(testcaseid, optarg);
			} else if (optind < argc && ('-' != argv[optind][0])) {
				strcpy(testcaseid, argv[optind]);
			}
			break;
		case OPTION_WRITE:
			is_write = TRUE;
			testoptions.iomode = ST_WDT_IOMODE_WRITE;
			break;
		case OPTION_IOCTL:
			is_ioctl = TRUE;
			if (optarg != NULL) {
				strcpy(ioctl_name, optarg);
			} else if ((optind < argc && ('-' != argv[optind][0]))) {
				strcpy(ioctl_name, argv[optind]);
			} else {
				strcpy(ioctl_name, DEFAULT_IOCTL_NAME);
			}
			while ((strcmp((ioctl_table[i].ioctl_name), ioctl_name)
				!= 0) && (ioctl_table[i].ioctl_name != NULL)) {
				i++;
			}
			if (NULL == ioctl_table[i].ioctl_name) {
				error = TRUE;
			} else {
				testoptions.ioctl = ioctl_table[i].ioctl;
				testoptions.ioctl_arg =
				    ioctl_table[i].ioctl_arg;
			}
			break;

		case OPTION_LOOP:
			if (optarg != NULL) {
				testoptions.loop = atoi(optarg);
			} else if (optind < argc && ('-' != argv[optind][0])) {
				testoptions.loop = atoi(argv[optind]);
			}

			break;
		case OPTION_IOCTL_ARG:
			if (optarg != NULL) {
				testoptions.ioctl_arg = atoi(optarg);
			} else if (optind < argc && ('-' != argv[optind][0])) {
				testoptions.ioctl_arg = atoi(argv[optind]);
			}

			break;
		case OPTION_VERSION:
			st_display_wdt_testsuite_version();
			version = TRUE;
			break;
		case OPTION_HELP:
			help = TRUE;
			break;
		}
	}
	/* If no test is specified, execute the defaut test case */
	if(is_ioctl == FALSE && is_write == FALSE) {
		is_write = TRUE;
		testoptions.iomode = ST_WDT_IOMODE_WRITE;
	}
	if (error == TRUE || help == TRUE) {
		if(version)
			exit(0);
		else
			st_display_wdt_test_suite_help();
		if( error == TRUE )
			result = FAILURE;
	} else {
		TEST_PRINT_TST_START(testcaseid);
		st_print_wdt_test_params(&testoptions, testcaseid);
		fileDesc = st_wdt_open(&testoptions);
		while (testoptions.loop > loop_count) {
			if (is_write == TRUE) {
				ret_val1 =
				    st_wdt_write_test(&testoptions, testcaseid, fileDesc);
			}
			if (is_ioctl == TRUE) {
				ret_val2 =
				    st_wdt_ioctl_test(&testoptions, testcaseid, fileDesc);
			}
			sleep(1);
			if ((SUCCESS != ret_val1) || (SUCCESS != ret_val2)) {
				result = FAILURE;
				break;
			}
			if (TRUE == is_write
			    || (WDIOC_KEEPALIVE == testoptions.ioctl)) {
				TEST_PRINT_TRC
				    ("Keeping WDT alive, sleep counter -%u(sec)",
				     loop_count);
			}
			loop_count++;
		}
		//result = st_wdt_close(fileDesc);
		TEST_PRINT_TST_RESULT(result, testcaseid);
		TEST_PRINT_TST_END(testcaseid);
		ioctl(fileDesc, WDIOC_GETTIMEOUT, &timeout);
	    	TEST_PRINT_TRC("EVM will be rebooted after %d seconds",timeout);
		while(1)
		{
			timeout--;
	    		TEST_PRINT_TRC("%d seconds remaining before reboot",timeout);
			sleep (1);
		}
	}
	return result;
}

/****************************************************************************
 * Function		- st_display_wdt_testsuite_version
 * Functionality	- This function displays the test suite version
 * Input Params		- None 
 * Return Value		- None
 * Note			- None
 ****************************************************************************/
static void st_display_wdt_testsuite_version(void)
{
	TEST_PRINT_TRC("Version : %s", VERSION_STRING);
}

/****************************************************************************
 * Function		- Main function
 * Functionality	- This is where the execution begins
 * Input Params		- argc,argv
 * Return Value		- None
 * Note			- None
 ****************************************************************************/
int main(int argc, char *argv[])
{

	int retval;
	/* Initialize options with default vales */
	st_init_wdt_test_params();

	/* Invoke the parser function to process the command line options */
	retval = st_process_wdt_test_options(argc, argv);
	return retval;
}


/*
 * st_edma_parser.c
 *
 * This file contains edma parser and gives user selectable test cases
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

/*Testcode related header files */
#include "st_edma_common.h"
#include "st_edma_parser.h"

char *testcaseid = "edma_tests";

/* Test case options structure */
extern struct st_edma_testparams testoptions;

/*Function to display test suite version */
void st_display_edma_testsuite_version();

/* Macro Definations */

#define ST_DEFAULT_OPTIONS    WDIOS_ENABLECARD
#define ST_DEFAULT_TIMEOUT    60
#define ST_DEFAULT_PRETIMEOUT 50
#define DEFAULT_LOOP_COUNT    1
#define ST_IOCTL_NAME_LEN     50

/*
 * Function             - st_process_edma_test_options 
 * Functionality        - This function parses the command line options and values passed for the options
 * Input Params         -  argc,argv
 * Return Value         -  None
 * Note                 -  None
 */
static int st_process_edma_test_options(int argc, char **argv)
{
	int error = TRUE;
	int version = FALSE;
	int help = FALSE;
	int ioctl = FALSE;
	int ret_val = FAILURE;
	char ioctl_name[ST_IOCTL_NAME_LEN];
	int i = 0;
	for (;;) {
		int option_index = 0;

    /** Options for getopt - New test case options added need to be
         * populated here*/
		static struct option long_options[] = {
			{"device", required_argument, NULL, OPTION_DEVICE_NAME},
			{"ioctl", required_argument, NULL, OPTION_IOCTL},
			{"acnt", required_argument, NULL, OPTION_ACNT},
			{"bcnt", required_argument, NULL, OPTION_BCNT},
			{"ccnt", required_argument, NULL, OPTION_CCNT},
			{"numtcs", required_argument, NULL, OPTION_NUMTCS},
			{"performance", no_argument, NULL, OPTION_PERFORMANCE},
			{"id", required_argument, NULL, OPTION_TESTCASE_ID},
			{"version", no_argument, NULL, OPTION_VERSION},
			{"help", no_argument, NULL, OPTION_HELP},
			{NULL, 0, NULL, 0}
		};
		int c = getopt_long_only(argc, argv, "vh", long_options,
					 &option_index);
		if (c == -1) {
			break;
		}
		if (c == '?') {
			help = TRUE;
			break;
		}
		switch (c) {
		case OPTION_DEVICE_NAME:
			error = FALSE;
			if (optarg != NULL) {
				strcpy(testoptions.device, optarg);
			} else if (optind < argc && ('-' != argv[optind][0])) {
				strcpy(testoptions.device, argv[optind]);
			}
			break;
		case OPTION_ACNT:
			if (optarg != NULL) {
				testoptions.acnt = atoi(optarg);
			} else if (optind < argc && ('-' != argv[optind][0])) {
				testoptions.acnt = atoi(argv[optind]);
			}

			break;

		case OPTION_BCNT:
			if (optarg != NULL) {
				testoptions.bcnt = atoi(optarg);
			} else if (optind < argc && ('-' != argv[optind][0])) {
				testoptions.bcnt = atoi(argv[optind]);
			}
			break;
		case OPTION_CCNT:
			if (optarg != NULL) {
				testoptions.ccnt = atoi(optarg);
			} else if (optind < argc && ('-' != argv[optind][0])) {
				testoptions.ccnt = atoi(argv[optind]);
			}
			break;

		case OPTION_NUMTCS:
			if (optarg != NULL) {
				testoptions.numtcs = atoi(optarg);
			} else if (optind < argc && ('-' != argv[optind][0])) {
				testoptions.numtcs = atoi(argv[optind]);
			}
			break;

		case OPTION_TESTCASE_ID:
			if (optarg != NULL) {
				strcpy(testoptions.testcaseid, optarg);
			} else if (optind < argc && ('-' != argv[optind][0])) {
				strcpy(testoptions.testcaseid, argv[optind]);
			}
			break;

		case OPTION_IOCTL:
			ioctl = TRUE;
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
			}
			break;
		case OPTION_PERFORMANCE:
			testoptions.perf_flag = 1;
			break;
		case OPTION_VERSION:
			version = TRUE;
			st_display_edma_testsuite_version();
			break;
		case OPTION_HELP:
			help = TRUE;
			break;
		}
	}

	if ((testoptions.acnt * testoptions.bcnt * testoptions.ccnt) > MAX_DMA_TRANSFER_IN_BYTES)
		error = TRUE;

	if (error == TRUE || help == TRUE) {
		st_display_edma_test_suite_help();
	if( error == TRUE)
		ret_val = FAILURE;		
	}
	else
		ret_val = st_edma_ioctl_test(&testoptions, testcaseid);

	return ret_val;
}

/*
 * Function                 - st_display_edma_testsuite_version
 * Functionality        - This function displays the test suite version
 * Input Params             - None 
 * Return Value             - None
 * Note                         - None
 */
void st_display_edma_testsuite_version()
{
	printf("Version : %s\n", VERSION_STRING);
	exit(0);
}

/*
 * Function                 - Main function
 * Functionality        - This is where the execution begins
 * Input Params         - argc,argv
 * Return Value             - None
 * Note                         - None
 */
int main(int argc, char *argv[])
{

	/* Initialize options with default vales */
	st_init_edma_test_params();

	/* Initialize Timer module */
	init_timer_module();

	/* Invoke the parser function to process the command line options */
	return st_process_edma_test_options(argc, argv);

}

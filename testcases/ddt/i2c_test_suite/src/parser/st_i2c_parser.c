/*
 * st_i2c_parser.c
 *
 * This file contains i2c parser and gives user selectable test cases
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

/*
 *Testcode related header files 
 */
#include "st_i2c_common.h"
#include "st_i2c_parser.h"

char *testcaseid = "i2c_tests";

/* 
 *Test case options structure 
 */
extern struct st_i2c_testparams testoptions;

/*
 *Function to display test suite version 
 */
void st_display_i2c_testsuite_version();

/* 
 *Function to get the hex value from the str 
 */
unsigned long st_get_unsigned(const char *str);

/*
 * Function             - st_process_i2c_test_options 
 * Functionality        - This function parses the command line options and values passed for the options
 * Input Params         -  argc,argv
 * Return Value         -  None
 * Note                 -  None
 */
static int st_process_i2c_test_options(int argc, char **argv)
{
	int error = TRUE;
	int version = FALSE;
	int help = FALSE;
	int result = FAILURE;
	for (;;) {
		int option_index = 0;
		static struct option long_options[] = {
			{"device", required_argument, NULL, OPTION_DEVICE},
			{"address", required_argument, NULL, OPTION_ADDRESS},
			{"buffer_size", required_argument, NULL,
			 OPTION_BUFFER_SIZE}, {"page_size", required_argument,
					       NULL, OPTION_PAGE_SIZE},
			{"total_size", required_argument, NULL,
			 OPTION_TOTAL_SIZE},
			{"id", required_argument, NULL, OPTION_TESTCASE_ID},
			{"read", no_argument, NULL, OPTION_READ}, {"write",
								   no_argument,
								   NULL,
								   OPTION_WRITE},
			{"performance", optional_argument, NULL,
			 OPTION_PERFORMANCE}, {"version", no_argument, NULL,
					       OPTION_VERSION}, {"help",
								 no_argument,
								 NULL,
								 OPTION_HELP},
			{NULL, 0, NULL, 0}
		};
		int c = getopt_long_only(argc, argv, "vh", long_options,
					 &option_index);
		if (c == -1) {
			break;
		}
		if (c == '?') {
			error = TRUE;
			break;
		}
		switch (c) {
		case OPTION_DEVICE:
			error = FALSE;
			if (optarg != NULL) {
				testoptions.device_node = optarg;
			} else if (optind < argc && ('-' != argv[optind][0])) {
				testoptions.device_node = argv[optind];
			}
			break;
		case OPTION_ADDRESS:
			if (optarg != NULL) {
				testoptions.address = st_get_unsigned(optarg);
			} else if (optind < argc && ('-' != argv[optind][0])) {
				testoptions.address =
				    st_get_unsigned(argv[optind]);
			}
			break;
		case OPTION_BUFFER_SIZE:
			if (optarg != NULL) {
				testoptions.buffer_size = atoi(optarg);
			} else if (optind < argc && ('-' != argv[optind][0])) {
				testoptions.buffer_size = atoi(argv[optind]);
			}
			break;
		case OPTION_PAGE_SIZE:
			if (optarg != NULL) {
				testoptions.page_size = atoi(optarg);
			} else if (optind < argc && ('-' != argv[optind][0])) {
				testoptions.page_size = atoi(argv[optind]);
			}
			break;
		case OPTION_TOTAL_SIZE:
			if (optarg != NULL) {
				testoptions.file_size = atoi(optarg);
			} else if (optind < argc && ('-' != argv[optind][0])) {
				testoptions.file_size = atoi(argv[optind]);
			}
			break;
		case OPTION_TESTCASE_ID:
			if (optarg != NULL) {
				testcaseid = optarg;
			} else if (optind < argc && ('-' != argv[optind][0])) {
				testcaseid = argv[optind];
			}
			break;
		case OPTION_READ:
			testoptions.iomode = 'r';
			break;
		case OPTION_WRITE:
			testoptions.iomode = 'w';
			break;
		case OPTION_PERFORMANCE:
			testoptions.throughput_flag = TRUE;
			testoptions.cpuload_flag = TRUE;
			if (optarg != NULL) {
				if (strcmp(optarg, THROUGHPUT) == 0) {
					testoptions.throughput_flag = TRUE;
					testoptions.cpuload_flag = FALSE;
				} else if (strcmp(optarg, CPULOAD) == 0) {
					testoptions.throughput_flag = FALSE;
					testoptions.cpuload_flag = TRUE;
				}
			} else if (optind < argc && ('-' != argv[optind][0])) {
				if (strcmp(argv[optind], THROUGHPUT) == 0) {
					testoptions.throughput_flag = TRUE;
					testoptions.cpuload_flag = FALSE;
				} else if (strcmp(argv[optind], CPULOAD) == 0) {
					testoptions.throughput_flag = FALSE;
					testoptions.cpuload_flag = TRUE;
				}
			}
			break;
		case OPTION_VERSION:
			version = TRUE;
			st_display_i2c_testsuite_version();
			break;
		case OPTION_HELP:
			help = TRUE;
			break;
		}
	}
	if (error == TRUE || help == TRUE) {
		if (version)
			exit(0);

		else
			st_display_i2c_test_suite_help();
		if( error == TRUE)
			result = FAILURE;
	}
	else {	
		st_print_i2c_test_params(&testoptions, testcaseid);
		if (testoptions.iomode == 'r')
			result = st_i2c_eeprom_read_test(&testoptions, testcaseid);
		else
			result = st_i2c_eeprom_write_test(&testoptions, testcaseid);
		TEST_PRINT_TST_RESULT(result, testcaseid);
		TEST_PRINT_TST_END(testcaseid);
	}
	return result;
}

/*
 * Function            	 	- st_get_unsigned
 * Functionality       	- Super-safe conversion of a string into an unsigned.
 * Input Params         	- str
 * Return Value         	- resultant value on succes, -1 on failure
 * Note                 		- None
 */
unsigned long st_get_unsigned(const char *str)
{
	char *end_ptr = NULL;
	unsigned long result = strtoul(str, &end_ptr, 0);
	if ((str[0] != '\0') && (*end_ptr == '\0')) {
		if (result == ULONG_MAX) {
			TEST_PRINT_ERR("Too large integer: %s", str);
			return FAILURE;
		}
		return result;
	}
	TEST_PRINT_ERR("Invalid value : %s", str);
	return FAILURE;
}

/*
 * Function            	 	- st_display_i2c_testsuite_version
 * Functionality       	- This function displays the test suite version
 * Input Params         	- None 
 * Return Value         	- None
 * Note                 		- None
 */
void st_display_i2c_testsuite_version()
{
	printf("Version : %s\n", VERSION_STRING);
}

/*
 * Function             	- Main function
 * Functionality       	- This is where the execution begins
 * Input Params      	- argc,argv
 * Return Value         	- None
 * Note                 		- None
 */
int main(int argc, char *argv[])
{

	/* Initialize options with default vales */
	st_init_i2c_test_params();

	/* Initialize Timer module */
	init_timer_module();

	/* Invoke the parser function to process the command line options */
	return st_process_i2c_test_options(argc, argv);
}

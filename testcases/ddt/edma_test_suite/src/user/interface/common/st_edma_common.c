/*
 * st_edma_common.c
 *
 * This file contains the interface specific funtions
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

/* Generic header files */
#include <st_defines.h>

/*Testcode header file*/
#include "st_edma_common.h"


/* Storage structures, enums, macros defined */
#define DEFAULT_ACNT     8
#define DEFAULT_BCNT     8
#define DEFAULT_CCNT     8
#define DEFAULT_NUMTCS   2
#define DEFAULT_DEVICE    "/dev/edma"
  




/* Global Variables */
struct st_edma_testparams testoptions;

/*
 * Function			- st_init_edma_test_params
 * Functionality        	- This function initilaizes the default values for
 * various test case options(which will be exposed as command line arguments)
 * Input Params         	-  None
 * Return Value         	-  None
 * Note                 		-  None
 */
void st_init_edma_test_params(void)
{
	testoptions.acnt = DEFAULT_ACNT;
	testoptions.bcnt = DEFAULT_BCNT;
	testoptions.ccnt = DEFAULT_CCNT;
	testoptions.numtcs = DEFAULT_NUMTCS;
	testoptions.perf_flag = FALSE;
	strcpy(testoptions.device, DEFAULT_DEVICE); 
	strcpy(testoptions.testcaseid, "EDMA"); 
	testoptions.ioctl = EDMA_ASYNC;
}

/*
 * Function             	- st_display_edma_test_suite_help
 * Functionality        	- This function displays the help/usage
 * Input Params         	- None
 * Return Value         	- None
 * Note                 		- None
 */
void st_display_edma_test_suite_help(void)
{
	printf( "edma_tests -device [-acnt] [-bcnt] [-ccnt] [-numtcs] [-performance]"
		"[-id] [-h/elp] [-v/ersion] \n\n"
		"-device      :  Device-node interface "
		" -/dev/edma\n"
		"-acnt        :  acnt value [8]"
		"-bcnt        :  bcnt value [8]"
		"-ccnt        :  ccnt value [8]"
		"	      :  Total acnt * bcnt * ccnt should be less than or"
		"	      :  equal to 1024 * 2 * 1024 due to buffer allocation limit"
		"-numtcs      :  Number of transfer controllers [2]"
		"-performance :  Throughput and cpuload measurement[disabled]\n"
		"                pass the option without any argument\n"
		"                to enable both cpuload and throughput\n"
		"                pass the option with string throughput\n"
		"                to enable throughput only\n"
		"                pass the option with string cpuload\n"
		"                to enable cpuload only\n"
		"-ioctl       :  IOCTL (transfer modes) to execute [edma_async]\n"
 	        "                Possible values- edma_async, edma_absync, edma_link_async\n"
                "                dma_link_absync, edma_chain_async, edma_chain_absync\n"
		"-id          :  Test case id string for testers reference\n"
		"                logging purpose\n"
		"-h/elp       :  Displays the help/usage\n"
		"-v/ersion    :  Version of EDMA Test suite\n");

}

/*
 * Function             	- st_print_edma_test_params
 * Functionality        	- This function prints the test option values
 * Input Params         	- Test params structure
 * Return Value         	- None
 * Note                 		- None
 */
void st_print_edma_test_params(struct st_edma_testparams *testoptions,
			      char *test_id)
{

	TEST_PRINT_TRC("******** edma Testcase  parameters  ******** ");
	TEST_PRINT_TRC("Device         : %s", testoptions->device);
	TEST_PRINT_TRC("acnt	 		: %d", testoptions->acnt);
	TEST_PRINT_TRC("bcnt 			: %d", testoptions->bcnt);
	TEST_PRINT_TRC("ccnt 			: %d", testoptions->ccnt);
	TEST_PRINT_TRC("numtcs			: %d", testoptions->numtcs);

		switch (testoptions->ioctl) {
		case EDMA_ASYNC:
			TEST_PRINT_TRC("Transfer Mode	: EDMA_ASYNC");
			break;
		case EDMA_ABSYNC:
			TEST_PRINT_TRC("Transfer Mode	: EDMA_ABSYNC");
			break;
		case EDMA_LINK_ASYNC:
			TEST_PRINT_TRC("Transfer Mode	: EDMA_LINK_ASYNC");
			break;
		case EDMA_LINK_ABSYNC:
			TEST_PRINT_TRC("Transfer Mode	: EDMA_LINK_ABSYNC");
			break;
		case EDMA_CHAIN_ASYNC:
			TEST_PRINT_TRC("Transfer Mode	: EDMA_CHAIN_ASYNC");
			break;
		case EDMA_CHAIN_ABSYNC:
			TEST_PRINT_TRC("Transfer Mode	: EDMA_CHAIN_ABSYNC");
			break;
		}
	
	TEST_PRINT_TRC(" ************* End of Test params ************* ");
}



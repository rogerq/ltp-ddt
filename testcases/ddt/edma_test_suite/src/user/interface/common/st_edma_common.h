/*
 * st_edma_common.h
 *
 * This file contains common definions
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

#ifndef _ST_edma_COMMON_H_
#define _ST_edma_COMMON_H_

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
#include <limits.h>

/*Testcode related Header files*/
#include <st_defines.h>
#include <st_log.h>
#include <st_fileapi.h>
#include <st_timer.h>


#define ST_DEVICE_NAME_LEN 50
#define ST_TESTCASE_ID_LEN  50

#define DEFAULT_IOCTL_NAME    "edma_async"


/*Structure for holding the test options */
struct st_edma_testparams {
	int acnt;
	int bcnt;
	int ccnt;
	int numtcs;
	int perf_flag;
	char device[ST_DEVICE_NAME_LEN];
	char testcaseid[ST_TESTCASE_ID_LEN];
	int ioctl;
};

#define EDMA_ASYNC 0
#define EDMA_ABSYNC 1
#define EDMA_LINK_ASYNC 6
#define EDMA_LINK_ABSYNC 3
#define EDMA_CHAIN_ASYNC 4
#define EDMA_CHAIN_ABSYNC 5


struct edma_ioctl_table {
	char *ioctl_name;
	int ioctl;

};

/* Function Declaratinos */

/*
 * Function                 - st_display_edma_test_suite_help
 * Functionality            - This function displays the help/usage
 * Input Params             - None
 * Return Value             - None
 * Note                         - None
 */
void st_display_edma_test_suite_help(void);

/*
 * Function                 - st_print_edma_test_params
 * Functionality            - This function prints the test option values
 * Input Params             - Test params structure
 * Return Value             - None
 * Note                         - None
 */
void st_print_edma_test_params(struct st_edma_testparams *testoptions,
			      char *test_id);

/*
 * Function         - st_init_edma_test_params
 * Functionality            - This function initilaizes the default values for
 * various test case options(which will be exposed as command line arguments)
 * Input Params             -  None
 * Return Value             -  None
 * Note                         -  None
 */
void st_init_edma_test_params(void);

/*
 * Function             - st_edma_write_test
 * Functionality        - This function recieves the test params and calls
 *                        the write
 * Input Params         -  info,test_id
 * Return Value         -  0: SUCCESS, -1: FAILURE
 * Note                 -  None
 */
int st_edma_write_test(struct st_edma_testparams *info, char *test_id);

/*
 * Function             - st_edma_ioctl_test
 * Functionality        - This function recieves the test params and calls
 *                        the appropariate ioctl
 * Input Params         -  info,test_id
 * Return Value         -  0: SUCCESS, -1: FAILURE
 * Note                 -  None
 */
int st_edma_ioctl_test(struct st_edma_testparams *info, char *test_id);

#endif /*ST_edma_COMMON_H_ */



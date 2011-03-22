/*
 * st_filesystem_common.c
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
#include "st_filesystem_common.h"

/* Storage structures, enums, macros defined */
#define DEFAULT_IOMODE 'w'
#define DEFAULT_FILENAME "mnt/filesystem/file"
#define DEFAULT_BUFFER_SIZE  102400
#define DEFAULT_FILE_SIZE 100

/* Global Variables */
struct st_filesystem_testparams testoptions;

/*
 * Function			- st_init_filesystem_test_params
 * Functionality        	- This function initilaizes the default values for
 * various test case options(which will be exposed as command line arguments)
 * Input Params         	-  None
 * Return Value         	-  None
 * Note                 		-  None
 */
void st_init_filesystem_test_params()
{

	testoptions.buffer_size = DEFAULT_BUFFER_SIZE;
	testoptions.file_size = DEFAULT_FILE_SIZE;
	testoptions.filename = NULL;
	testoptions.iomode = FALSE;

}

/*
 * Function             	- st_display_filesystem_test_suite_help
 * Functionality        	- This function displays the help/usage
 * Input Params         	- None
 * Return Value         	- None
 * Note                 		- None
 */
void st_display_filesystem_test_suite_help(void)
{
	printf( "filesystem_tests -file [-write] [-read] [-buffer_size] [-file_size] [-performance] "
		"[-id] [-h/elp] [-v/ersion] \n\n"
		"-file        :  Name of the file to read from or write to"
		" -/mnt/file\n"
		"-buffer_size :  Application buffer size in bytes [102400]\n"
		"-file_size   :  Total file size in MB [100]\n"
		"-read        :  I/O direction mode is read\n"
		"                no argument value required\n"
		"-write       :  I/O direction mode is write\n"
		"                no argument value required\n"
		"-performance :  Throughput and cpuload measurement\n"
		"                pass the option without any argument\n"
		"                to enable both cpuload and throughput\n"
		"                pass the option with string throughput\n"
		"                to enable throughput only\n"
		"                pass the option with string cpuload\n"
		"                to enable cpuload only\n"
		"-id          :  Test case id string for testers reference\n"
		"                logging purpose\n"
		"-h/elp       :  Displays the help/usage\n"
		"-v/ersion    :  Version of Filesytem Test suite\n");
}

/*
 * Function             	- st_print_filesystem_test_params
 * Functionality        	- This function prints the test option values
 * Input Params         	- Test params structure
 * Return Value         	- None
 * Note                 		- None
 */
void st_print_filesystem_test_params(struct st_filesystem_testparams
				     *testoptions, char *test_id)
{
	/* Start test case */
	TEST_PRINT_TST_START(test_id);
	TEST_PRINT("******** filesystem Testcase  parameters  ******** ");
	TEST_PRINT_TRC("The file name|%s", testoptions->filename);
	TEST_PRINT_TRC("The application buffer size in bytes|%d",
		       testoptions->buffer_size);
	TEST_PRINT_TRC("file size in MB|%d", testoptions->file_size);
	if (testoptions->iomode == 'r') {
		TEST_PRINT_TRC("Iomode|read");
	} else if (testoptions->iomode == 'w') {
		TEST_PRINT_TRC("Iomode|write");
	} else {
		TEST_PRINT_TRC("Iomode|copy");
		TEST_PRINT_TRC("The src file name|%s", testoptions->src);
		TEST_PRINT_TRC("The dst file name|%s", testoptions->dst);
		TEST_PRINT_TRC("duration|%d", testoptions->duration);
	}

	TEST_PRINT("************* End of Test params ***************** ");
}

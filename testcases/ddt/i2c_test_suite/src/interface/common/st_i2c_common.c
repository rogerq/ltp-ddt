/*
 * st_i2c_common.c
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
#include "st_i2c_common.h"

/* Storage structures, enums, macros defined */
#define DEFAULT_IOMODE 'r'
#define DEFAULT_DEVICE_NODE "/dev/i2c-0"
#define DEFAULT_I2C_ADDRESS 0x50
#define DEFAULT_I2C_PAGE_SIZE 64
#define DEFAULT_DEVICE 0	/* eeprom */
#define DEFAULT_BUFFER_SIZE  64
#define DEFAULT_FILE_SIZE 1024

/* Global Variables */
struct st_i2c_testparams testoptions;

/*
 * Function			- st_init_i2c_test_params
 * Functionality        	- This function initilaizes the default values for
 * various test case options(which will be exposed as command line arguments)
 * Input Params         	-  None
 * Return Value         	-  None
 * Note                 		-  None
 */
void st_init_i2c_test_params(void)
{
	testoptions.iomode = DEFAULT_IOMODE;
	testoptions.device_node = DEFAULT_DEVICE_NODE;
	testoptions.device = DEFAULT_DEVICE;
	testoptions.address = DEFAULT_I2C_ADDRESS;
	testoptions.page_size = DEFAULT_I2C_PAGE_SIZE;
	testoptions.buffer_size = DEFAULT_BUFFER_SIZE;
	testoptions.file_size = DEFAULT_FILE_SIZE;

}

/*
 * Function             	- st_display_i2c_test_suite_help
 * Functionality        	- This function displays the help/usage
 * Input Params         	- None
 * Return Value         	- None
 * Note                 		- None
 */
void st_display_i2c_test_suite_help(void)
{
	printf( "i2c_tests -device [-address] [-write] [-read] [-buffer_size]"
		"[-page_size] [-total_size] [-performance]"
		"[-id] [-h/elp] [-v/ersion] \n\n"
		"-device      :  Device-node interface "
		" -/dev/i2c/0\n"
		"-address     :  Application buffer size in bytes [0x50]"
		"-buffer_size :  Application buffer size in bytes [64]\n"
		"-page_size   :  hardware device page size in bytes [64]\n"
		"-total_size  :  Total size in bytes [1024]\n"
		"-read        :  I/O direction mode is read [default mode]\n"
		"                no argument value required\n"
		"-write       :  I/O direction mode is write\n"
		"                no argument value required\n"
		"-performance :  Throughput and cpuload measurement[disabled]\n"
		"                pass the option without any argument\n"
		"                to enable both cpuload and throughput\n"
		"                pass the option with string throughput\n"
		"                to enable throughput only\n"
		"                pass the option with string cpuload\n"
		"                to enable cpuload only\n"
		"-id          :  Test case id string for testers reference\n"
		"                logging purpose\n"
		"-h/elp       :  Displays the help/usage\n"
		"-v/ersion    :  Version of I2C Test suite\n");

}

/*
 * Function             	- st_print_i2c_test_params
 * Functionality        	- This function prints the test option values
 * Input Params         	- Test params structure
 * Return Value         	- None
 * Note                 		- None
 */
void st_print_i2c_test_params(struct st_i2c_testparams
			      *testoptions, char *test_id)
{

	TEST_PRINT_TST_START(test_id);
	TEST_PRINT("******** I2C Testcase  parameters  ******** ");

	if (testoptions->iomode == 'w') {
		TEST_PRINT_TRC("I2C | WRITE");
	} else {
		TEST_PRINT_TRC("I2C | READ");
	}
	TEST_PRINT_TRC("Device Node | %s", testoptions->device_node);
	if (testoptions->device == 0) {
		TEST_PRINT_TRC("I2C DEVICE | EEPROM ");
	} else if (testoptions->device == 1) {
		TEST_PRINT_TRC("I2C DEVICE | AUDIO CODEC ");
	} else {
		TEST_PRINT_TRC("I2C DEVICE | VIDEO CODEC ");
	}
	TEST_PRINT_TRC("Device Address | 0x%x", testoptions->address);
	TEST_PRINT_TRC("Device Page size | %d", testoptions->page_size);
	TEST_PRINT_TRC("The application buffer size in bytes | %d",
		       testoptions->buffer_size);
	TEST_PRINT_TRC("The total file size in bytes | %d",
		       testoptions->file_size);
	TEST_PRINT("************* End of Test params ***************** ");

}

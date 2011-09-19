/*
 * Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation version 2.
 *
 * This program is distributed "as is" WITHOUT ANY WARRANTY of any
 * kind, whether express or implied; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
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
#define DEFAULT_SRCFILE_SIZE 10

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
	testoptions.srcfile_size = DEFAULT_SRCFILE_SIZE;
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
	printf( "filesystem_tests -file [-write] [-read] [-buffer_size] [-file_size] [-srcfile_size] [-performance] "
		"[-id] [-h/elp] [-v/ersion] \n\n"
		"-file        :  Name of the file to read from or write to"
		" -/mnt/file\n"
		"-buffer_size :  Application buffer size in bytes [102400]\n"
		"-file_size   :  Total file size in MB [100], it should be divisible by srcfile_size\n"
		"-srcfile_size   :  Src file size in MB [10]\n"
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
	TEST_PRINT_TRC("file size in MB|%.2f", testoptions->file_size);
	if (testoptions->iomode == 'r') {
		TEST_PRINT_TRC("Iomode|read");
	} else if (testoptions->iomode == 'w') {
		TEST_PRINT_TRC("Iomode|write");
		TEST_PRINT_TRC("srcfile size in MB|%.2f", testoptions->srcfile_size);
	} else {
		TEST_PRINT_TRC("Iomode|copy");
		TEST_PRINT_TRC("The src file name|%s", testoptions->src);
		TEST_PRINT_TRC("The dst file name|%s", testoptions->dst);
		TEST_PRINT_TRC("duration|%d", testoptions->duration);
	}

	TEST_PRINT("************* End of Test params ***************** ");
}

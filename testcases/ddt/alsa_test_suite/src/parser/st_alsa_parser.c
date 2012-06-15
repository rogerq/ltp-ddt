/*
 * st_alsa_parser.c 
 *
 * This file is parser for alsa test suite
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

#include "st_alsa_common.h"
#include "st_alsa_parser.h"

/* 
 *Test case options structure 
 */
extern tc_dev_params testoptions_capture;
extern tc_dev_params testoptions_playback;

/* 
 *Declare global variables here 
 */
static int quit = FALSE;
int error = TRUE;
static int play_or_record = FALSE;
char *testcaseid = "alsa_perf_tests";
char *testname = "alsa_perf_tests";

/* 
 *Function parses the command line options 
 */
static void st_process_alsa_test_options(int argc, char **argv);

/* 
 *Function to print the test suite help 
 */
static void st_alsa_test_suite_help(void);

/* 
 *Function to print the test params 
 */
void st_alsa_print_test_params(tc_dev_params * test_opt);

/*
 * Function             - st_alsa_test_suite_help
 * Functionality        - This function prints the usage help for the test suite
 * Input Params         -  None
 * Return Value         -  None
 * Note                 -  None
 */
static void st_alsa_test_suite_help(void)
{
	printf
	    ("alsa_tests -playback -record -[file] -[access] [-channel] [-device] [-totalsize] [-format] [-performance] "
	     "-[nonblock] [-periodsize] [-rate] [-id] [-h/elp] [-v/ersion] \n\n"
	     "-access=# 	ACCESS TYPE TO PCM, [3]\n"
	     "\t  	0= mmap interleaved\n"
	     "\t  	1= mmap non interleaved\n"
	     "\t  	2= mmap complex\n"
	     "\t  	3= rw interleaved\n"
	     "\t  	4= rw non interleaved\n"
	     "-channel=# 	CHANNELS [2]\n"
	     "\t  	1= Mono\n"
	     "\t  	2= Stereo\n"
	     "-device=NAME	select PCM by name [hw:0,0]\n"
	     "-file=Name	Name of file [/mnt/default_capture.raw]  \n"
	     "-totalsize=#	Total size of the auido data to capture/playback [10485760]  \n"
	     "-format=#  	SAMPLE FORMAT [4]\n"
	     "\t   	0= signed 8 bit\n"
	     "\t   	1= unsigned 8 bit\n"
	     "\t   	2= signed 16 bit LE\n"
	     "\t   	3= signed 16 bit BE\n"
	     "\t   	4= unsigned 16 bit LE\n"
	     "\t   	5= signed 16 bit BE\n"
	     "\t   	6= signed 24 bit LE\n"
	     "\t   	7= signed 24 bit BE\n"
	     "\t   	8= unsigned 24 bit LE\n"
	     "\t   	9= unsigned 24 bit BE\n"
	     "\t  	10= signed 32 bit LE\n"
	     "\t  	11= signed 32 bit BE\n"
	     "\t  	12= unsigned 32 bit LE\n"
	     "\t  	13= unsigned 32 bit BE\n"
	     "-nonblock	Non Blocking Mode \n"
	     "-periodsize=#Distance between interrupts is # frames [64]\n"
	     "-rate=# 		Sampling Rate [44100] \n"
	     "-playback=#	Playback\n"
	     "\t		0= Playback from buffer (sine wave)\n"
	     "\t		1= Playback from file\n"
	     "-record=#	Record\n"
	     "\t		0= Capture to a buffer\n"
	     "\t		1= Capture to a file\n"
	     "-performance :  Throughput and cpuload measurement\n"
	     "				 pass the option without any argument\n"
	     "				 to enable both cpuload and throughput\n"
	     "				 pass the option with string throughput\n"
	     "				 to enable throughput only\n"
	     "				 pass the option with string cpuload\n"
	     "				 to enable cpuload only\n"
	     "-id		  :  Test case id string for testers reference \n"
	     "				 logging purpose [alsa_perf_tests]\n"
	     "-h/elp 	  :  Displays the help/usage\n"
	     "-v/ersion	  :  Version of Filesytem Test suite\n");
}

/*
 * Function		- st_display_alsa_testsuite_version
 * Functionality	- This function displays the test suite version
 * Input Params		- None 
 * Return Value		- None
 * Note			- None
 */
void st_display_alsa_testsuite_version()
{
	printf("ALSA TestSuite V %s\n", VERSION_STRING);
}

/*
 * Function             - Main function
 * Functionality       	- This is where the execution begins
 * Input Params      	- argc,argv
 * Return Value         - None
 * Note                 - None
 */
int main(int argc, char *argv[])
{
	int ret = SUCCESS;

	/*Initializing capture default options */
	st_alsa_init_capture_test_params();

	/*Initializing playback default options */
	st_alsa_init_playback_test_params();

	/*Processing the command line options */
	st_process_alsa_test_options(argc, argv);

	/* Initialize Timer module */
	init_timer_module();
	if (quit == FALSE && error == FALSE) {
		if (play_or_record == OPTION_PLAYBACK) {
			st_alsa_print_test_params(&testoptions_playback);
			if (testoptions_playback.play_from_file == TRUE)
				ret =
				    st_audio_file_playback
				    (&testoptions_playback);

			else
				ret = st_audio_playback(&testoptions_playback);
		}

		else if (play_or_record == OPTION_RECORD) {
			st_alsa_print_test_params(&testoptions_capture);
			if (testoptions_capture.capture_to_file == TRUE)
				ret =
				    st_audio_file_capture(&testoptions_capture);

			else
				ret = st_audio_capture(&testoptions_capture);
		} else {
			st_alsa_test_suite_help();
		}
	} else {
		st_alsa_test_suite_help();
	}
	if (quit == FALSE && error == FALSE) {
		TEST_PRINT_TST_RESULT(ret, testcaseid);
		TEST_PRINT_TST_END(testcaseid);
	}
	return ret;
}

/*
 * Function             - st_process_alsa_test_options
 * Functionality        - This function parses the command line options and values passed for the options
 * Input Params         -  argc,argv
 * Return Value         -  None
 * Note                 -  None
 */
static void st_process_alsa_test_options(int argc, char **argv)
{
	for (;;) {
		int option_index = 0;

		/*
		 * Options for getopt - New test case options added need to be
		 * populated here
		 */
		static struct option long_options[] = {
			{"access", required_argument, NULL, OPTION_ACCESS},
			{"channel", required_argument, NULL, OPTION_CHANNEL},
			{"device", required_argument, NULL, OPTION_DEVICE},
			{"format", required_argument, NULL, OPTION_FORMAT},
			{"file", required_argument, NULL, OPTION_FILE},
			{"periodsize", required_argument, NULL,
			 OPTION_PERIODSIZE},
			{"totalsize", required_argument, NULL,
			 OPTION_TOTALSIZE},
			{"rate", required_argument, NULL, OPTION_RATE},
			{"playback", required_argument, NULL, OPTION_PLAYBACK},
			{"record", required_argument, NULL, OPTION_RECORD},
			{"loopback", no_argument, NULL, OPTION_LOOPBACK},
			{"nonblock", no_argument, NULL, OPTION_NONBLOCK},
			{"performance", optional_argument, NULL,
			 OPTION_PERFORMANCE},
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
			quit = TRUE;
			break;
		}
		switch (c) {
		case OPTION_ACCESS:
			if (optarg != NULL) {
				testoptions_capture.access_type = atoi(optarg);
				testoptions_playback.access_type = atoi(optarg);
			} else if (optind < argc && ('-' != argv[optind][0])) {
				testoptions_capture.access_type =
				    atoi(argv[optind]);
				testoptions_playback.access_type = atoi(optarg);
			}
			break;
		case OPTION_CHANNEL:
			if (optarg != NULL) {
				testoptions_capture.channel = atoi(optarg);
				testoptions_playback.channel = atoi(optarg);
			} else if (optind < argc && ('-' != argv[optind][0])) {
				testoptions_capture.channel =
				    atoi(argv[optind]);
				testoptions_playback.channel =
				    atoi(argv[optind]);
			}
			break;
		case OPTION_DEVICE:
			if (optarg != NULL) {
				testoptions_capture.card_name = optarg;
				testoptions_playback.card_name = optarg;
			} else if (optind < argc && ('-' != argv[optind][0])) {
				testoptions_capture.card_name = argv[optind];
				testoptions_playback.card_name = argv[optind];
			}
			break;
		case OPTION_FORMAT:
			if (optarg != NULL) {
				testoptions_capture.format = atoi(optarg);
				testoptions_playback.format = atoi(optarg);
			} else if (optind < argc && ('-' != argv[optind][0])) {
				testoptions_capture.format = atoi(argv[optind]);
				testoptions_playback.format =
				    atoi(argv[optind]);
			}
			break;
		case OPTION_FILE:
			if (optarg != NULL) {
				testoptions_capture.file_name = optarg;
				testoptions_playback.file_name = optarg;
			} else if (optind < argc && ('-' != argv[optind][0])) {
				testoptions_capture.file_name = argv[optind];
				testoptions_playback.file_name = argv[optind];
			}
			break;
		case OPTION_NONBLOCK:
			if (NULL != optarg) {
				testoptions_capture.io_opmode = NON_BLOCKING;
				testoptions_playback.io_opmode = NON_BLOCKING;
			} else if (optind < argc && argv[optind]) {
				testoptions_capture.io_opmode = NON_BLOCKING;
				testoptions_playback.io_opmode = NON_BLOCKING;
			}
			break;
		case OPTION_PERIODSIZE:
			if (optarg != NULL) {
				testoptions_capture.period_size = atoi(optarg);
				testoptions_playback.period_size = atoi(optarg);
			} else if (optind < argc && ('-' != argv[optind][0])) {
				testoptions_capture.period_size =
				    atoi(argv[optind]);
				testoptions_playback.period_size =
				    atoi(argv[optind]);
			}
			break;
		case OPTION_TOTALSIZE:
			if (optarg != NULL) {
				testoptions_capture.total_size = atoi(optarg);
				testoptions_playback.total_size = atoi(optarg);
			} else if (optind < argc && ('-' != argv[optind][0])) {
				testoptions_capture.total_size =
				    atoi(argv[optind]);
				testoptions_playback.total_size =
				    atoi(argv[optind]);
			}
			break;
		case OPTION_RATE:
			if (optarg != NULL) {
				testoptions_capture.sampling_rate =
				    atoi(optarg);
				testoptions_playback.sampling_rate =
				    atoi(optarg);
			} else if (optind < argc && ('-' != argv[optind][0])) {
				testoptions_capture.sampling_rate =
				    atoi(argv[optind]);
				testoptions_playback.sampling_rate =
				    atoi(argv[optind]);
			}
			break;
		case OPTION_HELP:
			st_alsa_test_suite_help();
			quit = TRUE;
		case OPTION_VERSION:
			quit = TRUE;
			st_display_alsa_testsuite_version();
			break;
		case OPTION_PLAYBACK:
			error = FALSE;
			play_or_record = OPTION_PLAYBACK;
			if (optarg != NULL) {
				testoptions_playback.play_from_file =
				    atoi(optarg);
			} else if (optind < argc && ('-' != argv[optind][0])) {
				testoptions_playback.play_from_file =
				    atoi(argv[optind]);
			}
			break;
		case OPTION_RECORD:
			error = FALSE;
			play_or_record = OPTION_RECORD;
			if (optarg != NULL) {
				testoptions_capture.capture_to_file =
				    atoi(optarg);
			} else if (optind < argc && ('-' != argv[optind][0])) {
				testoptions_capture.capture_to_file =
				    atoi(argv[optind]);
			}
			break;
		case OPTION_PERFORMANCE:
			testoptions_playback.throughput_flag = TRUE;
			testoptions_playback.cpuload_flag = TRUE;
			testoptions_capture.throughput_flag = TRUE;
			testoptions_capture.cpuload_flag = TRUE;
			if (optarg != NULL) {
				if (strcmp(optarg, THROUGHPUT) == 0) {
					testoptions_playback.throughput_flag =
					    TRUE;
					testoptions_playback.cpuload_flag =
					    FALSE;
					testoptions_capture.throughput_flag =
					    TRUE;
					testoptions_capture.cpuload_flag =
					    FALSE;
				} else if (strcmp(optarg, CPULOAD) == 0) {
					testoptions_playback.throughput_flag =
					    FALSE;
					testoptions_playback.cpuload_flag =
					    TRUE;
					testoptions_capture.throughput_flag =
					    FALSE;
					testoptions_capture.cpuload_flag = TRUE;
				}
			} else if (optind < argc && ('-' != argv[optind][0])) {
				if (strcmp(argv[optind], THROUGHPUT) == 0) {
					testoptions_playback.throughput_flag =
					    TRUE;
					testoptions_playback.cpuload_flag =
					    FALSE;
					testoptions_capture.throughput_flag =
					    TRUE;
					testoptions_capture.cpuload_flag =
					    FALSE;
				} else if (strcmp(argv[optind], CPULOAD) == 0) {
					testoptions_playback.throughput_flag =
					    FALSE;
					testoptions_playback.cpuload_flag =
					    TRUE;
					testoptions_capture.throughput_flag =
					    FALSE;
					testoptions_capture.cpuload_flag = TRUE;
				}
			}
			break;
		case OPTION_TESTCASE_ID:
			if (optarg != NULL) {
				testcaseid = optarg;
			} else if (optind < argc && ('-' != argv[optind][0])) {
				testcaseid = argv[optind];
			}
			break;
		}
	}
}

/*
 * Function             - st_alsa_print_test_params
 * Functionality        - This function prints the test params
 * Input Params         -  test_opt
 * Return Value         -  None
 * Note                 -  None
 */
void st_alsa_print_test_params(tc_dev_params * test_opt)
{
	char *format = NULL;
	char *access = NULL;
	switch (test_opt->format) {
	case PCM_FORMAT_S8:
		format = "S8";
		break;
	case PCM_FORMAT_S16_LE:
		format = "S16_LE";
		break;
	case PCM_FORMAT_S16_BE:
		format = "S16_BE";
		break;
	case PCM_FORMAT_U16_LE:
		format = "U16_LE";
		break;
	case PCM_FORMAT_U16_BE:
		format = "U16_BE";
		break;
	case PCM_FORMAT_S24_LE:
		format = "S24_LE";
		break;
	case PCM_FORMAT_S24_BE:
		format = "S24_BE";
		break;
	case PCM_FORMAT_U24_LE:
		format = "U24_LE";
		break;
	case PCM_FORMAT_U24_BE:
		format = "U24_BE";
		break;
	case PCM_FORMAT_S32_BE:
		format = "S32_BE";
		break;
	case PCM_FORMAT_S32_LE:
		format = "S32_LE";
		break;
	case PCM_FORMAT_U32_BE:
		format = "U32_BE";
		break;
	case PCM_FORMAT_U32_LE:
		format = "U32_LE";
		break;
	}
	switch (test_opt->access_type) {
	case PCM_ACCESS_MMAP_INTERLEAVED:
		access = "MMAP_INTERLEAVED";
		break;
	case PCM_ACCESS_MMAP_NONINTERLEAVED:
		access = "MMAP_NONINTERLEAVED";
		break;
	case PCM_ACCESS_MMAP_COMPLEX:
		access = "MMAP_COMPLEX";
		break;
	case PCM_ACCESS_RW_INTERLEAVED:
		access = "RW_INTERLEAVED";
		break;
	case PCM_ACCESS_RW_NONINTERLEAVED:
		access = "RW_NONINTERLEAVED";
		break;
	}
	TEST_PRINT_TST_START(testcaseid);
	TEST_PRINT("******** ALSA Testcase  parameters  ******** ");
	if (play_or_record == OPTION_RECORD) {
		TEST_PRINT_TRC
		    ("CAPTURE Test is going to start with following values ");
	} else if (play_or_record == OPTION_PLAYBACK) {
		TEST_PRINT_TRC
		    ("PLAYBACK Test is going to start with following values ");
	} else {
		TEST_PRINT_TRC
		    ("LOOPBACK Test is going to start with following values ");
	}
	TEST_PRINT_TRC("Card name		|%s", test_opt->card_name);
	TEST_PRINT_TRC("Device Number	|%d", test_opt->device);
	TEST_PRINT_TRC("Access Type		|%s", access);
	TEST_PRINT_TRC("Period Size		|%d", test_opt->period_size);
	TEST_PRINT_TRC("Sampling Rate	|%d", test_opt->sampling_rate);
	TEST_PRINT_TRC("Channels		|%d", test_opt->channel);
	TEST_PRINT_TRC("format		|%s", format);
	if (test_opt->capture_to_file == TRUE
	    || test_opt->play_from_file == TRUE) {
		TEST_PRINT_TRC("File name		|%s",
			       test_opt->file_name);
	}
	TEST_PRINT_TRC("Total Size		|%d", test_opt->total_size);
	TEST_PRINT("************* End of Test params ***************** ");
}

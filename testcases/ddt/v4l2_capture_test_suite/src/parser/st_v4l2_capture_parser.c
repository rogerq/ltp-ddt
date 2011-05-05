/*
 * st_v4l2_parser.c
 *
 * This file contains the parser code for v4l2 driver
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

#include "st_v4l2_capture_parser.h"
#include <st_v4l2_capture_common.h>

/*
* This function parses the command line options
* and values passed for the options
*/
static int process_v4l2_capture_test_options(int argc, char *argv[])
{
	int version = FALSE, help = FALSE;
	int retVal = SUCCESS;
	int ioctl_test = FALSE;

	for (;;) {
		int option_index = 0, c;
		/* Options for getopt */
		static struct option long_options[] = {
			{"device_cap", optional_argument,
				NULL, OPTION_DEVICE_NAME_CAP},
			{"width", optional_argument,
				NULL, OPTION_WIDTH},
			{"height", optional_argument,
				NULL, OPTION_HEIGHT},
			{"top", optional_argument,
				NULL, OPTION_TOP},
			{"left", optional_argument,
				NULL, OPTION_LEFT},
			{"control_value", optional_argument,
				NULL, OPTION_VALUE},
			{"control_id", optional_argument,
				NULL, OPTION_ID},
			{"crop_height", optional_argument,
				NULL, OPTION_CROP_HEIGHT},
			{"crop_width", optional_argument,
				NULL, OPTION_CROP_WIDTH},
			{"crop_top", optional_argument,
				NULL, OPTION_CROP_TOP},
			{"crop_left", optional_argument,
				NULL, OPTION_CROP_LEFT},
			{"fmt_height", optional_argument,
				NULL, OPTION_FMT_HEIGHT},
			{"fmt_width", optional_argument,
				NULL, OPTION_FMT_WIDTH},
			{"fmt_top", optional_argument,
				NULL, OPTION_FMT_TOP},
			{"fmt_left", optional_argument,
				NULL, OPTION_FMT_LEFT},
			{"pixelformat", optional_argument,
				NULL, OPTION_PIXELFMT},
			{"id", optional_argument,
				NULL, OPTION_TESTID},
			{"loop", no_argument,
				NULL, OPTION_LOOP},
			{"ioctl", optional_argument,
				NULL, OPTION_IOCTL_TEST},
			{"crtl_value", optional_argument,
				NULL, OPTION_CRTL_VAL},
			{"crtl_id", optional_argument,
				NULL, OPTION_CRTL_ID},				
			{"buffernos", no_argument,
				NULL, OPTION_BUFNOS},
			{"device_disp", optional_argument,
				NULL, OPTION_DEVICE_NAME_DISP},
			{"buf_type", no_argument,
				NULL, OPTION_BUFTYPE},
			{"cpuload", no_argument,
				NULL, OPTION_CPULOAD},
			{"framerate", no_argument,
				NULL, OPTION_FRAMERATE},
			{"userptr", no_argument,
				NULL, OPTION_USERPTR},
			{"version", no_argument,
				NULL, OPTION_VERSION},
			{"help", no_argument,
				NULL, OPTION_HELP},
			{NULL, 0, NULL, 0}
		};

		c = getopt_long_only(argc, argv, "v::h::", long_options,
				&option_index);
		if (c == -1)
			break;

		switch (c) {
		case OPTION_DEVICE_NAME_CAP:
			if (optarg != NULL)
				testoptions.device_name_cap = optarg;
			else if (optind < argc && argv[optind])
				testoptions.device_name_cap = argv[optind];
			break;
		case OPTION_WIDTH:
			if (optarg != NULL)
				testoptions.width = atoi(optarg);
			else if (optind < argc && argv[optind])
				testoptions.width = atoi(argv[optind]);
			break;
		case OPTION_HEIGHT:
			if (optarg != NULL)
				testoptions.height = atoi(optarg);
			else if (optind < argc && argv[optind])
				testoptions.height = atoi(argv[optind]);
			break;
		case OPTION_TOP:
			if (optarg != NULL)
				testoptions.top = atoi(optarg);
			else if (optind < argc && argv[optind])
				testoptions.top = atoi(argv[optind]);
			break;
		case OPTION_LEFT:
			if (optarg != NULL)
				testoptions.left = atoi(optarg);
			else if (optind < argc && argv[optind])
				testoptions.left = atoi(argv[optind]);
			break;
		case OPTION_VALUE:
			if (optarg != NULL)
				testoptions.st_control.value = atoi(optarg);
			else if (optind < argc && argv[optind])
				testoptions.st_control.value =
					atoi(argv[optind]);
			break;
		case OPTION_ID:
			{
				int cntrl_id = 0;
				if (optarg != NULL)
					cntrl_id = atoi(optarg);
				else if (optind < argc && argv[optind])
					cntrl_id = atoi(argv[optind]);

				if (cntrl_id == 1)
					testoptions.st_control.id =
						V4L2_CID_ROTATE;
				else if (cntrl_id == 2)
					testoptions.st_control.id =
						V4L2_CID_BG_COLOR;
			}
			break;
		case OPTION_CROP_HEIGHT:
			if (optarg != NULL)
				testoptions.crop_height = atoi(optarg);
			else if (optind < argc && argv[optind])
				testoptions.crop_height = atoi(argv[optind]);
			break;
		case OPTION_CROP_WIDTH:
			if (optarg != NULL)
				testoptions.crop_width = atoi(optarg);
			else if (optind < argc && argv[optind])
				testoptions.crop_width = atoi(argv[optind]);
			break;
		case OPTION_CROP_TOP:
			if (optarg != NULL)
				testoptions.crop_top = atoi(optarg);
			else if (optind < argc && argv[optind])
				testoptions.crop_top = atoi(argv[optind]);
			break;
		case OPTION_CROP_LEFT:
			if (optarg != NULL)
				testoptions.crop_left = atoi(optarg);
			else if (optind < argc && argv[optind])
				testoptions.crop_left = atoi(argv[optind]);
			break;
		case OPTION_FMT_HEIGHT:
			if (optarg != NULL)
				testoptions.fmt_height = atoi(optarg);
			else if (optind < argc && argv[optind])
				testoptions.fmt_height = atoi(argv[optind]);
			break;
		case OPTION_FMT_WIDTH:
			if (optarg != NULL)
				testoptions.fmt_width = atoi(optarg);
			else if (optind < argc && argv[optind])
				testoptions.fmt_width = atoi(argv[optind]);
			break;
		case OPTION_FMT_TOP:
			if (optarg != NULL)
				testoptions.fmt_top = atoi(optarg);
			else if (optind < argc && argv[optind])
				testoptions.fmt_top = atoi(argv[optind]);
			break;
		case OPTION_FMT_LEFT:
			if (optarg != NULL)
				testoptions.fmt_left = atoi(optarg);
			else if (optind < argc && argv[optind])
				testoptions.fmt_left = atoi(argv[optind]);
			break;
		case OPTION_PIXELFMT:
			if (optarg != NULL)
				testoptions.pixelformat = atoi(optarg);
			else if (optind < argc && argv[optind])
				testoptions.pixelformat = atoi(argv[optind]);
			break;
		case OPTION_TESTID:
			if (optarg != NULL)
				testoptions.testcase_id = optarg;
			else if (optind < argc && argv[optind])
				testoptions.testcase_id = argv[optind];
			break;
		case OPTION_BUFNOS:
			if (optarg != NULL)
				testoptions.st_reqbuf_disp.count =
					atoi(optarg);
			else if (optind < argc && argv[optind])
				testoptions.st_reqbuf_disp.count =
					atoi(argv[optind]);
			break;
		case OPTION_BUFTYPE:
			{
				int buf_type = 0;
				testoptions.buffer_type =
					V4L2_BUF_TYPE_VIDEO_OUTPUT;

				if (optarg != NULL)
					buf_type = atoi(optarg);
				else if (optind < argc && argv[optind])
					buf_type = atoi(argv[optind]);

				if (buf_type == 1)
					testoptions.buffer_type =
						V4L2_BUF_TYPE_VIDEO_OUTPUT;
				else if (buf_type == 2)
					testoptions.buffer_type =
						V4L2_BUF_TYPE_VIDEO_OVERLAY;
				else if (buf_type == 3)
					testoptions.buffer_type =
						V4L2_BUF_TYPE_VIDEO_CAPTURE;
			}
			break;
		case OPTION_LOOP:
			if (optarg != NULL)
				testoptions.loop = atoi(optarg);
			else if (optind < argc && argv[optind])
				testoptions.loop = atoi(argv[optind]);
			break;
		case OPTION_IOCTL_TEST:
			if (optarg != NULL)
				testoptions.ioctl_no = atoi(optarg);
			else if (optind < argc && argv[optind])
				testoptions.ioctl_no = atoi(argv[optind]);
			ioctl_test = TRUE;
			break;
		case OPTION_CRTL_ID:
			if (optarg != NULL)
				testoptions.st_control.id = atoi(optarg);
			else if (optind < argc && argv[optind])
				testoptions.st_control.id = atoi(argv[optind]);
			break;
		case OPTION_CRTL_VAL:
			if (optarg != NULL)
				testoptions.st_control.value = atoi(optarg);
			else if (optind < argc && argv[optind])
				testoptions.st_control.value = atoi(argv[optind]);
			break;			
		case OPTION_DEVICE_NAME_DISP:
			if (optarg != NULL)
				testoptions.device_name_disp = optarg;
			else if (optind < argc && argv[optind])
				testoptions.device_name_disp = argv[optind];
			break;
		case OPTION_CPULOAD:
			testoptions.cpuload = TRUE;
			break;
		case OPTION_FRAMERATE:
			testoptions.framerate = TRUE;
			break;
		case OPTION_USERPTR:
			testoptions.userptr = TRUE;
			break;
		case OPTION_VERSION:
		case 'v':
		case 'V':
			display_v4l2_capture_testsuite_version();
			version = TRUE;
			break;
		case OPTION_HELP:
		case 'h':
		case 'H':
		default:
			display_v4l2_capture_test_suite_help();
			help = TRUE;
			break;
		}
	}
	if (ioctl_test == TRUE)
		retVal = st_v4l2_capture_ioctl_test(&testoptions);
	else if (help != TRUE && version != TRUE)
		retVal = st_v4l2_capture_test(&testoptions);

	return retVal;
}


/* This function displays the test suite version */
void display_v4l2_capture_testsuite_version(void)
{
	printf("v4l2CaptureTestSuite V %s\n", VERSION_STRING);
}

/* This is where the execution begins */
int main(int argc, char **argv)
{
	/* Initialize options with default vales */
	init_v4l2_capture_test_params(&testoptions);

	/* Invoke the parser function to process the command line options */
	return process_v4l2_capture_test_options(argc, argv);
}

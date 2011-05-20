/*
 * st_fbdev_parser.c
 *
 * This file contains fbdev parser and gives user selectable test cases
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

/*
 * Header files
 */
#include <st_fbdev_common.h>
#include <st_fbdev_options.h>

/*
 * Default settings/configuration
 */
#define DEFAULT_NODE		"/dev/fb0"
#define DEFAULT_LOOP_CNT	1

/*
 * Main data structure
 */
static struct fbdev_display_testparams testoptions = {
	.device_name	= DEFAULT_NODE,
	.loopcount	= DEFAULT_LOOP_CNT,
	.framerate	= FALSE,
	.cpuload	= FALSE,
	.output		= -1,
	.file_name	= NULL,
};

/*
 * Parse RGBA values
 */
static int getColorParams(char** opt, struct fb_bitfield *color)
{
	char *ptr;

	ptr = (char*)(*opt);
	if (!ptr)
		goto exit;

	color->length = strtoul(ptr, &ptr, 0);
	if (!ptr)
		goto exit;

	if (*ptr == '/')
		color->offset = strtoul(ptr+1, &ptr, 0);

	if (ptr) {
		while (*ptr && isspace(*ptr))
			ptr++;
		if (*ptr == ',') {
			ptr++;
		} else if (*ptr) {
			goto exit;
		}
	}

	*opt = ptr;
	return 0;
exit:
	TEST_PRINT_ERR("Bad RGBA syntax: rL/rO,gL/gO,bL/bO,tL/tO or"
				" rL,gL,bL,tL\n");
	return -EINVAL;
}

/*
 * This function parses the command line options and values passed for the options
 */
static int st_process_fbdev_display_test_options(int argc, char **argv)
{
	int ret = SUCCESS;
	int version = FALSE, help = FALSE;
	int ioctl_test = FALSE, pan_display = FALSE, ck_display = FALSE, alpha_display = FALSE;

	/* Initialize options with default vales, so ingnoring return value */
	st_init_fbdev_display_test_params(&testoptions);

	for (;;) {
		int option_index = 0, c;
		/* Options for getopt */
		static struct option long_options[] = {
			{"device", optional_argument, NULL, OPTION_DEVICE_NAME},
			{"width", optional_argument, NULL, OPTION_WIDTH},
			{"height", optional_argument, NULL, OPTION_HEIGHT},
			{"virtual_width", optional_argument, NULL,
				OPTION_VIRTUAL_WIDTH},
			{"virtual_height", optional_argument, NULL,
				OPTION_VIRTUAL_HEIGHT},
			{"bits_per_pixel", optional_argument, NULL,
				OPTION_BPP},
			{"left", optional_argument, NULL, OPTION_LEFT_MARGIN},
			{"right", optional_argument, NULL, OPTION_RIGHT_MARGIN},
			{"lower", optional_argument, NULL, OPTION_LOWER_MARGIN},
			{"upper", optional_argument, NULL, OPTION_UPPER_MARGIN},
			{"hsynclen", optional_argument, NULL, OPTION_HSYNC_LEN},
			{"vsynclen", optional_argument, NULL, OPTION_VSYNC_LEN},
			{"grayscale", optional_argument, NULL,
				OPTION_GRAYSCALE},
			{"rotate", optional_argument, NULL, OPTION_ROTATE},
			{"mirror", optional_argument, NULL, OPTION_MIRROR},
			{"rgba", optional_argument, NULL, OPTION_RGBA_OFF},
			{"pan", optional_argument, NULL, OPTION_PAN},
			{"nonstd", optional_argument, NULL,
				OPTION_NONSTD_PIXELFMT},
			{"file", optional_argument, NULL, OPTION_FILE_NAME},
			{"output", optional_argument, NULL, OPTION_OUTPUT},
			{"ioctl", optional_argument, NULL, OPTION_IOCTL_TEST},
			{"loop", optional_argument, NULL, OPTION_LOOP_CNT},
			{"id", optional_argument, NULL, OPTION_TESTCASE_ID},
			{"framerate", no_argument, NULL, OPTION_FRAME_RATE},
			{"cpuload", no_argument, NULL, OPTION_CPU_LOAD},
                        {"colorkey", no_argument, NULL, OPTION_COLORKEY},
                        {"alphablend", no_argument, NULL, OPTION_ALPHA},
			{"version", no_argument, NULL, OPTION_VERSION},
			{"help", no_argument, NULL, OPTION_HELP},
			{NULL, 0, NULL, 0}
		};

		c = getopt_long_only(argc, argv, "v::h::", long_options,
				&option_index);
		if (c == -1)
			break;

		switch (c) {
		case OPTION_DEVICE_NAME:
			if (optarg != NULL)
				testoptions.device_name = optarg;
			else if (optind < argc && argv[optind])
				testoptions.device_name = argv[optind];

			/* Device node has changed: re-initialize */
			st_init_fbdev_display_test_params(&testoptions);

			break;

		case OPTION_WIDTH:
			if (optarg != NULL)
				testoptions.var.xres = atoi(optarg);
			else if (optind < argc && argv[optind])
				testoptions.var.xres = atoi(argv[optind]);

			break;

		case OPTION_HEIGHT:
			if (optarg != NULL)
				testoptions.var.yres = atoi(optarg);
			else if (optind < argc && argv[optind])
				testoptions.var.yres = atoi(argv[optind]);

			break;

		case OPTION_VIRTUAL_WIDTH:
			if (optarg != NULL)
				testoptions.var.xres_virtual = atoi(optarg);
			else if (optind < argc && argv[optind])
				testoptions.var.xres_virtual = atoi(argv[optind]);
			break;

		case OPTION_VIRTUAL_HEIGHT:
			if (optarg != NULL)
				testoptions.var.yres_virtual = atoi(optarg);
			else if (optind < argc && argv[optind])
				testoptions.var.yres_virtual = atoi(argv[optind]);
			break;

		case OPTION_BPP:
			if (optarg != NULL)
				testoptions.var.bits_per_pixel = atoi(optarg);
			else if (optind < argc && argv[optind])
				testoptions.var.bits_per_pixel = atoi(argv[optind]);
			break;
		case OPTION_LEFT_MARGIN:
			if (optarg != NULL)
				testoptions.var.left_margin = atoi(optarg);
			else if (optind < argc && argv[optind])
				testoptions.var.left_margin = atoi(argv[optind]);
			break;
		case OPTION_RIGHT_MARGIN:
			if (optarg != NULL)
				testoptions.var.right_margin = atoi(optarg);
			else if (optind < argc && argv[optind])
				testoptions.var.right_margin = atoi(argv[optind]);
			break;
		case OPTION_LOWER_MARGIN:
			if (optarg != NULL)
				testoptions.var.lower_margin = atoi(optarg);
			else if (optind < argc && argv[optind])
				testoptions.var.lower_margin = atoi(argv[optind]);
			break;
		case OPTION_UPPER_MARGIN:
			if (optarg != NULL)
				testoptions.var.upper_margin = atoi(optarg);
			else if (optind < argc && argv[optind])
				testoptions.var.upper_margin = atoi(argv[optind]);
			break;
		case OPTION_HSYNC_LEN:
			if (optarg != NULL)
				testoptions.var.hsync_len = atoi(optarg);
			else if (optind < argc && argv[optind])
				testoptions.var.hsync_len = atoi(argv[optind]);
			break;
		case OPTION_VSYNC_LEN:
			if (optarg != NULL)
				testoptions.var.vsync_len = atoi(optarg);
			else if (optind < argc && argv[optind])
				testoptions.var.vsync_len = atoi(argv[optind]);
			break;
		case OPTION_GRAYSCALE:
			if (optarg != NULL)
				testoptions.var.grayscale = atoi(optarg);
			else if (optind < argc && argv[optind])
				testoptions.var.grayscale = atoi(argv[optind]);
			break;
		case OPTION_ROTATE:
			if (optarg != NULL)
				testoptions.var.rotate = atoi(optarg);
			else if (optind < argc && argv[optind])
				testoptions.var.rotate = atoi(argv[optind]);
			break;
		case OPTION_MIRROR:
			if (optarg != NULL)
				testoptions.mirror = atoi(optarg);
			else if (optind < argc && argv[optind])
				testoptions.mirror = atoi(argv[optind]);

			TEST_PRINT_ERR("Not Supported\n");
			break;
		case OPTION_RGBA_OFF:
			{
				int ret;
				char *ptr;
				struct fb_var_screeninfo *var;

				if (optarg != NULL)
					ptr = optarg;
				else if (optind < argc && argv[optind])
					ptr = argv[optind];
				var = &testoptions.var;

				ret = getColorParams(&ptr, &var->red);
				if (ret)
					exit(-1);
				ret = getColorParams(&ptr, &var->green);
				if (ret)
					exit(-1);
				ret = getColorParams(&ptr, &var->blue);
				if (ret)
					exit(-1);
				ret = getColorParams(&ptr, &var->transp);
				if (ret)
					exit(-1);
			}
			break;
		case OPTION_PAN:
			{
				char *ptr;
				struct fb_var_screeninfo *var;

				if (optarg != NULL)
					ptr = optarg;
				else if (optind < argc && argv[optind])
					ptr = argv[optind];

				if (!ptr) {
					TEST_PRINT_ERR("Bad Pan Offset: Syntax"
							"[-pan xoffset,yoffset]\n");
					exit(-1);
				}
				var = &testoptions.var;

				var->xoffset = strtoul(ptr, &ptr, 0);
				if (!ptr) {
					TEST_PRINT_ERR("Bad Pan Offset: Syntax"
							"[-pan xoffset,yoffset]\n");
					exit(-1);
				}

				if (*ptr == ',')
					var->yoffset = strtoul(ptr+1, &ptr, 0);

				pan_display = TRUE;
			}
			break;
		case OPTION_NONSTD_PIXELFMT:
			if (optarg != NULL)
				testoptions.var.nonstd = atoi(optarg);
			else if (optind < argc && argv[optind])
				testoptions.var.nonstd = atoi(argv[optind]);
			break;
		case OPTION_OUTPUT:
			{
				char *ptr = NULL;

				if (optarg != NULL)
					ptr = optarg;
				else if (optind < argc && argv[optind])
					ptr = argv[optind];

				if (!ptr && !testoptions.pfunc.set_output) {
					TEST_PRINT_ERR("Bad Oputput device: "
					"Syntax [-output lcd/tv/dvi]\n");
					exit(-1);
				}
				if (!strcmp(ptr, "lcd"))
					testoptions.output = 0;
				else if (!strcmp(ptr, "dvi"))
					testoptions.output = 1;
				else if (!strcmp(ptr, "tv"))
					testoptions.output = 2;
				else
					testoptions.output = -1;
			}
			break;
		case OPTION_IOCTL_TEST:
			if (optarg != NULL)
				testoptions.ioctl_no = atoi(optarg);
			else if (optind < argc && argv[optind])
				testoptions.ioctl_no = atoi(argv[optind]);
			ioctl_test = TRUE;
			break;

		case OPTION_FILE_NAME:
			if (optarg != NULL)
				testoptions.file_name = optarg;
			else if (optind < argc && argv[optind])
				testoptions.file_name = argv[optind];
			ret = -EINVAL;
			break;
		case OPTION_LOOP_CNT:
			if (optarg != NULL)
				testoptions.loopcount = atoi(optarg);
			else if (optind < argc && argv[optind])
				testoptions.loopcount = atoi(argv[optind]);
			break;
		case OPTION_TESTCASE_ID:
			if (optarg != NULL)
				testoptions.testcase_id = optarg;
			else if (optind < argc && argv[optind])
				testoptions.testcase_id = argv[optind];
			break;
		case OPTION_FRAME_RATE:
			testoptions.framerate = TRUE;
			break;
		case OPTION_CPU_LOAD:
			testoptions.cpuload = TRUE;
			break;
                case OPTION_COLORKEY:
			ck_display = TRUE;
                        break;
                case OPTION_ALPHA:
                        alpha_display = TRUE;
                        break;
		case OPTION_VERSION:
		case 'v':
		case 'V':
			st_display_fbdev_testsuite_version();
			version = TRUE;
			break;
		case OPTION_HELP:
		case 'h':
		case 'H':
		default:
			st_display_fbdev_display_test_suite_help(&testoptions);
			help = TRUE;
			break;
		}
	}

	if ((version != TRUE) && (help != TRUE) && (ioctl_test != TRUE))
		st_print_fbdev_display_show_params(&testoptions);

	/* Instead of setting output at every test case we are setting only once
	   and the place is here */
	if (testoptions.pfunc.set_output && testoptions.output != -1) {
		ret = testoptions.pfunc.set_output(&testoptions);
		if (ret)
			return ret;
	}

	if (ioctl_test == TRUE)
		ret = st_fbdev_ioctl_test(&testoptions);
	else if (testoptions.file_name)
		ret = st_fbdev_display_bmp_test(&testoptions);
	else if ((pan_display == TRUE) || (testoptions.cpuload == TRUE) ||
			(testoptions.framerate == TRUE))
		ret = st_fbdev_pan_display_test(&testoptions);
	else if (ck_display == TRUE) 
		ret =  st_fbdev_colorkey_test(&testoptions);
       else if (alpha_display == TRUE)
             ret =  st_fbdev_alphablending_test(&testoptions);
	else
	ret = st_fbdev_display_test(&testoptions);
	return ret;
}


int main(int argc, char *argv[])
{
	/* Invoke the parser function to process the command line options */
	return st_process_fbdev_display_test_options(argc, argv);
}

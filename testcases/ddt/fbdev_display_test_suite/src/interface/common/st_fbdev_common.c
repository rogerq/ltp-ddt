/*
 * st_fbdev_common.c
 *
 * File contains some common function definitions
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

/* Header files*/
#include "st_fbdev_common.h"

#define MAXLOOPCOUNT            500
#define MAX_BUFFER              2
#define NUMBUFFERS              3
#define IMG_WIDTH                       400
#define IMG_HEIGHT                      272

char dev_name[1][20] = { "/dev/video1"};

struct buf_info {
        unsigned int length;
        char *start;
};

static unsigned int  video1[IMG_WIDTH][IMG_HEIGHT];
static unsigned int  graphics[IMG_WIDTH][IMG_HEIGHT];

/* RGB888 [also applicable to RGB8888] color bar pattern */
static unsigned int rgb888[2][8] = {
	{
		(0xFF << 16) | (0xFF << 8) | (0xFF),
		(0x00 << 16) | (0x00 << 8) | (0x00),
		(0xFF << 16) | (0x00 << 8) | (0x00),
		(0x00 << 16) | (0xFF << 8) | (0x00),
		(0x00 << 16) | (0x00 << 8) | (0xFF),
		(0xFF << 16) | (0xFF << 8) | (0x00),
		(0xFF << 16) | (0x00 << 8) | (0xFF),
		(0x00 << 16) | (0xFF << 8) | (0xFF),
	}, {
		(0x00 << 16) | (0xFF << 8) | (0xFF),
		(0xFF << 16) | (0x00 << 8) | (0xFF),
		(0xFF << 16) | (0xFF << 8) | (0x00),
		(0x00 << 16) | (0x00 << 8) | (0xFF),
		(0x00 << 16) | (0xFF << 8) | (0x00),
		(0xFF << 16) | (0x00 << 8) | (0x00),
		(0x00 << 16) | (0x00 << 8) | (0x00),
		(0xFF << 16) | (0xFF << 8) | (0xFF),
	},
};

static short rgb565[2][8] = {
	{
		(0x1F << 11) | (0x3F << 5) | (0x1F),
		(0x00 << 11) | (0x00 << 5) | (0x00),
		(0x1F << 11) | (0x00 << 5) | (0x00),
		(0x00 << 11) | (0x3F << 5) | (0x00),
		(0x00 << 11) | (0x00 << 5) | (0x1F),
		(0x1F << 11) | (0x3F << 5) | (0x00),
		(0x1F << 11) | (0x00 << 5) | (0x1F),
		(0x00 << 11) | (0x3F << 5) | (0x1F),
	}, {
		(0x00 << 11) | (0x3F << 5) | (0x1F),
		(0x1F << 11) | (0x00 << 5) | (0x1F),
		(0x1F << 11) | (0x3F << 5) | (0x00),
		(0x00 << 11) | (0x00 << 5) | (0x1F),
		(0x00 << 11) | (0x3F << 5) | (0x00),
		(0x1F << 11) | (0x00 << 5) | (0x00),
		(0x00 << 11) | (0x00 << 5) | (0x00),
		(0x1F << 11) | (0x3F << 5) | (0x1F),
	}
};

static short ycbcr[8] = {
		(0x1F << 11) | (0x3F << 5) | (0x1F),
		(0x00 << 11) | (0x00 << 5) | (0x00),
		(0x1F << 11) | (0x00 << 5) | (0x00),
		(0x00 << 11) | (0x3F << 5) | (0x00),
		(0x00 << 11) | (0x00 << 5) | (0x1F),
		(0x1F << 11) | (0x3F << 5) | (0x00),
		(0x1F << 11) | (0x00 << 5) | (0x1F),
		(0x00 << 11) | (0x3F << 5) | (0x1F),
	};

void fill(void *start, unsigned int pixfmt, unsigned int w, unsigned int h, int c)
{
	int i, j, k;
	int startcolor=c;

	switch(pixfmt) {
		case V4L2_PIX_FMT_RGB565:
			{
				unsigned short *ptr = (unsigned short *)start;
				unsigned short val;
				for(i = 0 ; i < h ; i ++) {
					startcolor = c;
					for (j = 0 ; j < 8 ; j ++) {
						for( k = 0 ; k < w / 8 ; k ++) {
							val = ycbcr[startcolor];
							*ptr = val;
							ptr ++;
						}
						startcolor ++;
						if(startcolor == 8)
							startcolor = 0;
					}
				}
			}
			break;
	}
}

void fill_alpha(void *start, unsigned int w, unsigned int h,
                unsigned int output_device)
{
        int i, j;
        static int count = 0;
        static int direction = 0;
        static char count1=0;

        if(output_device == 1) {
                if(count >= 250 || (count <= 0 && direction)) {
                        direction = ~direction;
                }
                if(!direction)
                        count+=25;
                else
                        count-=25;
                count1 = 255 - count;
                if(count1 < 25 )
                        count1 = 0;

        }

        unsigned int *ptr = (unsigned int *)start;
        ptr += w* (h-1) ;
        for(i = 0 ; i < h ; i ++) {
                for (j = 0 ; j < w ; j ++) {
                        if(output_device == 1) {
                                if(i < 200)
                                        *ptr  = ((graphics[i][j] & 0xFFFFFF00)
                                                        | count);
                                else
                                        *ptr = ((graphics[i][j] & 0xFFFFFF00)
                                                        | count1);
                        } else if (output_device == 0) {
                                *ptr = video1[i][j];
                        }
                        ptr ++;
                }
                ptr -= 2*w;
        }

}

/*
 * Help/Usage instructions
 *
 * Help is broken down into two layers, Standard and Custom (Platform specific).
 */
void st_display_fbdev_display_test_suite_help(struct fbdev_display_testparams
		*options)
{
	printf("FbdevDisplayTestSuite V %s\n", VERSION_STRING);
	printf("Usage:\n"
		"./fbdev_display_tests <options>\n\n"
		"-device		Device name on which display test is to be run\n"
		"\t\t\tPossible values-/dev/fb0,/dev/fb1\n"
		"-width			Width of the image to be displayed\n"
		"-height		Height of the image to be displayed\n"
		"-virtual_width		Virtual Width of the image to be displayed\n"
		"-virtual_height	Virtual Height of the image to be displayed\n"
		"-bits_per_pixel	Pixel Format\n"
		"-grayscale		Grey scale\n"
		"-left			Left Margin\n"
		"-right			Right Margin\n"
		"-lower			Lower Margin\n"
		"-upper			Upper Margin\n"
		"-hsynclen		Hsync len\n"
		"-vsynclen		Vsync len\n"
		"-rgba			Color depth and offset [< r,g,b,a>]\n"
		"-colorkey                Colorkeying testing\n"
		"-alphablend              Alpha blending testcase\n"
		"-pan			Double buffering [<xoffset,yoffset>]\n"
		"\t\t\t Syntax - rL/rO,gL/gO,bL/bO,tL/tO or rL,gL,bL,tL]\n"
		"-rotate		Rotation angle [Note: Check support for rotation]\n"
		"\t\t\tPossible values- 	0, 90, 180, 270\n"
		"-file			Name of the BMP image file to display\n"
		"\t\t\t Make sure the file is as per -w(width),-h(height)"
		" configured with valid pixel format\n"
		"-loop			Number of times to run loop for ioctl test\n"
		"-ioctl			IOCTL number\n"
		"\t\t\tPossible values-0:getVscreenInfo , 1:putVscreenInfo "
		"2:getFscreenInfo 3: blank 4:getCmap 5:putCmap \n"
		"\t\t\t Note: Also check custom ioctl support below\n"
		"-id			Test case id string for logging purpose\n"
		"-framerate      	Displays the frame rate in fps(frames per sec)\n"
		"-help			Displays the help\n"
		"-version		Displays the version of LFTB package\n"
		"-cpuload         	Displays the cpu load in percenatge\n");

	TEST_PRINT_TRC("Custom (Platform Specific) Fbdev Configuration help\n");

	/* Platform specific Help */
	if (options->pfunc.help)
		options->pfunc.help();

	exit(0);
}
/*
 * Prints Version control string
 */
void st_display_fbdev_testsuite_version()
{
	printf("FbdevDisplayTestSuite V %s\n", VERSION_STRING);
	exit(0);
}

/*
 * This function prints test status
 */
void st_fbdev_display_test_status(int status, char *test_id)
{
	/* print status of the test case */
	TEST_PRINT_TST_RESULT(status, test_id);
}


/*
 * Prints the test option values being used for current test case
 *
 * Function is broken down into two layers, Standard and Custom (Platform specific).
 */
void st_print_fbdev_display_show_params(struct fbdev_display_testparams *options)
{
	TEST_PRINT_TST_START(options->testcase_id);
	TEST_PRINT_TRC("The Test is going to start with following values");
	TEST_PRINT_TRC("The device node |%s", options->device_name);
	TEST_PRINT_TRC("Height of the image |%d", options->var.xres);
	TEST_PRINT_TRC("Width  of the image |%d", options->var.yres);
	TEST_PRINT_TRC("Virtual Height of the image |%d",
				options->var.xres_virtual);
	TEST_PRINT_TRC("Virtual Width  of the image |%d",
				options->var.yres_virtual);
	TEST_PRINT_TRC("Pixel clock |%d", options->var.pixclock);
	TEST_PRINT_TRC("Bits per Pixel of the image |%d",
				options->var.bits_per_pixel);
	TEST_PRINT_TRC("Pan X-offset |%d", options->var.xoffset);
	TEST_PRINT_TRC("Pan Y-offset |%d", options->var.yoffset);
	TEST_PRINT_TRC("Rotation Angle |%d", options->var.rotate);
	TEST_PRINT_TRC("GrayScale |%d", options->var.grayscale);
	TEST_PRINT_TRC("Left Margin |%d", options->var.left_margin);
	TEST_PRINT_TRC("Right Margin |%d", options->var.right_margin);
	TEST_PRINT_TRC("Lower Margin |%d", options->var.lower_margin);
	TEST_PRINT_TRC("Upper Margin |%d", options->var.upper_margin);
	TEST_PRINT_TRC("Hsync Length |%d", options->var.hsync_len);
	TEST_PRINT_TRC("Vsync Length |%d", options->var.vsync_len);
	TEST_PRINT_TRC("Bit Fields [RGBA]  |%d/%d, %d/%d, %d/%d, %d/%d",
                options->var.red.length, options->var.red.offset,
                options->var.green.length, options->var.green.offset,
                options->var.blue.length, options->var.blue.offset,
                options->var.transp.length, options->var.transp.offset);

	TEST_PRINT_TRC("Custom (Platform Specific) Fbdev Configuration\n");
	/* Platform specific options */
	if (options->pfunc.show_params)
		options->pfunc.show_params(options);
}

/*
 * Initilaizes the default values for various test case options
 */
int st_init_fbdev_display_test_params(struct fbdev_display_testparams *options)
{
	int ret;

	/* Open the device */
	ret = open(options->device_name, O_RDWR);
	if (ret == -1)
		return ret;

	options->fd = ret;

	/* Read default Variable Screen info */
	ret = ioctl(options->fd, FBIOGET_VSCREENINFO, &options->var);
	if (ret != SUCCESS)
		goto exit;

	/* Read default Fix Screen info */
	ret = ioctl(options->fd, FBIOGET_FSCREENINFO, &options->fix);
exit:
	/* Close the device node, since it gets opened in test case execution */
	ret |= close(options->fd);

	/* Initialize platform specific API's */
	st_fbdev_init_platform_api(&options->pfunc);

	return ret;
}

/*
 * Fill up buffer with color bars.
 */
void fill_color_bar(unsigned char *addr, unsigned int line_length,
		unsigned int height, unsigned int bits_per_pixel)
{
	int i, j, k;

	switch (bits_per_pixel) {
	case 16:
		{
			unsigned short *start = (unsigned short*) addr;

			for (i = 0; i < 8; i++) {
				for(j = 0 ; j < (height / 8) ; j++) {
					for (k = 0; k < (line_length/2); k++)
						start[k] = rgb565[0][i];
					start += (line_length/2);
				}
			}
		}
		break;
	case 24:
	case 32:
		{
			unsigned int *start = (unsigned int*)addr;

			for(i = 0 ; i < 8 ; i++) {
				for(j = 0 ; j < (height / 8) ; j++) {
					for(k = 0 ; k < (line_length/4); k++)
						start[k] = rgb888[0][i];
					start += (line_length/4);
				}
			}
		}
		break;
	default:
		TEST_PRINT_TRC("Invalid bpp");
		break;
	}
}
/*
 * Fill up buffer with vertical color bars.
 */
void fill_color_bar_vertical(unsigned char *addr, int width, int height,
		unsigned int bits_per_pixel)
{
	int i, j, k;

	switch (bits_per_pixel) {
	case 16:
		{
			unsigned short *start = (unsigned short*) addr;
			for (j = 0; j < height / 2; j++) {
				for (i = 0; i < 8; i++) {
					for (k = 0; k < width / 8; k++) {
						*start = rgb565[0][i];
						start++;
					}
				}
			}
		}
		break;
	case 24:
	case 32:
		{
			unsigned int *start = (unsigned int*) addr;
			for (j = 0; j < height / 2; j++) {
				for (i = 0; i < 8; i++) {
					for (k = 0; k < width / 8; k++) {
						*start = rgb888[0][i];
						start++;
					}
				}
			}
		}
		break;
	default:
		TEST_PRINT_TRC("Invalid bpp");
		break;

	}
}

/*
 * Create color bar pattern and swap it after every 1 sec
 */
void fill_reverse_color_bar(unsigned char *addr, unsigned int line_length,
		unsigned int height, unsigned int bits_per_pixel)
{
	int i, j, k;

	switch (bits_per_pixel) {
	case 16:
		{
			unsigned short *start = (unsigned short*) addr;

			for (i = 0; i < 8; i++) {
				for(j = 0 ; j < (height / 8) ; j++) {
					for (k = 0; k < (line_length/2); k++)
						start[k] = rgb565[1][i];
					start += (line_length/2);
				}
			}
		}
		break;
	case 24:
	case 32:
		{
			unsigned int *start = (unsigned int*) addr;

			for(i = 0 ; i < 8 ; i++) {
				for(j = 0 ; j < (height / 8) ; j++) {
					for(k = 0 ; k < (line_length/4); k++)
						start[k] = rgb888[1][i];
					start += (line_length/4);
				}
			}
		}
		break;
	default:
		TEST_PRINT_TRC("Invalid bpp");
		break;
	}
}

/*
 * Read the SYSFS interface; please pass absolute path here
 */
int read_from_sysfs(const char *file_path, char *val)
{
	int ret, fd;

	fd = open(file_path, O_RDWR);
	if (fd < 0) {
		TEST_PRINT_ERR("Error in opening %s\n", file_path);
		return -EINVAL;
	}

	ret = read(fd, val, 100);
	if (ret <= 0) {
		TEST_PRINT_ERR("Error in writing %s %d\n", file_path, ret);
		return -EINVAL;
	}
	ret = close(fd);

	return ret;
}

/*
 * Write to the SYSFS interface; please pass absolute path here
 */
int write_to_sysfs(const char *file, const char *val)
{
	int ret, fd;
	unsigned int len;

	fd = open(file, O_RDWR);
	if (fd < 0) {
		TEST_PRINT_ERR("Error in opening %s\n", file);
		return -EINVAL;
	}
	len = strlen(val);

	ret = write(fd, val, len);
	if (ret != len) {
		TEST_PRINT_ERR("Error in writing %s %s\n", file, val);
		return -EINVAL;
	}
	ret = close(fd);

	return ret;
}
/*
 * Opens FBDEV device
 */
int st_fbdev_open_interface(char *name, int *fd)
{
	int ret;

	ret = open(name, O_RDWR);
	if (ret <= 0) {
		perror("Open: ");
		return ret;
	}

	*fd = ret;

	TEST_PRINT_TRC("FBDEV %s device opened", name);

	return SUCCESS;
}

/*
 * Closes FBDEV device
 */
int st_fbdev_close_interface(char *name, int *fd)
{
	int ret;

	ret = close(*fd);
	if (0 != ret) {
		TEST_PRINT_TRC("%s: ", name);
		perror("Close: ");
		return ret;
	}

	*fd = -1;

	TEST_PRINT_TRC("FBDEV %s device closed", name);

	return SUCCESS;
}

/*
 * Get fixed screen info
 */
int st_fbdev_get_fscreeninfo_interface(int fd, struct fb_fix_screeninfo *fix)
{
	int ret = SUCCESS;

	ret = ioctl(fd, FBIOGET_FSCREENINFO, fix);
	if (SUCCESS != ret)
		perror ("FBIOGET_FSCREENINFO: ");

	return ret;
}

/*
 * Get variable screen info
 */
int st_fbdev_get_vscreeninfo_interface(int fd, struct fb_var_screeninfo *var)
{
	int ret;

	ret = ioctl(fd, FBIOGET_VSCREENINFO, var);
	if (SUCCESS != ret)
		perror ("FBIOGET_VSCREENINFO: ");

	return ret;
}

/*
 * Put/Set variable screen info
 */
int st_fbdev_put_vscreeninfo_interface(int fd, struct fb_var_screeninfo *var)
{
	int ret;

	/* call Ioctl */
	ret = ioctl(fd, FBIOPUT_VSCREENINFO, var);
	if (SUCCESS != ret)
		perror ("FBIOPUT_VSCREENINFO: ");

	return ret;
}

/*
 * Maps FBDEV buffers to user space
 */
int st_fbdev_mmap_interface(int fd, unsigned int line_length,
		unsigned int virtual_height, unsigned char** buf_addr)
{
	unsigned int buf_size;

	buf_size = line_length * virtual_height;

	/* Map the fbdev dev0 buffers to user space */
	*buf_addr = mmap(NULL, buf_size, PROT_READ | PROT_WRITE,
			MAP_SHARED, fd, 0);
	if (MAP_FAILED == *buf_addr) {
		perror("MMAP: ");
		return -1;
	}

	return 0;
}

/*
 *  Pan/Double buffer the display
 */
int st_fbdev_pandisplay_interface(int fd, struct fb_var_screeninfo *var)
{
	int ret;

	ret = ioctl(fd, FBIOPAN_DISPLAY, var);
	if (SUCCESS != ret)
		perror ("FBIOPAN_DISPLAY: ");

	return ret;
}

/*
 * Enable/Disable blanking
 */
int st_fbdev_blank_interface(int fd, Bool st_blank)
{
	int ret;

	ret = ioctl(fd, FBIOBLANK, st_blank);
	if (0 != ret)
		perror ("FBIOBLANK: ");

	return ret;
}

/*
 * Get color map table
 */
int st_fbdev_getcmap_ioctl_interface(int fd, struct fb_cmap *cmap)
{
	int ret;

	ret = ioctl(fd, FBIOGETCMAP, cmap);
	if (SUCCESS != ret)
		perror ("FBIOGETCMAP: ");

	return ret;
}

/*
 * Put/Set color map table
 */
int st_fbdev_putcmap_ioctl_interface(int fd, struct fb_cmap *cmap)
{
	int ret;

	ret = ioctl(fd, FBIOPUTCMAP, cmap);
	if (SUCCESS != ret)
		perror ("FBIOPUTCMAP");

	return ret;
}

/*
 * Displays color bar to Fbdev buffer
 */
void st_fbdev_create_color_bar(unsigned int line_length, unsigned int height,
		unsigned int bits_per_pixel, unsigned char* buf_addr)
{
	/* Fill the buffers with the color bars */
	fill_color_bar(buf_addr, line_length, height, bits_per_pixel);
}

/*
 * Unmaps buffers
 */
int st_fbdev_unmap_interface(int fd, unsigned int line_length,
		unsigned int virtual_height, unsigned char* buf_addr)
{
	int ret;
	unsigned int buf_size;

	buf_size = line_length * virtual_height;

	ret = munmap(buf_addr, buf_size);
	if (SUCCESS != ret)
		perror ("munmap: ");

	return ret;
}

/* 
* Colorkey Video Display
*/

int video_colrkey_display()
{
	int mode = O_RDWR, ret, j, i;
	int fd, a, numbuffers = 3, ch_no = 0;
	struct v4l2_requestbuffers req;
	struct v4l2_buffer buf;
	struct buf_info *buff_info;
	struct v4l2_format fmt;
	struct v4l2_framebuffer framebuffer;
	struct v4l2_capability capability;

	/* Open the video1 device */
	fd = open((const char *)dev_name[ch_no], mode);
	if (fd <= 0) {
		TEST_PRINT_ERR("Cannot open = %s device\n", dev_name[ch_no]);
		exit(0);
	}

	/* Check if the device is capable of streaming */
	if (ioctl(fd, VIDIOC_QUERYCAP, &capability) < 0) {
		perror("VIDIOC_QUERYCAP");
		goto vid_exit2;
	}

	/* set the buffer parameters */
	fmt.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	ret = ioctl(fd, VIDIOC_G_FMT, &fmt);
	if (ret < 0) {
		perror("VIDIOC_G_FMT\n");
		goto vid_exit2;
	}

	/* Set the image size pixel format and device type*/
	//fmt.fmt.pix.width = WIDTH;
	//fmt.fmt.pix.height = HEIGHT;
	fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_RGB565;
	ret = ioctl(fd, VIDIOC_S_FMT, &fmt);
	if (ret < 0) {
		perror("VIDIOC_S_FMT\n");
		goto vid_exit2;
	}

	fmt.type = V4L2_BUF_TYPE_VIDEO_OVERLAY;
	ret = ioctl(fd, VIDIOC_G_FMT, &fmt);
	if (ret < 0) {
		perror("VIDIOC_G_FMT 2\n");
		goto vid_exit2;
	}

	/* Set the window size  and chromakey*/

	fmt.fmt.win.w.left = 0;
	fmt.fmt.win.w.top = 0;
	//fmt.fmt.win.w.width = WIDTH;
	//fmt.fmt.win.w.height = HEIGHT;
	/* Make Yellow color transperent */
	/*
	 *      5     |    6      |   5
	 *  1 1 1 1 1  1 1 1 1 1 1  0 0 0 0 0
	 */
	fmt.fmt.win.chromakey = 0xFFE0;
	ret = ioctl(fd, VIDIOC_S_FMT, &fmt);
	if (ret < 0) {
		perror("VIDIOC_S_FMT 2\n");
		goto vid_exit2;
	}

	/* Get the flags related to overlay parameters like alpha blending, color keying. */
	/* Enabled color keying after setting the chroma key as chormakey set is used
	while enabling of the color keying */
	ret = ioctl (fd, VIDIOC_G_FBUF, &framebuffer);
	if (ret < 0) {
		perror ("VIDIOC_G_FBUF");
		goto vid_exit2;

	}
	/* Set SRC_COLOR_KEYING if device supports that */
	if(framebuffer.capability & V4L2_FBUF_CAP_CHROMAKEY) {

		framebuffer.flags |= V4L2_FBUF_FLAG_CHROMAKEY;
		framebuffer.flags &= ~V4L2_FBUF_FLAG_SRC_CHROMAKEY;

		ret = ioctl (fd, VIDIOC_S_FBUF, &framebuffer);
		if (ret < 0) {
			perror ("VIDIOC_S_FBUF");
			goto vid_exit2;

		}
	}

	fmt.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	ret = ioctl(fd, VIDIOC_G_FMT, &fmt);
	if (ret < 0) {
		perror("VIDIOC_G_FMT\n");
		goto vid_exit2;
	}

	/* Request buffers from driver
	 * Buffer can be driver allocated type MMAP
	 * or userptr
	 */
	req.count = numbuffers;
	req.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	req.memory = V4L2_MEMORY_MMAP;

	ret = ioctl(fd, VIDIOC_REQBUFS, &req);
	if (ret < 0) {
		perror("cannot allocate memory\n");
		goto vid_exit2;
	}

	buff_info = (struct buf_info *) malloc
		(sizeof(struct buf_info) * req.count);
	if (!buff_info) {
		TEST_PRINT_ERR("cannot allocate memory for buff_info\n");
		goto vid_exit2;
	}
	/* Query the buffer to get the physical address of the allocated
	 * buffers. Get the virtual  address of the buffer by
	 * passing the physical address to mmap call of the driver
	 */
	for (i = 0; i < req.count; i++) {
		buf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
		buf.index = i;
		ret = ioctl(fd, VIDIOC_QUERYBUF, &buf);
		if (ret < 0) {
			perror("VIDIOC_QUERYCAP\n");
			for (j = 0; j < i; j++)
				munmap(buff_info[j].start,
						buff_info[j].length);
			goto vid_exit3;
		}
		buff_info[i].length = buf.length;
		buff_info[i].start =
			mmap(NULL, buf.length, PROT_READ | PROT_WRITE,
					MAP_SHARED, fd, buf.m.offset);

		if ((unsigned int) buff_info[i].start == MAP_SHARED) {
			TEST_PRINT_ERR("Cannot mmap = %d buffer\n", i);
			for (j = 0; j < i; j++)
				munmap(buff_info[j].start,
						buff_info[j].length);
			goto vid_exit3;
		}
		memset(buff_info[i].start, 0x80, buff_info[i].length);
		/* Initially fill the buffer */
		fill(buff_info[i].start, fmt.fmt.pix.pixelformat, fmt.fmt.pix.width,
				fmt.fmt.pix.height, 0);
	}
	memset(&buf, 0, sizeof(buf));

	/* Enqueue buffers */
	for (i = 0; i < req.count; i++) {
		buf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
		buf.index = i;
		buf.memory = V4L2_MEMORY_MMAP;
		ret = ioctl(fd, VIDIOC_QBUF, &buf);
		if (ret < 0) {
			perror("VIDIOC_QBUF\n");
			goto vid_exit4;
		}
	}
	/* Start streamon */
	a = 0;
	ret = ioctl(fd, VIDIOC_STREAMON, &a);
	if (ret < 0) {
		perror("VIDIOC_STREAMON\n");
		goto vid_exit4;
	}

	/* Forever looop for streaming */
	for(i = 0 ; i < MAXLOOPCOUNT ; i ++) {
		ret = ioctl(fd, VIDIOC_DQBUF, &buf);
		if(ret < 0){
			perror("VIDIOC_DQBUF\n");
			goto vid_exit5;
		}
		ret = ioctl(fd, VIDIOC_QBUF, &buf);
		if(ret < 0){
			perror("VIDIOC_QBUF\n");
			goto vid_exit5;
		}
	}

	/* Streaming off */
	ret = ioctl(fd, VIDIOC_STREAMOFF, &a);
	if (ret < 0) {
		perror("VIDIOC_STREAMOFF\n");
		goto vid_exit4;
	}
	/* Unmap all the buffers */
	for (i = 0; i < req.count; i++)
		munmap(buff_info[i].start, buff_info[i].length);

	ret = ioctl (fd, VIDIOC_G_FBUF, &framebuffer);
	if (ret < 0) {
		perror ("VIDIOC_G_FBUF");
		goto vid_exit4;

	}
	if(framebuffer.capability & V4L2_FBUF_CAP_CHROMAKEY) {

		framebuffer.flags &= ~V4L2_FBUF_FLAG_CHROMAKEY;

		ret = ioctl (fd, VIDIOC_S_FBUF, &framebuffer);
		if (ret < 0) {
			perror ("VIDIOC_S_FBUF");
			goto vid_exit4;

		}
	}

	close(fd);
	return 0;

vid_exit5:
	ret = ioctl(fd, VIDIOC_STREAMOFF, &a);
	if (ret < 0) {
		perror("VIDIOC_STREAMOFF\n");
		goto vid_exit4;
	}
vid_exit4:
	for (j = 0; j < req.count; j++)
		munmap(buff_info[j].start, buff_info[j].length);
vid_exit3:
	free(buff_info);
vid_exit2:
	close(fd);

return 0;
}

int video_alphablending_display( )
{
#if 1 
	int mode = O_RDWR, ret, j, i;
	int fd, a, numbuffers = 3, ch_no = 0;
	struct v4l2_requestbuffers req;
	struct v4l2_buffer buf;
	struct buf_info *buff_info;
	struct v4l2_format fmt;
	struct v4l2_crop crop;
	struct v4l2_framebuffer framebuffer;

	/* Open the video1 device */
	fd = open((const char *)dev_name[ch_no], mode);
	if (fd <= 0) {
		TEST_PRINT_ERR("Cannot open = %s device\n", dev_name[ch_no]);
		exit(0);
	}

	ret = ioctl (fd, VIDIOC_G_FBUF, &framebuffer);
	if (ret < 0) {
		perror ("VIDIOC_G_FBUF");
		exit(1);

	}
	if(framebuffer.capability & V4L2_FBUF_CAP_LOCAL_ALPHA) {

		framebuffer.flags |= V4L2_FBUF_FLAG_LOCAL_ALPHA;
		framebuffer.flags &= ~(V4L2_FBUF_FLAG_CHROMAKEY |
				V4L2_FBUF_FLAG_SRC_CHROMAKEY);

		ret = ioctl (fd, VIDIOC_S_FBUF, &framebuffer);
		if (ret < 0) {
			perror ("VIDIOC_S_FBUF");
			exit(1);

		}
	}

	fmt.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	ret = ioctl(fd, VIDIOC_G_FMT, &fmt);
	if (ret < 0) {
		perror("VIDIOC_G_FMT\n");
		close(fd);
		exit(0);
	}
	/* Set the image size pixel format and device type*/
	fmt.fmt.pix.width = IMG_WIDTH;
	fmt.fmt.pix.height = IMG_HEIGHT;

	/* Video1 does not support ARGB so it takes
	 * RGB32 as unpack RGB24
	 */
	fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_RGB32;
	ret = ioctl(fd, VIDIOC_S_FMT, &fmt);
	if (ret < 0) {
		perror("VIDIOC_S_FMT\n");
		close(fd);
		exit(0);
	}

	crop.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	ret = ioctl(fd, VIDIOC_G_CROP, &crop);
	if (ret < 0) {
		perror("VIDIOC_G_CROP\n");
		close(fd);
		exit(0);
	}
	/* Set the Cropping parameters */
	crop.c.left = 0;
	crop.c.top = 0;
	crop.c.width = IMG_WIDTH;
	crop.c.height = IMG_HEIGHT;
	ret = ioctl(fd, VIDIOC_S_CROP, &crop);
	if (ret < 0) {
		perror("VIDIOC_S_CROP\n");
		close(fd);
		exit(0);
	}

	fmt.type = V4L2_BUF_TYPE_VIDEO_OVERLAY;
	ret = ioctl(fd, VIDIOC_G_FMT, &fmt);
	if (ret < 0) {
		perror("VIDIOC_G_FMT 2\n");
		close(fd);
		exit(0);
	}
	/* Set the window size */
	fmt.fmt.win.w.left = 0;
	fmt.fmt.win.w.top = 0;
	fmt.fmt.win.w.width = IMG_WIDTH;
	fmt.fmt.win.w.height = IMG_HEIGHT;
	ret = ioctl(fd, VIDIOC_S_FMT, &fmt);
	if (ret < 0) {
		perror("VIDIOC_S_FMT 2\n");
		close(fd);
		exit(0);
	}

	fmt.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	ret = ioctl(fd, VIDIOC_G_FMT, &fmt);
	if (ret < 0) {
		perror("VIDIOC_G_FMT\n");
		close(fd);
		exit(0);
	}

	/* Request buffers from driver
	 * Buffer can be driver allocated type MMAP
	 * or userptr
	 */
	req.count = numbuffers;
	req.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	req.memory = V4L2_MEMORY_MMAP;

	ret = ioctl(fd, VIDIOC_REQBUFS, &req);
	if (ret < 0) {
		perror("cannot allocate memory\n");
		close(fd);
		exit(0);
	}

	buff_info =
		(struct buf_info *) malloc(sizeof(struct buf_info) *
					   req.count);
	if (!buff_info) {
		TEST_PRINT_ERR("cannot allocate memory for buff_info\n");
		close(fd);
		exit(0);
	}
	/* Query the buffer to get the physical address of the allocated
	 * buffers. Get the virtual  address of the buffer by
	 * passing the physical address to mmap call of the driver
	 */
	for (i = 0; i < req.count; i++) {
		buf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
		buf.index = i;
		ret = ioctl(fd, VIDIOC_QUERYBUF, &buf);
		if (ret < 0) {
			perror("VIDIOC_QUERYCAP\n");
			for (j = 0; j < i; j++)
				munmap(buff_info[j].start,
						buff_info[j].length);
			close(fd);
			exit(0);
		}
		buff_info[i].length = buf.length;
		buff_info[i].start =
			mmap(NULL, buf.length, PROT_READ | PROT_WRITE,
					MAP_SHARED, fd, buf.m.offset);

		if ((unsigned int) buff_info[i].start ==
				MAP_SHARED) {
			TEST_PRINT_ERR("Cannot mmap = %d buffer\n", i);
			for (j = 0; j < i; j++)
				munmap(buff_info[j].start,
						buff_info[j].length);
			close(fd);
			exit(0);
		}
		memset(buff_info[i].start, 0x80, buff_info[i].length);
		/* Initially fill the buffer */
		fill_alpha(buff_info[i].start, fmt.fmt.pix.width,
				fmt.fmt.pix.height, 0);
	}
	memset(&buf, 0, sizeof(buf));

	/* Enqueue buffers */
	for (i = 0; i < req.count; i++) {
		buf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
		buf.index = i;
		buf.memory = V4L2_MEMORY_MMAP;
		ret = ioctl(fd, VIDIOC_QBUF, &buf);
		if (ret < 0) {
			perror("VIDIOC_QBUF\n");
			for (j = 0; j < req.count; j++)
				munmap(buff_info[j].start,
						buff_info[j].length);
			exit(0);
		}
	}
	/* Start streamon */
	a = 0;
	ret = ioctl(fd, VIDIOC_STREAMON, &a);
	if (ret < 0) {
		perror("VIDIOC_STREAMON\n");
		for (i = 0; i < req.count; i++)
			munmap(buff_info[i].start, buff_info[i].length);
		exit(0);
	}

	/* Forever looop for streaming */
	for(i = 0 ; i < MAXLOOPCOUNT ; i ++) {
		ret = ioctl(fd, VIDIOC_DQBUF, &buf);
		if(ret < 0){
			perror("VIDIOC_DQBUF\n");
			for (j = 0; j < req.count; j++)
				munmap(buff_info[j].start, buff_info[j].length);
			close(fd);
			exit(0);
		}
		/* Filling the buffer without alpha value since video1 does not
		 * support ARGB
		 */
		fill_alpha(buff_info[buf.index].start, fmt.fmt.pix.width,
				fmt.fmt.pix.height, 0);
		ret = ioctl(fd, VIDIOC_QBUF, &buf);
		if(ret < 0){
			perror("VIDIOC_QBUF\n");
			for (j = 0; j < req.count; j++)
				munmap(buff_info[j].start, buff_info[j].length);
			close(fd);
			exit(0);
		}
		sleep(1);
	}

	/* Streaming off */
	ret = ioctl(fd, VIDIOC_STREAMOFF, &a);
	if (ret < 0) {
		perror("VIDIOC_STREAMOFF\n");
		for (i = 0; i < req.count; i++)
			munmap(buff_info[i].start, buff_info[i].length);
		close(fd);
		exit(0);
	}
	/* Unmap all the buffers */
	for (i = 0; i < req.count; i++)
		munmap(buff_info[i].start, buff_info[i].length);

	ret = ioctl (fd, VIDIOC_G_FBUF, &framebuffer);
	if (ret < 0) {
		perror ("VIDIOC_G_FBUF");
		exit(1);

	}
	if(framebuffer.capability & V4L2_FBUF_CAP_LOCAL_ALPHA) {

		framebuffer.flags &= ~V4L2_FBUF_FLAG_LOCAL_ALPHA;

		ret = ioctl (fd, VIDIOC_S_FBUF, &framebuffer);
		if (ret < 0) {
			perror ("VIDIOC_S_FBUF");
			exit(1);

		}
	}

	close(fd);
#endif
	return 0;
}

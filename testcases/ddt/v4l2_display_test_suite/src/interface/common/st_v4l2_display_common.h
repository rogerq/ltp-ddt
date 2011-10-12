/*
 * st_v4l2_display_common.h
 *
 * This file contains the common functions declaration
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
 *    notice, this list of conditions and the following disclaimer iyoun the
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


#ifndef _ST_COMMON_H_
#define _ST_COMMON_H_

/* Standard Linux header files */
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
#include <asm/types.h>
#include <time.h>
#include <pthread.h>
#include <signal.h>
#include <sched.h>
#include <linux/unistd.h>
#include <sys/prctl.h>
#include <sys/stat.h>

/* V4L2 specific header file */
#include <linux/videodev2.h>

/* Framebuffer module header file */
#include <linux/fb.h>

/* LFTB header files */
#include <st_defines.h>
#include <st_log.h>
#include <st_timer.h>
#include <st_cpu_load.h>

/* Macros- Default settings/values for display */
#define DEFAULT_NODE        VID1_NODE
#define DEFAULT_NODE_FB	    FB0_NODE
#define DEFAULT_NO_BUFFERS  3
#define DEFAULT_LOOP	    100
#define VID1_NODE           "/dev/video1"
#define FB0_NODE	    "/dev/fb0"
#define DEFAULT_CTRL_VALUE  0
#define MAX_BUFFERS         10
#define DEFAULT_CROP_WIDTH  0
#define DEFAULT_CROP_HEIGHT 0
#define DEFAULT_FMT_WIDTH   0
#define DEFAULT_FMT_HEIGHT  0
#define DEFAULT_WIDTH       0
#define DEFAULT_HEIGHT      0

/*Testcode related Header files*/
#include <st_defines.h>
#include <st_log.h>
#include <st_timer.h>

enum pixfmt{
	RGB565 = 1,
	YUYV,
	UVVY,
	RGB24,
	RGB32,
	NV16
} pixfmt;

enum ioctls{
	CROP_CAP,
	QUERY_CAP,
	TRY_FMT,
	G_FMT,
	S_FMT,
	G_CTRL,
	S_CTRL,
	G_CROP,
	S_CROP,
	SCALE,
	REQBUF,
	QUERYBUF,
	ENQBUF,
	STREAM_ON_OFF
} ioctls;

struct v4l2_display_testparams testoptions;

/*Structure for holding the test options */
struct v4l2_display_testparams {
	/* Device node name */
	char *device_name;
	/* Frame buffer device */
	char *device_name_fb;
	/* Width of the input image to be displayed */
	int pixelformat;
	/* Pixel format to display */
	int width;
	/* Height of the input image to be displayed */
	int height;
	/* Top of image */
	int top;
	/* Left of image */
	int left;
	/* Rotation angle */
	int rotation;
	/* Buffer nos */
	int noofbuffers;
	/* Loop nos. */
	int loop;
	/* IOCTL Number */
	int ioctl_no;
	/* Windowing width */
	int fmt_width;
	/* Windowing Height */
	int fmt_height;
	/* Windowing left */
	int fmt_left;
	/* Windowing top */
	int fmt_top;
	/* CPU load */
	int cpuload;
	/* Frame rate */
	int framerate;
	/* Userpointer Flag */
	int userptr;
	/* Device node */
	int st_dev;
	/* Framebuffer device */
	int st_dev_fb;
	/* Testcase ID */
	char *testcase_id;
	/* Crop width */
	int crop_width;
	/* Crop Height */
	int crop_height;
	/* Crop left */
	int crop_left;
	/* Crop top */
	int crop_top;
	/* Control Value */
	int value;
	/* Buffer type */
	int buffer_type;
	/* Display Output */
	int output;
	/* Structures */
	struct v4l2_format st_fmt;
	struct v4l2_control st_control;
	struct v4l2_crop st_crop;
	struct v4l2_requestbuffers st_reqbuf;
	struct v4l2_buffer st_buf;
	struct v4l2_cropcap st_cropcap;
	struct v4l2_capability st_capability;
	struct v4l2_fmtdesc st_fmtdesc;
	struct fb_fix_screeninfo st_fix;
	struct fb_var_screeninfo st_var;
};

struct buf_info {
	int index;
	unsigned int length;
	unsigned char *start;
};

/* Enable color bar display */
#define COLOR_BAR_DISPLAY

/* max command length */
#define TEST_ID_LENGTH      100

/* function prototype */
/*Parser file */
void display_v4l2_display_testsuite_version(void);
/* Testcase File */
int st_v4l2_display_test(struct v4l2_display_testparams *testoptions);
int st_v4l2_display_ioctl_test(struct v4l2_display_testparams *testoptions);
/* Comman interface file */
void colorbar_generate(unsigned char *addr, int width,
	 int height, int order, int pixelfmt);
void fill_lines(int width);
void color_bar(unsigned char *addr, int pitch, int h, int size, int order);
int st_v4l2_display_open(char *name, int *dev);
int st_fbdev_display_open(char *name, int *dev);
int st_v4l2_display_close(int dev);
int st_fbdev_display_close(int dev);
int st_v4l2_display_set_format(int dev, struct v4l2_format *st_fmt);
int st_v4l2_display_get_format(int dev, struct v4l2_format *st_fmt);
int st_v4l2_display_request_buffer_mmap(int dev,
		 struct v4l2_requestbuffers *st_reqbuf);
int st_v4l2_display_buffer_userptr(int dev_fb, int buffersize);
int st_v4l2_display_query_buffer_mmap(int dev, int count,
		 struct v4l2_format *st_fmt,
			 struct v4l2_buffer *st_buf, int pixelfmt);
int st_v4l2_display_queue_buffers_mmap(int dev, int count,
		 struct v4l2_buffer *st_buf);
int st_v4l2_display_queue_buffers_userptr(int dev, int count,
		 struct v4l2_buffer *st_buf);
int st_v4l2_display_color_bar(int dev, int noframes,
		 struct v4l2_format *st_fmt,
			 struct v4l2_buffer *st_buf, int pixelfmt);
int st_v4l2_display_color_bar_userptr(int dev, int noframes,
		 struct v4l2_format *st_fmt,
			 struct v4l2_buffer *st_buf,
				 int buffer_size, int pixelfmt);
int st_v4l2_display_streamon(int dev);
int st_v4l2_display_streamoff(int dev);
int st_v4l2_display_unmap_mmap(int count);
int st_v4l2_display_unmap_userptr(int buffer_size, int count);
int st_v4l2_set_control(int dev, struct v4l2_control *st_control);
int st_v4l2_get_control(int dev, struct v4l2_control *st_control);
int st_v4l2_display_set_crop(int dev, struct v4l2_crop *st_crop);
int st_v4l2_display_get_crop(int dev, struct v4l2_crop *st_crop);
int st_v4l2_display_set_scaling(int dev, struct v4l2_format *st_fmt,
		struct v4l2_crop *st_crop);
void init_v4l2_display_test_params
		(struct v4l2_display_testparams *testoptions);
void display_v4l2_display_test_suite_help(void);
int st_v4l2_display_querycap(int dev, struct v4l2_capability *capability);
int st_v4l2_display_query_crop(int dev, struct v4l2_cropcap *cropcap);
int st_v4l2_display_enum_fmt(int dev, struct v4l2_fmtdesc *fmtdesc);
int chk_pixel_format(int pixfmt);
int st_v4l2_display_try_format(int dev, struct v4l2_format *st_fmt);
int st_fbdev_get_vscreeninfo_interface
		(int fd, struct fb_var_screeninfo *st_var);
int st_fbdev_get_fscreeninfo_interface
		(int fd, struct fb_fix_screeninfo *st_fix);
int st_set_output_interface(struct v4l2_display_testparams
		*options);
int write_to_sysfs(const char *file, const char *val);

#endif /* _ST_COMMON_H_ */

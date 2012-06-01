/*
 * st_fbdev_common.h
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

#ifndef _ST_FBDEV_COMMON_H_
#define _ST_FBDEV_COMMON_H_

/* Standard header files */
#include <pthread.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <getopt.h>
#include <ctype.h>

#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/time.h>

#include <linux/version.h>
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,38)
#include <linux/videodev.h>
#endif
#include <linux/videodev2.h>
#include <linux/fb.h>

/* LFTB header files */
#include <st_defines.h>
#include <st_log.h>
#include <st_timer.h>
#include <st_cpu_load.h>

//unsigned char *dev_display[2];

struct fbdev_display_testparams;




/* Function pointer table for platform specific API's */
struct fbdev_func_table {
	int (*wait_for_vsync)(int fd);
	int (*set_output)(struct fbdev_display_testparams *options);
	void (*show_params)(struct fbdev_display_testparams *options);
	void (*help)(void);
};

/* Structure for holding the test options */
struct fbdev_display_testparams {
	/* Device node name */
	char *device_name;
	/* File Descriptor */
	int fd;
	/* Platform specific functions */
	struct fbdev_func_table pfunc;
	/* Variable screen info */
	struct fb_var_screeninfo var;
	/* Fix Screen Info */
	struct fb_fix_screeninfo fix;
	/* Color map table */
	struct fb_cmap cmap;
	/* Mirroring angle */
	int mirror;
	/* Variable to enable frame rate info*/
	Bool framerate;
	/* Variable to enable cpu load info*/
	Bool cpuload;
	/*  Ioctl test: please refer to help */
	int ioctl_no;
	/* Directly associated with no of frames/iteration for test case */
	int loopcount;
	/* Test Case ID string */
	char *testcase_id;
	/* Raw File name to read the image data from */
	char *file_name;
	/* Current Output */
	int output;
};


/* Help/Usage instructions */
void st_display_fbdev_display_test_suite_help(struct fbdev_display_testparams
		*options);

/* Prints Version control string */
void st_display_fbdev_testsuite_version(void);

/* print Result status */
void st_fbdev_display_test_status(int status, char *test_id);

/* Prints the test option values being used for current test case */
void st_print_fbdev_display_show_params(
        struct fbdev_display_testparams *options);

/* Initilaizes the default values for various test case options */
int st_init_fbdev_display_test_params(struct fbdev_display_testparams *options);

/* Fill up buffer with color bars. */
void fill_color_bar(unsigned char *addr, unsigned int width,
		unsigned int height, unsigned int bits_per_pixel);

/* Fill up buffer with vertical color bars. */
void fill_color_bar_vertical(unsigned char *addr, int width, int height,
		unsigned int bits_per_pixel);

/* Helper function to read/write from SYSFS */
int read_from_sysfs(const char *file, char *val);
int write_to_sysfs(const char *file, const char *val);

/* Fill color bar in reverse order */
void fill_reverse_color_bar(unsigned char *addr, unsigned int width,
		unsigned int height, unsigned int bits_per_pixel);

/* Opens FBDEV device */
int st_fbdev_open_interface(char *name, int *fd);

/* Closes FBDEV device */
int st_fbdev_close_interface(char *name, int *fd);

/* Get fixed screen info */
int st_fbdev_get_fscreeninfo_interface(int fd, struct fb_fix_screeninfo *fix);

/* Get variable screen info */
int st_fbdev_get_vscreeninfo_interface(int fd, struct fb_var_screeninfo *var);

/* Put/Set variable screen info */
int st_fbdev_put_vscreeninfo_interface(int fd, struct fb_var_screeninfo *var);

/* Maps FBDEV buffers to user space */
int st_fbdev_mmap_interface(int fd, unsigned int line_length,
            unsigned int virtual_height, unsigned char** buf_addr);

/* Pan/Double buffer the display */
int st_fbdev_pandisplay_interface(int fd, struct fb_var_screeninfo *var);

/* Enable/Disable blanking */
int st_fbdev_blank_interface(int fd, Bool st_blank);

/* Get color map table */
int st_fbdev_getcmap_ioctl_interface(int fd, struct fb_cmap *cmap);

/* Put/Set color map table */
int st_fbdev_putcmap_ioctl_interface(int fd, struct fb_cmap *cmap);

/* Displays color bar to Fbdev buffer */
void st_fbdev_create_color_bar(unsigned int line_length, unsigned int height,
		unsigned int bits_per_pixel, unsigned char* buf_addr);

/* Unmaps buffers */
int st_fbdev_unmap_interface(int fd, unsigned int line_length,
            unsigned int virtual_height, unsigned char* buf_addr);

/*
 * Common funtions: Individual test cases
 */
/* Default display routine: uses only standard Fbdev interfaces */
int st_fbdev_display_test(struct fbdev_display_testparams *params);

/* Panning: Also used for FPS and Cpuload calculation */
int st_fbdev_pan_display_test(struct fbdev_display_testparams *params);

/* IOCTL test */
int st_fbdev_ioctl_test(struct fbdev_display_testparams *params);

/*
 * Platform specific/ Non-Standard API's but commonly used
 */
void st_fbdev_init_platform_api(struct fbdev_func_table *func);

/*
 * Read the image from BMP file
 */
int st_fbdev_display_bmp_test(struct fbdev_display_testparams *params);

/* 
* Color keying testcase
*/
int st_fbdev_colorkey_test(struct fbdev_display_testparams *params);

int video_colrkey_display(void);

int video_alphablending_display(void);

void fill_alpha(void *start, unsigned int w, unsigned int h,
			unsigned int output_device);
int st_fbdev_alphablending_test
		(struct fbdev_display_testparams *params);
#endif                          /* _ST_FBDEV_COMMON_H_ */

/*
 * st_fbdev_pan_display_tests.c
 *
 * This file calls FBDEV driver functions to display color bar on the
 * selected output
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

/*
 * Header files
 */
#include <pthread.h>
#include "../interface/common/st_fbdev_common.h"

#define MAXLOOPCOUNT            30
#define MAX_BUFFER              2
#define IMG_WIDTH                       400
#define IMG_HEIGHT                      272
#define NUMBUFFERS              3
#define OMAP_TI_BMP             "omap_ti.rgb"
#define OMAP_TI_BLANK_BMP       "omap_ti_blank.rgb"

static unsigned int  video1[IMG_WIDTH][IMG_HEIGHT];
static unsigned int  graphics[IMG_WIDTH][IMG_HEIGHT];

struct buf_info {
        unsigned int length;
        char *start;
};

Int32 CK_STATUS_T3;
Int32 CK_STATUS_T4;

static int calc_result_time(struct timeval *result, struct timeval *after,
		                                     struct timeval *before)
{
	/* Perform the carry for the later subtraction by updating "before" */
	if (after->tv_usec < before->tv_usec) {
		int nsec = (before->tv_usec - after->tv_usec) / 1000000 + 1;
		before->tv_usec -= 1000000 * nsec;
		before->tv_sec += nsec;
	}
	if (after->tv_usec - before->tv_usec > 1000000) {
		int nsec = (after->tv_usec - before->tv_usec) / 1000000;

		before->tv_usec += 1000000 * nsec;
		before->tv_sec -= nsec;
	}
	/* Compute the time remaining to wait, tv_usec is certainly positive.
	 * */
	result->tv_sec = after->tv_sec - before->tv_sec;
	result->tv_usec = after->tv_usec - before->tv_usec;
	/* Return 1 if result is negative. */
	return after->tv_sec < before->tv_sec;
}

/* Thread for graphics device 0 */

void *fbdev_display_alpha(void *ptr) {
	int i, ret, buffersize, status = SUCCESS;
	unsigned char* buf_addr[MAX_BUFFER];
	unsigned int cpu_load, index;
	struct proc_stat cpuload;
	struct timeval before, after, result;
	struct fbdev_display_testparams * params = (struct fbdev_display_testparams *) ptr;

	/* Open the device */
	ret = st_fbdev_open_interface(params->device_name, &params->fd);
	if (ret != SUCCESS)
		goto exit;

	/* Enable window */
	ret = st_fbdev_blank_interface(params->fd, VESA_NO_BLANKING);
	if (ret) {
		status = FAILURE;
		goto exit;
	}
	TEST_PRINT_TRC("FBIO_BLANK Ioctl passed");

	/* If we have buffer size more than w*h*bpp then configure virtual
	 * resolution h*2*/
	if (params->var.yres == params->var.yres_virtual) {
		if (params->fix.smem_len > (params->var.xres *
					params->fix.line_length))
			params->var.yres_virtual = params->var.yres * 2;
	}
	/* TODO: Same should be implemented for xoffest panning */

	/* put variable screen info */
	ret = st_fbdev_put_vscreeninfo_interface(params->fd, &params->var);
	if (ret) {
		status = FAILURE;
		goto exit;
	}
	TEST_PRINT_TRC("FBIOPUT_VSCREENINFO Ioctl passed");

	/* get fix screen info */
	ret = st_fbdev_get_fscreeninfo_interface(params->fd, &params->fix);
	if (ret) {
		status = FAILURE;
		goto exit;
	}
	TEST_PRINT_TRC("FBIOGET_FSCREENINFO Ioctl Passed");

	/* map vid0 buffers to user space */
	ret = st_fbdev_mmap_interface(params->fd, params->fix.line_length,
			params->var.yres_virtual, &buf_addr[0]);
	if (ret) {
		status = FAILURE;
		goto exit;
	}
	TEST_PRINT_TRC("mmap Ioctl passed");

	buffersize = params->fix.line_length * params->var.yres;
	
	/* Store each buffer addresses */
	for(i = 1 ; i < MAX_BUFFER ; i ++) {
		buf_addr[i] = buf_addr[i-1] + buffersize;
	}

	/* Fill the buffers with image*/
	fill_alpha(buf_addr[0], params->fix.line_length/4,
			params->var.yres,1);
	fill_alpha(buf_addr[1], params->fix.line_length/4,
			params->var.yres,1);
			
	/* If CPU load requested */
	if (params->cpuload == TRUE)
		start_cpuload_measurement(&cpuload);

	if (params->framerate == TRUE)
		gettimeofday(&before, NULL);

	/* Create image pattern */
	for (i = 0; i < params->loopcount; i++) {
		struct fb_var_screeninfo var;
		/* get variable screen info */
		ret = st_fbdev_get_vscreeninfo_interface(params->fd, &var);
		if (ret) {
			status = FAILURE;
			goto mmap_exit;
		}
		var.xoffset += params->var.xoffset;
		if (var.xoffset > var.xres)
			var.xoffset = 0;
		var.yoffset += params->var.yoffset;
		if (var.yoffset > var.yres)
			var.yoffset = 0;

		/* Pan the display */
		ret = st_fbdev_pandisplay_interface(params->fd, &var);
		if (ret) {
			status = FAILURE;
			goto mmap_exit;
		}
		
	index = (~(i % 2)) & 0x1;
	
	fill_alpha(buf_addr[index], params->fix.line_length/4,
			params->var.yres,1);		
		
		/* Wait for Vsync */
		if (params->pfunc.wait_for_vsync)
		ret = params->pfunc.wait_for_vsync(params->fd);
		if (ret) {
			status = FAILURE;
			goto mmap_exit;
		}
	}

	if (params->framerate == TRUE) {
		gettimeofday(&after, NULL);

		calc_result_time(&result, &after, &before);
		TEST_PRINT_TRC("The Fbdev Display : Frame rate = %lu",
					params->loopcount/result.tv_sec);
	}
	/* If CPU load requested */
	if(params->cpuload == TRUE) {
		cpu_load = stop_cpuload_measurement(&cpuload);
		TEST_PRINT_TRC("The Fbdev Display : Percentage CPU Load = %u%%",
					cpu_load);
	}

mmap_exit:
	/* unmap buffers */
	ret = st_fbdev_unmap_interface(params->fd, params->fix.line_length,
			params->var.yres_virtual, buf_addr[0]);
	if (ret)
		status = FAILURE;
exit:
	/* close FBDEV device */
	ret = st_fbdev_close_interface(params->device_name, &params->fd);
	if (ret)
		status = FAILURE;

	/* print status of the test case */
	st_fbdev_display_test_status(status, params->testcase_id);

	/* end test case */
	TEST_PRINT_TST_END(params->testcase_id);
	
	CK_STATUS_T3 = status; 
	return NULL;
}

/* Thread for graphics device 0 */
void *video_display_alpha(void *ptr) {

int status;
status = video_alphablending_display();

CK_STATUS_T4 = status; 

return NULL;
}

int st_fbdev_alphablending_test(struct fbdev_display_testparams *params)
{
	int ret1, ret2, status;
	pthread_t t1, t2;
	int i,j;
	FILE * file_r;
	char red,green,blue;
	unsigned int val;

	/* Open the file for the video1 and store the
	 * RGB value
	 */
	file_r = fopen(OMAP_TI_BMP,"r");
	if(file_r == NULL) {
		printf("Unable to open video1 file\n");
		return -1;
	}
	for(i = 0 ; i < IMG_HEIGHT ; i++) {
		for(j = 0 ; j < IMG_WIDTH ; j++) {
			blue = fgetc (file_r);
			green = fgetc (file_r);
			red = fgetc (file_r);
			val = (0 << 24) | (red << 16) | (green << 8) |
				(blue << 0);
			video1[i][j] = val;
		}

	}
	fclose(file_r);

	/* Open the file for graphics and store the RGB
	 * Value
	 */
	file_r = fopen(OMAP_TI_BLANK_BMP, "r");
	if(file_r == NULL){
		printf("Unable to open graphics file\n");
		return -1;
	}
	for(i = 0 ; i < IMG_HEIGHT ; i++) {
		for(j = 0 ; j < IMG_WIDTH ; j++) {
			blue = fgetc (file_r);
			green = fgetc (file_r);
			red = fgetc (file_r);
			val = (red << 24) |(green << 16) | (blue << 8) |
				(0 << 0);
			graphics[i][j] = val;
		}
	}
	fclose(file_r);

	/* Create threads for Video1 and Video2 streaming*/
	ret1 = pthread_create(&t1, NULL, fbdev_display_alpha, (void *)params); 			//Thread one for displaying horizontal color bar
	ret2 = pthread_create(&t2, NULL, video_display_alpha, (void *)params);			//Thread two for displaying verical color bar with keying enable
	pthread_join(t1, NULL);
	pthread_join(t2, NULL);

	if (SUCCESS == CK_STATUS_T3) {
		if (SUCCESS == CK_STATUS_T4) {
			status = SUCCESS;
		}
	} 
	
 	return status;
}

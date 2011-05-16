/*
 * st_fbdev_colorkey_tests.c
 *
 * This file calls FBDEV driver functions to display color bar on the interface with color keying function,
 * it makes Yellow color transperent from top pipeline and displays vertical color bar of below pipeline. 
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
/* Testcode header files*/
#include <st_fbdev_common.h>
extern Int32 st_img_type;
extern int fd_dev;

#define	ENABLE_COLOR_KEY	1
#define	COLOR_KEY			65504
#define	KEY_CHANNEL_OUT		0	
#define	LOOP_COUNT_1		1000		  
#define LOOP_COUNT_2            1000

Int32 CK_STATUS_T1;
Int32 CK_STATUS_T2;

/* Thread for graphics device 0 */

void *fbdev_display(void *ptr) {

	int i, ret, status = SUCCESS;
	unsigned char* buf_addr;
	struct fbdev_display_testparams * params = (struct fbdev_display_testparams *) ptr;


	/* Open the device */
	ret = st_fbdev_open_interface(params->device_name, &params->fd);
	if (ret != SUCCESS) {
		status = ret;
		return NULL;
	}

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
			params->var.yres_virtual, &buf_addr);
	if (ret) {
		status = FAILURE;
		goto exit;
	}
	TEST_PRINT_TRC("mmap Ioctl passed");

	fill_color_bar(buf_addr, params->fix.line_length,
			params->var.yres, params->var.bits_per_pixel);

	/* Create color bar pattern */
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
		/* Wait for Vsync */
		if (params->pfunc.wait_for_vsync)
			ret = params->pfunc.wait_for_vsync(params->fd);
		if (ret) {
			status = FAILURE;
			goto mmap_exit;
		}
	}

mmap_exit:
	/* unmap buffers */
	ret = st_fbdev_unmap_interface(params->fd, params->fix.line_length,
			params->var.yres_virtual, buf_addr);
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

	CK_STATUS_T1 = status; 

	return NULL;
}


/* Thread for graphics device 0 */
void *video_display(void *ptr) {

int status;
status = video_colrkey_display();

CK_STATUS_T2 = status; 

return NULL;
}

int st_fbdev_colorkey_test(struct fbdev_display_testparams *params)
{
	int ret1, ret2;
	Int32 status = FAILURE;
	pthread_t t1, t2;

      TEST_PRINT_TRC("Test to display horizontal color bar on FBDEV0 pipeline and vertical color bar on VIDEO0 pipeline");
      TEST_PRINT_TRC("Colorkeying enable makes Yellow bar from FBDEV0 transperent and you can see vertical color bar from FBDEV1 there");
 
	ret1 = pthread_create(&t1, NULL, fbdev_display, (void *)params);
	ret2 = pthread_create(&t2, NULL, video_display, (void *)params);
	pthread_join(t1, NULL);
	pthread_join(t2, NULL);

	if (SUCCESS == CK_STATUS_T1) {
		if (SUCCESS == CK_STATUS_T2) {
			status = SUCCESS;
		}
	} 
	
 	return status;
}

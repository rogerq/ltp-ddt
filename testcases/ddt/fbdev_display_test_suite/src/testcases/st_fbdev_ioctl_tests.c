/*
 * st_fbdev_ioctl_tests.c
 *
 * This file calls FBDEV driver functions to test all the ioctls
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
/* header files*/

#include <st_fbdev_common.h>
#include <st_fbdev_options.h>

/*
 * Individual IOCTL Test: blank on/off the
 */
int st_fbdev_fbio_blank_ioctl_test(struct fbdev_display_testparams *params)
{
	int ret;

	TEST_PRINT_TST_START("FBIOBLANK Ioctl Test|%s", params->testcase_id);

	/* Open the device */
	ret = st_fbdev_open_interface(params->device_name, &params->fd);
	if (ret != SUCCESS)
		return ret;

	/* disable window */
	ret = st_fbdev_blank_interface(params->fd, FB_BLANK_POWERDOWN);
	if (ret)
		goto exit;

	TEST_PRINT_TRC("FBIOBLANK ioctl succeeded to power down");
	sleep(2);

	/* enable device */
	ret = st_fbdev_blank_interface(params->fd, FB_BLANK_UNBLANK);
	if (ret)
		goto exit;

	TEST_PRINT_TRC("FBIOBLANK ioctl succeeded to power up");
	sleep(2);

exit:
	/* close FBDEV device */
	ret |= st_fbdev_close_interface(params->device_name, &params->fd);

	/* print status of the test case */
	st_fbdev_display_test_status(ret, params->testcase_id);

	/* end test case */
	TEST_PRINT_TST_END(params->testcase_id);

	return ret;
}



/*
 * Individual IOCTL Test: get variable screen info
 */
int st_fbdev_get_vscreeninfo_ioctl_test(struct fbdev_display_testparams *params)
{
	int ret;
	struct fb_var_screeninfo *var = &params->var;

	TEST_PRINT_TST_START("FBIOGET_VSCREENINFO Ioctl Test|%s",
			params->testcase_id);

	/* Open the device */
	ret = st_fbdev_open_interface(params->device_name, &params->fd);
	if (ret != SUCCESS)
		return ret;

	/* get variable screen info */
	ret = st_fbdev_get_vscreeninfo_interface(params->fd, var);
	if (ret)
		goto exit;

	TEST_PRINT_TRC("X-res |%u", var->xres);
	TEST_PRINT_TRC("Y-res |%u", var->yres);
	TEST_PRINT_TRC("X-res virtual |%u", var->xres_virtual);
	TEST_PRINT_TRC("Y-res virtual |%u", var->yres_virtual);
	TEST_PRINT_TRC("X-offset |%u", var->xoffset);
	TEST_PRINT_TRC("Y-offset |%u", var->yoffset);
	TEST_PRINT_TRC("Bits per pixel |%u", var->bits_per_pixel);
	TEST_PRINT_TRC("Gray scale |%u", var->grayscale);
	TEST_PRINT_TRC("RGBA [rL/rO,gL/gO,bL/bO,tL/tO] |%u/%u,%u/%u,%u/%u,%u/%u",
			var->red.length, var->red.offset,
			var->green.length, var->green.offset,
			var->blue.length, var->blue.offset,
			var->transp.length, var->transp.offset);
	TEST_PRINT_TRC("Bitfield msb for transp |%u", var->transp.msb_right);
	TEST_PRINT_TRC("Non Standard mode |%u", var->nonstd);
	TEST_PRINT_TRC("Activate |%u", var->activate);
	TEST_PRINT_TRC("height in mm |%u", var->height);
	TEST_PRINT_TRC("width	in mm |%u", var->width);
	TEST_PRINT_TRC("Accel flags |%u", var->accel_flags);
	TEST_PRINT_TRC("Pixel Clock |%u", var->pixclock);
	TEST_PRINT_TRC("Left margin |%u", var->left_margin);
	TEST_PRINT_TRC("Right margin |%u", var->right_margin);
	TEST_PRINT_TRC("Upper margin |%u", var->upper_margin);
	TEST_PRINT_TRC("Lower margin |%u", var->lower_margin);
	TEST_PRINT_TRC("Hsync len |%u", var->hsync_len);
	TEST_PRINT_TRC("Vsync len |%u", var->vsync_len);
	TEST_PRINT_TRC("sync | %u", var->sync);
	TEST_PRINT_TRC("Vmode |%u", var->vmode);
	TEST_PRINT_TRC("Rotation angle |%u", var->rotate);

exit:
	/* close FBDEV device */
	ret |= st_fbdev_close_interface(params->device_name, &params->fd);

	/* print status of the test case */
	st_fbdev_display_test_status(ret, params->testcase_id);

	/* end test case */
	TEST_PRINT_TST_END(params->testcase_id);

	return ret;
}


/*
 * Individual IOCTL Test: get fixed screen info
 */
int st_fbdev_put_vscreeninfo_ioctl_test(struct fbdev_display_testparams *params)
{
	int ret;
	struct fb_var_screeninfo *var = &params->var;
	struct fb_var_screeninfo r_var;

	TEST_PRINT_TST_START("FBIOPUT_VSCREENINFO Ioctl Test|%s",
			params->testcase_id);

	/* Open the device */
	ret = st_fbdev_open_interface(params->device_name, &params->fd);
	if (ret != SUCCESS)
		return ret;

	ret = st_fbdev_put_vscreeninfo_interface(params->fd, var);
	if (ret)
		goto exit;

	/* get variable screen info */
	ret = st_fbdev_get_vscreeninfo_interface(params->fd, &r_var);
	if (ret)
		goto exit;

	TEST_PRINT_TRC("X-res |%u", r_var.xres);
	TEST_PRINT_TRC("Y-res |%u", r_var.yres);
	TEST_PRINT_TRC("X-res virtual |%u", r_var.xres_virtual);
	TEST_PRINT_TRC("Y-res virtual |%u", r_var.yres_virtual);
	TEST_PRINT_TRC("X-offset |%u", r_var.xoffset);
	TEST_PRINT_TRC("Y-offset |%u", r_var.yoffset);
	TEST_PRINT_TRC("Bits per pixel |%u", r_var.bits_per_pixel);
	TEST_PRINT_TRC("Gray scale |%u", r_var.grayscale);
	TEST_PRINT_TRC("RGBA [rL/rO,gL/gO,bL/bO,tL/tO] |%u/%u,%u/%u,%u/%u,%u/%u",
			r_var.red.length, r_var.red.offset,
			r_var.green.length, r_var.green.offset,
			r_var.blue.length, r_var.blue.offset,
			r_var.transp.length, r_var.transp.offset);
	TEST_PRINT_TRC("Bitfield msb for transp |%u", r_var.transp.msb_right);
	TEST_PRINT_TRC("Non Standard mode |%u", r_var.nonstd);
	TEST_PRINT_TRC("Activate |%u", r_var.activate);
	TEST_PRINT_TRC("height in mm |%u", r_var.height);
	TEST_PRINT_TRC("width	in mm |%u", r_var.width);
	TEST_PRINT_TRC("Accel flags |%u", r_var.accel_flags);
	TEST_PRINT_TRC("Pixel Clock |%u", r_var.pixclock);
	TEST_PRINT_TRC("Left margin |%u", r_var.left_margin);
	TEST_PRINT_TRC("Right margin |%u", r_var.right_margin);
	TEST_PRINT_TRC("Upper margin |%u", r_var.upper_margin);
	TEST_PRINT_TRC("Lower margin |%u", r_var.lower_margin);
	TEST_PRINT_TRC("Hsync len |%u", r_var.hsync_len);
	TEST_PRINT_TRC("Vsync len |%u", r_var.vsync_len);
	TEST_PRINT_TRC("sync | %u", r_var.sync);
	TEST_PRINT_TRC("Vmode |%u", r_var.vmode);
	TEST_PRINT_TRC("Rotation angle |%u", r_var.rotate);

exit:
	/* close FBDEV device */
	ret |= st_fbdev_close_interface(params->device_name, &params->fd);

	/* print status of the test case */
	st_fbdev_display_test_status(ret, params->testcase_id);

	/* end test case */
	TEST_PRINT_TST_END(params->testcase_id);

	return ret;
}

/*
 * Individual IOCTL Test: get fixed screen info
 */
int st_fbdev_get_fscreeninfo_ioctl_test(struct fbdev_display_testparams *params)
{
	int ret;
	struct fb_fix_screeninfo *fix = &params->fix;

	TEST_PRINT_TST_START("FBIOGET_FSCREENINFO Ioctl Test|%s",
			params->testcase_id);

	/* Open the device */
	ret = st_fbdev_open_interface(params->device_name, &params->fd);
	if (ret != SUCCESS)
		return ret;

	/* get fix screen info */
	ret = st_fbdev_get_fscreeninfo_interface(params->fd, fix);
	if (ret)
		goto exit;

	/* Print all Fix Screen information */

	TEST_PRINT_TRC("\n\nFix Screen Info: ");
	TEST_PRINT_TRC("Id |%s", fix->id);
	TEST_PRINT_TRC("Start of FB Mem |%ld", fix->smem_start);
	TEST_PRINT_TRC("length of FB Mem |%u", fix->smem_len);
	TEST_PRINT_TRC("FB Type |%u", fix->type);
	TEST_PRINT_TRC("Aux FB Type |%u", fix->type_aux);
	TEST_PRINT_TRC("Visual |%u", fix->visual);
	TEST_PRINT_TRC("X Pan Step (HW Pan) |%d", fix->xpanstep);
	TEST_PRINT_TRC("Y Pan Step (HW Pan) |%d", fix->ypanstep);
	TEST_PRINT_TRC("Y wrap (HW Y wrap) |%d", fix->ywrapstep);
	TEST_PRINT_TRC("Line Length |%u", fix->line_length);
	TEST_PRINT_TRC("Mmap IO Start (Phys Addr) |%ld", fix->mmio_start);
	TEST_PRINT_TRC("Mmap IO lenght |%u", fix->mmio_len);
	TEST_PRINT_TRC("Accel |%u", fix->accel);

exit:
	/* close FBDEV device */
	ret |= st_fbdev_close_interface(params->device_name, &params->fd);

	/* print status of the test case */
	st_fbdev_display_test_status(ret, params->testcase_id);

	/* end test case */
	TEST_PRINT_TST_END(params->testcase_id);

	return ret;
}


/*
 *  Individual IOCTL Test: get color map table
 */
int st_fbdev_fbio_getcmap_ioctl_test(struct fbdev_display_testparams *params)
{
	int ret;
	struct fb_cmap *cmap = &params->cmap;
	unsigned short r[4], g[4], b[4], t[4];

	TEST_PRINT_TST_START("FBIOGETCMAP Ioctl Test|%s", params->testcase_id);

	/* Open the device */
	ret = st_fbdev_open_interface(params->device_name, &params->fd);
	if (ret != SUCCESS)
		return ret;

	cmap->red = r;
	cmap->green = g;
	cmap->blue = b;
	cmap->transp = t;
	cmap->len = 4;
	/* get variable screen info */
	ret = st_fbdev_getcmap_ioctl_interface(params->fd, cmap);
	if (ret)
		goto exit;

	/* print color map table */
	TEST_PRINT_TRC("\n\nCMAP info: ");
	TEST_PRINT_TRC("CMAP Start |%d", cmap->start);
	TEST_PRINT_TRC("CMAP Length |%d", cmap->len);
	TEST_PRINT_TRC("Red[0 1 2 3] |%d %d %d %d", cmap->red[0],
				cmap->red[1], cmap->red[2], cmap->red[3]);
	TEST_PRINT_TRC("Green[0 1 2 3] |%d %d %d %d", cmap->green[0],
				cmap->green[1], cmap->green[2], cmap->green[3]);
	TEST_PRINT_TRC("Blue[0 1 2 3] |%d %d %d %d", cmap->blue[0],
				cmap->blue[1], cmap->blue[2], cmap->blue[3]);
	TEST_PRINT_TRC("Trans[0 1 2 3] |%d %d %d %d", cmap->transp[0],
				cmap->transp[1], cmap->transp[2], cmap->transp[3]);

exit:
	/* close FBDEV device */
	ret |= st_fbdev_close_interface(params->device_name, &params->fd);

	/* print status of the test case */
	st_fbdev_display_test_status(ret, params->testcase_id);

	/* end test case */
	TEST_PRINT_TST_END(params->testcase_id);

	return ret;
}

/*
 * Individual IOCTL Test: put color map table
 */
int st_fbdev_fbio_putcmap_ioctl_test(struct fbdev_display_testparams *params)
{
	int ret;
	struct fb_cmap *cmap = &params->cmap;
	unsigned short r[4] = {0xFF, 0x00, 0x00, 0xFF};
	unsigned short g[4] = {0x00, 0xFF, 0x00, 0xFF};
	unsigned short b[4] = {0x00, 0x00, 0xFF, 0x00};
	unsigned short t[4] = {0x00, 0x00, 0x00, 0x00};
	unsigned short rr[4], rg[4], rb[4], rt[4];

	TEST_PRINT_TST_START("FBIOPUTCMAP Ioctl Test|%s", params->testcase_id);

	/* Open the device */
	ret = st_fbdev_open_interface(params->device_name, &params->fd);
	if (ret != SUCCESS)
		return ret;

	memset(cmap, 0, sizeof(struct fb_cmap));
	cmap->red = r;
	cmap->blue = b;
	cmap->green = g;
	cmap->transp = t;
	cmap->len = 4;

	/* print color map table */
	TEST_PRINT_TRC("\n\nWritting CMAP Table: \n");
	TEST_PRINT_TRC("CMAP Start |%d", cmap->start);
	TEST_PRINT_TRC("CMAP Length |%d", cmap->len);
	TEST_PRINT_TRC("Red[0 1 2 3] |%d %d %d %d", cmap->red[0],
				cmap->red[1], cmap->red[2], cmap->red[3]);
	TEST_PRINT_TRC("Green[0 1 2 3] |%d %d %d %d", cmap->green[0],
				cmap->green[1], cmap->green[2], cmap->green[3]);
	TEST_PRINT_TRC("Blue[0 1 2 3] |%d %d %d %d", cmap->blue[0],
				cmap->blue[1], cmap->blue[2], cmap->blue[3]);
	TEST_PRINT_TRC("Trans[0 1 2 3] |%d %d %d %d", cmap->transp[0],
				cmap->transp[1], cmap->transp[2], cmap->transp[3]);
	/* put cmap table */
	ret = st_fbdev_putcmap_ioctl_interface(params->fd, cmap);
	if (ret)
		goto exit;

	/* Read back the CMAP table */
	memset(cmap, 0, sizeof(struct fb_cmap));
	cmap->red = rr;
	cmap->blue = rb;
	cmap->green = rg;
	cmap->transp = rt;
	cmap->len = 4;

	/* get variable screen info */
	ret = st_fbdev_getcmap_ioctl_interface(params->fd, cmap);
	if (ret)
		goto exit;

	/* print color map table */
	TEST_PRINT_TRC("\n\nReading back CMAP Table: \n");
	TEST_PRINT_TRC("CMAP Start |%d", cmap->start);
	TEST_PRINT_TRC("CMAP Length |%d", cmap->len);
	TEST_PRINT_TRC("Red[0 1 2 3] |%d %d %d %d", cmap->red[0],
				cmap->red[1], cmap->red[2], cmap->red[3]);
	TEST_PRINT_TRC("Green[0 1 2 3] |%d %d %d %d", cmap->green[0],
				cmap->green[1], cmap->green[2], cmap->green[3]);
	TEST_PRINT_TRC("Blue[0 1 2 3] |%d %d %d %d", cmap->blue[0],
				cmap->blue[1], cmap->blue[2], cmap->blue[3]);
	TEST_PRINT_TRC("Trans[0 1 2 3] |%d %d %d %d", cmap->transp[0],
				cmap->transp[1], cmap->transp[2], cmap->transp[3]);

exit:
	/* close FBDEV device */
	ret |= st_fbdev_close_interface(params->device_name, &params->fd);

	/* print status of the test case */
	st_fbdev_display_test_status(ret, params->testcase_id);

	/* end test case */
	TEST_PRINT_TST_END(params->testcase_id);

	return ret;
}


/*
 * IOCTL test cases: Based on IOCTL no, it executes particular IOCTL. Please
 * refer to the help for more details on ioctl no.s
 */

int st_fbdev_ioctl_test(struct fbdev_display_testparams *params)
{
	int i, ret = 0;

	switch(params->ioctl_no) {
	case OPTION_IOCTL_GETVSCREENINFO:
		for(i=0; i< params->loopcount; i++) {
			ret = st_fbdev_get_vscreeninfo_ioctl_test(params);
			if (ret)
				goto exit;
		}

		break;
	case OPTION_IOCTL_PUTVSCREENINFO:
		for(i=0; i< params->loopcount; i++) {
			ret = st_fbdev_put_vscreeninfo_ioctl_test(params);
			if (ret)
				goto exit;
		}

		break;
	case OPTION_IOCTL_GETFSCREENINFO:
		for(i=0; i< params->loopcount; i++) {
			ret = st_fbdev_get_fscreeninfo_ioctl_test(params);
			if (ret)
				goto exit;
		}

		break;
	case OPTION_IOCTL_BLANK:
		for(i=0; i< params->loopcount; i++) {
			ret = st_fbdev_fbio_blank_ioctl_test(params);
			if (ret)
				goto exit;
		}

		break;
	case OPTION_IOCTL_GETCMAP:
		for(i=0; i< params->loopcount; i++) {
			ret = st_fbdev_fbio_getcmap_ioctl_test(params);
			if (ret)
				goto exit;
		}
		break;

	case OPTION_IOCTL_PUTCMAP:
		for(i=0; i< params->loopcount; i++) {
			ret = st_fbdev_fbio_putcmap_ioctl_test(params);
			if (ret)
				goto exit;
		}

		break;
	default:
		break;
	}

exit:
	return ret;

}

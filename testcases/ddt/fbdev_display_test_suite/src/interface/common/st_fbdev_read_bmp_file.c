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

#define BMP_BITMAP_DATA_OFF	10
#define BMP_WIDTH_OFF		18
#define BMP_BPP_OFF		28
#define BMP_RLE_OFF		30
#define BMP_COLOR_OFF		54

#define fill4B(a)       ( ( 4 - ( (a) % 4 ) ) & 0x03)

static int get_bmp_res(int fd, unsigned int *width, unsigned int *height)
{
	int ret;
	unsigned char buf[4];

	ret = lseek(fd, BMP_WIDTH_OFF, SEEK_SET);
	if (ret == -1) {
		perror("failed to set file fp: ");
		return -EINVAL;
	}

	read(fd, buf, 4);
        *width = buf[0] + (buf[1] << 8) + (buf[2] << 16) + (buf[3] << 24);

	read(fd, buf, 4);
	*height = buf[0] + (buf[1] << 8) + (buf[2] << 16) + (buf[3] << 24);

	TEST_PRINT_TRC("Input File Res(WxH) |%dx%d", *width, *height);
	return SUCCESS;
}

static int read_image_from_file(struct fbdev_display_testparams *params,
		unsigned char *buffer)
{
	int j, k, ret;
	int fd, bpp, bitmap_off;
	unsigned int width, height;
	unsigned char buf[4];

	fd = open(params->file_name, O_RDONLY);
	if (fd == -1) {
		perror("file open: ");
		return -EINVAL;
	}

	TEST_PRINT_TRC("Input File |%s", params->file_name);

	ret = get_bmp_res(fd, &width, &height);
	if (ret)
		goto exit;

	ret = lseek(fd, BMP_BPP_OFF, SEEK_SET);
	if (ret == -1) {
		perror("1: failed to set file fp: ");
		goto exit;
	}

	read(fd, buf, 2);
	bpp = buf[0] + (buf[1] << 8);

	TEST_PRINT_TRC("Input file BPP |%d", bpp);

	ret = lseek(fd, BMP_BITMAP_DATA_OFF, SEEK_SET);
	if (ret == -1) {
		perror("2: failed to set file fp: ");
		return -EINVAL;
	}
	read(fd, buf, 4);
	bitmap_off = buf[0] + (buf[1] << 8) + (buf[2] << 16) + (buf[3] << 24);
	TEST_PRINT_TRC("Bitmap Off |%d", bitmap_off);

	if (lseek(fd, bitmap_off, SEEK_SET) == -1) {
		perror("3: failed to set file fp: ");
		return -EINVAL;
	}

	switch (bpp) {
	case 16:
		{
			unsigned short *start = (unsigned short *)
				(buffer + params->fix.line_length * (height - 1));

			for(j = 0 ; j < height ; j++) {
				for (k = 0; k < width; k++) {
					read(fd, buf, 2);
					start[k] = buf[0] + (buf[1] << 8);
				}
				start -= (params->fix.line_length/2);
			}
		}
		break;
	case 24:
		{
			unsigned int *start = (unsigned int *)
				(buffer + params->fix.line_length * (height - 1));

			for(j = 0 ; j < height ; j++) {
				for(k = 0 ; k < width; k++) {
					read(fd, buf, 3);
					start[k] = buf[0] + (buf[1] << 8) + (buf[2] << 16);
				}
				start -= (params->fix.line_length/4);
			}
		}
		break;
	case 32:
		{
			unsigned int *start = (unsigned int *)
				(buffer + params->fix.line_length * (height - 1));

			for(j = 0 ; j < height ; j++) {
				for(k = 0 ; k < width; k++) {
					read(fd, buf, 4);
					start[k] = buf[0] + (buf[1] << 8) +
						(buf[2] << 16) + (buf[3] << 24);
				}
				start -= (params->fix.line_length/4);
			}
		}
		break;
	default:
		TEST_PRINT_TRC("Invalid bpp");
		break;
	}

exit:
	close(fd);
	return ret;
}

int st_fbdev_display_bmp_test(struct fbdev_display_testparams *params)
{
	int ret, status = SUCCESS;
	unsigned char* buf_addr;

	/* Open the device */
	ret = st_fbdev_open_interface(params->device_name, &params->fd);
	if (ret != SUCCESS)
		return ret;

	/* Enable window */
	ret = st_fbdev_blank_interface(params->fd, VESA_NO_BLANKING);
	if (ret) {
		status = FAILURE;
		goto exit;
	}
	TEST_PRINT_TRC("FBIO_BLANK Ioctl passed");

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

	TEST_PRINT_TRC("Reading image data from file");
	read_image_from_file(params, buf_addr);

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

	return ret;
}

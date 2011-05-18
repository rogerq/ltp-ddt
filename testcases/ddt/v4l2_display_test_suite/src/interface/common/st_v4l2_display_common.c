/*
 * st_v4l2_display_common.c
 *
 * This file contains the common functions definitions
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

/* V4L2_DISPLAY structs, enums, macros defined */
#include "st_v4l2_display_common.h"

/* Buffers for display */
static struct buf_info display_buff_info[MAX_BUFFERS];
static unsigned long buffer_addr[MAX_BUFFERS];

/*color pattern RGB565*/
static unsigned int rgb565[8] = {
	(0x1F << 11) | (0x3F << 5) | (0x1F),
	(0x00 << 11) | (0x00 << 5) | (0x00),
	(0x1F << 11) | (0x00 << 5) | (0x00),
	(0x00 << 11) | (0x3F << 5) | (0x00),
	(0x00 << 11) | (0x00 << 5) | (0x1F),
	(0x1F << 11) | (0x3F << 5) | (0x00),
	(0x1F << 11) | (0x00 << 5) | (0x1F),
	(0x00 << 11) | (0x3F << 5) | (0x1F),
};

static unsigned int rgb888[8] = {
	(0xFF << 16) | (0xFF << 8) | (0xFF),
	(0x00 << 16) | (0x00 << 8) | (0x00),
	(0xFF << 16) | (0x00 << 8) | (0x00),
	(0x00 << 16) | (0xFF << 8) | (0x00),
	(0x00 << 16) | (0x00 << 8) | (0xFF),
	(0xFF << 16) | (0xFF << 8) | (0x00),
	(0xFF << 16) | (0x00 << 8) | (0xFF),
	(0x00 << 16) | (0xFF << 8) | (0xFF),
};

static inline int config_dss_sysfs(const char *file, char *val)
{
        char f_name[100];

        sprintf(f_name, "/sys/devices/platform/omapdss/%s", file);
        return write_to_sysfs(f_name, val);
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
* Time difference for framerate calculation
*/
static int timeval_subtract(struct timeval *result, struct timeval *x,
                                     struct timeval *y)
{
        /* Perform the carry for the later subtraction by updating y.
         * */
        if (x->tv_usec < y->tv_usec) {
                int nsec = (y->tv_usec - x->tv_usec) /
                        1000000 + 1;
                y->tv_usec -= 1000000 * nsec;
                y->tv_sec += nsec;
        }
        if (x->tv_usec - y->tv_usec > 1000000) {
                int nsec = (x->tv_usec - y->tv_usec) /
                        1000000;
                y->tv_usec += 1000000 * nsec;
                y->tv_sec -= nsec;
        }

        /* Compute the time remaining to wait.
           tv_usec is certainly positive. */
        result->tv_sec = x->tv_sec - y->tv_sec;
        result->tv_usec = x->tv_usec - y->tv_usec;

        /* Return 1 if result is
         * negative. */
        return x->tv_sec < y->tv_sec;
}

/*
* This function generates the color bars
*/
void colorbar_generate(unsigned char *addr, int width,
	 int height, int order, int pixelfmt)
{
	unsigned short *ptr = (unsigned short *)addr + order*width;
	int i, j, k;

	switch (pixelfmt) {
	case RGB32:
	case RGB24:
		for (i = 0; i < 8; i++) {
			for (j = 0; j < (height/8); j++) {
				for (k = 0; k < width; k++)
					ptr[k] = rgb888[i];
				ptr = ptr + width;
				if ((unsigned int)ptr >=
					 ((unsigned int)addr +
						 (width * 2) * height))
					ptr = (unsigned short *)addr;
			}
		}
		break;
	case RGB565:
	case NV16:
	case YUYV:
	case UVVY:
	default:
		for (i = 0; i < 8; i++) {
			for (j = 0; j < (height/8); j++) {
				for (k = 0; k < width; k++)
					ptr[k] = rgb565[i];
				ptr = ptr + width;
				if ((unsigned int)ptr >=
					((unsigned int)addr +
						 (width * 2) * height))
					ptr = (unsigned short *)addr;
			}
		}
		break;
	}
}

/*
* This function opens V4L2_DISPLAY window
*/
int st_v4l2_display_open(char *name, int *dev)
{
	int retVal;

	retVal = open(name, O_RDWR);
	if (0 >= retVal) {
		perror("Open: ");
		return retVal;
	}

	*dev = retVal;

	return retVal;
}

/*
* This function opens Framebuffer window
*/
int st_fbdev_display_open(char *name, int *dev)
{
	int retVal;

	retVal = open(name, O_RDWR);
	if (0 >= retVal) {
		perror("Open: ");
		return retVal;
	}

	*dev = retVal;

	return retVal;
}

/*
* This function closes V4L2_DISPLAY window
*/
int st_v4l2_display_close(int dev)
{
	int retVal = SUCCESS;

	retVal = close(dev);
	if (0 != retVal) {
		perror("Close: ");
		return retVal;
	}

	return retVal;
}

/*
* This function closes Framebuffer window
*/
int st_fbdev_display_close(int dev)
{
	int retVal = SUCCESS;

	retVal = close(dev);
	if (0 != retVal) {
		perror("Close: ");
		return retVal;
	}

	return retVal;
}

/*
* This function sets V4L2_DISPLAY width,height & pixel
*/
int st_v4l2_display_set_format(int dev, struct v4l2_format *st_fmt)
{
	int retVal = SUCCESS;

	retVal = ioctl(dev, VIDIOC_S_FMT, st_fmt);
	if (SUCCESS > retVal) {
		perror("VIDIOC_S_FMT");
	}

	return retVal;
}

/*
* This function gets V4L2_DISPLAY width,height & pixel
*/
int st_v4l2_display_get_format(int dev, struct v4l2_format *st_fmt)
{
	int retVal = SUCCESS;

	retVal = ioctl(dev, VIDIOC_G_FMT, st_fmt);
	if (SUCCESS > retVal) {
		perror("VIDIOC_G_FMT");
		return retVal;
	}

	return retVal;
}

/*
* This function implements the REQBUF ioctl
*/

int st_v4l2_display_request_buffer_mmap(int dev,
	 struct v4l2_requestbuffers *st_reqbuf)
{
	int retVal = SUCCESS;

	retVal = ioctl(dev, VIDIOC_REQBUFS, st_reqbuf);

	if (SUCCESS > retVal) {
		perror("VIDIOC_REQBUFS");
		return retVal;
	}

	return retVal;
}

/*
* This function implements the QUERYBUF ioctl and mmap
*/
int st_v4l2_display_query_buffer_mmap(int dev, int count,
	struct v4l2_format *st_fmt, struct v4l2_buffer *st_buf, int pixelfmt)
{

	int retVal = SUCCESS;
	int i = 0;

	for (i = 0; i < count ; i++) {
		st_buf->index = i;
		retVal = ioctl(dev, VIDIOC_QUERYBUF, st_buf);
		if (SUCCESS > retVal) {
			perror("VIDIOC_QUERYBUF");
			return retVal;
		}

		/* Mmap */
		display_buff_info[i].length = st_buf->length;
		display_buff_info[i].index = i;

		display_buff_info[i].start =
			mmap(NULL, st_buf->length, PROT_READ |
					PROT_WRITE, MAP_SHARED,
						 dev, st_buf->m.offset);
		if ((unsigned int *)display_buff_info[i].start == MAP_FAILED ||
				(unsigned int)display_buff_info[i].start
						 == MAP_SHARED) {
			return retVal;
		}

		memset(display_buff_info[i].start, 0x80, st_buf->length);
		colorbar_generate(display_buff_info[i].start,
				st_fmt->fmt.pix.width, st_fmt->fmt.pix.height,
				0, pixelfmt);
	}

	return retVal;
}

/*
* This function maps the buffers in userpointer technique
*/
int st_v4l2_display_buffer_userptr(int dev_fb, int buffersize)
{
	int i = 0;

	buffer_addr[0] = (unsigned long)mmap((void *)0,
				 buffersize * MAX_BUFFERS,
					(PROT_READ|PROT_WRITE),
						 MAP_SHARED, dev_fb, 0);
	if (buffer_addr[0] == (unsigned long)MAP_FAILED)
		return FAILURE;

	memset((void *)buffer_addr[0], 0, buffersize * MAX_BUFFERS);

	/* Store each buffer addresses in the local variable */
	for (i = 1; i < MAX_BUFFERS; i++) {
		buffer_addr[i] = buffer_addr[i-1] + buffersize;
		memset((void *)buffer_addr[i], 0, sizeof(testoptions.st_buf));
	}
	return SUCCESS;
}

/*
* This function enqueues the buffers
*/
int st_v4l2_display_queue_buffers_mmap(int dev, int count,
	 struct v4l2_buffer *st_buf)
{

	int retVal = SUCCESS;
	int i = 0;

	for (i = 0; i < count ; i++) {
		st_buf->index = i;

		retVal = ioctl(dev, VIDIOC_QBUF, st_buf);
		if (retVal < 0) {
			perror("VIDIOC_QBUF");
			return retVal;
		}
	}

	if (SUCCESS > retVal) {
		perror("VIDIOC_QBUF");
		return retVal;
	}

	return retVal;
}

/*
* This function enqueues the buffers
*/
int st_v4l2_display_queue_buffers_userptr(int dev, int count,
	struct v4l2_buffer *st_buf)
{

	int retVal = SUCCESS;
	int i = 0;

	for (i = 0; i < count ; i++) {
		st_buf->index = i;
		st_buf->m.userptr = buffer_addr[i];

		retVal = ioctl(dev, VIDIOC_QBUF, st_buf);
		if (retVal < 0) {
			perror("VIDIOC_QBUF");
			return retVal;
		}
	}

	if (SUCCESS > retVal) {
		perror("VIDIOC_QBUF");
		return retVal;
	}

	return retVal;
}

/*
* This function does the display of color bars
*/
int st_v4l2_display_color_bar_userptr(int dev, int noframes,
	struct v4l2_format *st_fmt, struct v4l2_buffer *st_buf,
		 int buffer_size, int pixelfmt)
{
	int retVal = SUCCESS;
	int counter = 0;
	unsigned int cpu_load;

	struct timeval before, after, result;
	struct proc_stat cpuload;

	start_cpuload_measurement(&cpuload);
	gettimeofday(&before, NULL);

	for (counter = 0;  counter < noframes; counter++) {
		retVal = ioctl(dev, VIDIOC_DQBUF, st_buf);
		if (SUCCESS > retVal) {
			perror("VIDIOC_DQBUF");
			return retVal;
		}
		/* putting moving color bars */
		colorbar_generate((void *)buffer_addr[st_buf->index],
				st_fmt->fmt.pix.width, st_fmt->fmt.pix.height,
				counter%(st_fmt->fmt.pix.height/2), pixelfmt);

		/* Now queue it back to display it */
		st_buf->index = st_buf->index;
		st_buf->m.userptr = buffer_addr[st_buf->index];

		st_buf->type =  V4L2_BUF_TYPE_VIDEO_OUTPUT;
		st_buf->memory = V4L2_MEMORY_USERPTR;
		st_buf->length = buffer_size;

		retVal = ioctl(dev, VIDIOC_QBUF, st_buf);
		if (SUCCESS > retVal) {
			perror("VIDIOC_QBUF");
			return retVal;
		}
	}

        	gettimeofday(&after, NULL);
	timeval_subtract(&result, &after, &before);

        	TEST_PRINT_TRC("Calculated Frame Rate:\t%ld Fps", noframes/result.tv_sec);

	cpu_load = stop_cpuload_measurement(&cpuload);
	TEST_PRINT_TRC("The V4L2 Display : Percentage CPU Load = %u%%",
				cpu_load);
	return retVal;
}

/*
* This function does the display of color bars
*/
int st_v4l2_display_color_bar(int dev, int noframes,
	struct v4l2_format *st_fmt, struct v4l2_buffer *st_buf, int pixelfmt)
{
	int retVal = SUCCESS;
	int counter = 0;

	for (counter = 0;  counter < noframes; counter++) {
		retVal = ioctl(dev, VIDIOC_DQBUF, st_buf);
		if (SUCCESS > retVal)
			return retVal;

		/* putting moving color bars */
		colorbar_generate(display_buff_info[st_buf->index].start,
				st_fmt->fmt.pix.width, st_fmt->fmt.pix.height,
				counter%(st_fmt->fmt.pix.height/2), pixelfmt);

		/* Now queue it back to display it */
		st_buf->index = st_buf->index;

		retVal = ioctl(dev, VIDIOC_QBUF, st_buf);
		if (SUCCESS > retVal)
			return retVal;
	}

	return retVal;
}

/*
* This function implements the STREAMON ioctl
*/
int st_v4l2_display_streamon(int dev)
{
	int type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	int retVal = SUCCESS;

	retVal = ioctl(dev, VIDIOC_STREAMON, &type);

	if (SUCCESS > retVal) {
		perror("VIDIOC_STREAMON");
		return retVal;
	}

	return retVal;
}

/*
* This function implements the STREAMON ioctl
*/
int st_v4l2_display_streamoff(int dev)
{
	int type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	int retVal = SUCCESS;

	retVal = ioctl(dev, VIDIOC_STREAMOFF, &type);

	if (SUCCESS > retVal) {
		perror("VIDIOC_STREAMOFF");
		return retVal;
	}

	return retVal;
}

/*
* This function implements the unmap call
*/
int st_v4l2_display_unmap_mmap(int count)
{
	int i = 0;
	int retVal = SUCCESS;

	for (i = 0; i < count; i++)
		retVal = munmap(display_buff_info[i].start,
				display_buff_info[i].length);
	if (SUCCESS > retVal)
		return retVal;

	return retVal;
}

/*
* This function implements the unmap call
*/
int st_v4l2_display_unmap_userptr(int buffer_size, int count)
{
	int i = 0;
	int retVal = SUCCESS;

	for (i = 0; i < count; i++)
		retVal = munmap((void *)buffer_addr[i],
				buffer_size);
	if (SUCCESS > retVal)
		return retVal;

	return retVal;
}

/*
* This function set the control
*/
int st_v4l2_set_control(int dev, struct v4l2_control *st_control)
{
	int retVal = SUCCESS;

	retVal = ioctl(dev, VIDIOC_S_CTRL, st_control);
	if (retVal < 0) {
		perror("VIDIOC_S_CTRL");
		return retVal;
	}

	return retVal;
}

/*
* This function get the control
*/
int st_v4l2_get_control(int dev, struct v4l2_control *st_control)
{
	int retVal = SUCCESS;

	retVal = ioctl(dev, VIDIOC_G_CTRL, st_control);
	if (retVal < 0) {
		perror("VIDIOC_S_CTRL");
		return retVal;
	}

	TEST_PRINT_TRC("The control value set to = %d", st_control->value);
	return retVal;
}

/*
* This function implements the S_CROP ioctl
*/
int st_v4l2_display_set_crop(int dev, struct v4l2_crop *st_crop)
{

	int retVal = SUCCESS;

	retVal = ioctl(dev, VIDIOC_S_CROP, st_crop);
	if (retVal < 0) {
		perror("VIDIOC_S_CROP");
		return retVal;
	}

	return retVal;
}

/*
* This function implements the G_CROP ioctl
*/
int st_v4l2_display_get_crop(int dev, struct v4l2_crop *st_crop)
{

	int retVal = SUCCESS;

	retVal = ioctl(dev, VIDIOC_G_CROP, st_crop);
	if (retVal < 0) {
		perror("VIDIOC_S_CROP");
		return retVal;
	}

	return retVal;
}

/*
* This function implements scaling
*/
int st_v4l2_display_set_scaling(int dev,
	struct v4l2_format *st_fmt, struct v4l2_crop *st_crop)
{
	int retVal = SUCCESS;

	retVal = ioctl(dev, VIDIOC_S_FMT, st_fmt);
	if (retVal < 0) {
		perror("VIDIOC_S_FMT");
		return retVal;
	}

	retVal = ioctl(dev, VIDIOC_S_CROP, st_crop);
	if (retVal < 0) {
		perror("VIDIOC_S_CROP");
		return retVal;
	}

	TEST_PRINT_TRC("The zoom height of the image | %d", st_crop->c.height);
	TEST_PRINT_TRC("The zoom width of the image | %d", st_crop->c.width);
	TEST_PRINT_TRC("The zoom top of the image | %d", st_crop->c.top);
	TEST_PRINT_TRC("The zoom left of the image | %d", st_crop->c.left);

	return retVal;
}

/*
* This function initilaizes the default values for
*/
void init_v4l2_display_test_params(struct v4l2_display_testparams *testoptions)
{
	testoptions->device_name = DEFAULT_NODE;
	testoptions->st_reqbuf.count = DEFAULT_NO_BUFFERS;
	testoptions->loop = DEFAULT_LOOP;
	testoptions->cpuload = FALSE;
	testoptions->framerate = FALSE;
	testoptions->st_control.value = DEFAULT_CTRL_VALUE;
	testoptions->device_name_fb = DEFAULT_NODE_FB;
	testoptions->userptr = FALSE;
}


/*
* This function implements the CROPCAP ioctl
*/
int st_v4l2_display_query_crop(int dev, struct v4l2_cropcap *cropcap)
{
	int retVal = SUCCESS;

	retVal = ioctl(dev, VIDIOC_CROPCAP, cropcap);
	if (retVal < 0) {
		perror("VIDIOC_CROPCAP");
		return retVal;
	}
	return retVal;
}

/*
* This function implements the QUERYCAP ioctl
*/
int st_v4l2_display_querycap(int dev,
	struct v4l2_capability *capability)
{
	int retVal = SUCCESS;

	retVal = ioctl(dev, VIDIOC_QUERYCAP, capability);
	if (retVal < 0) {
		perror("VIDIOC_QUERYCAP");
		return retVal;
	}
	if (capability->capabilities & V4L2_CAP_VIDEO_OUTPUT) {
		TEST_PRINT_TRC("Display capability is supported");
	} else {
		TEST_PRINT_TRC("Display capability is not supported");
	}

	if (capability->capabilities & V4L2_CAP_STREAMING) {
		TEST_PRINT_TRC("Streaming is supported");
	} else {
		TEST_PRINT_TRC("Streaming is not supported");
	}

	return retVal;
}

/*
* This function implements the enum fmt ioctl
*/
int st_v4l2_display_enum_fmt(int dev, struct v4l2_fmtdesc *fmtdesc)
{
	int retVal = SUCCESS;
	int i = 0;
	while (1) {
		fmtdesc->index = i;
		retVal = ioctl(dev, VIDIOC_ENUM_FMT, fmtdesc);
		if (retVal < 0) {
			perror("VIDIOC_ENUM_FMT");
			break;
			return retVal;
		}
		TEST_PRINT_TRC("description = %s\n", fmtdesc->description);
		if (fmtdesc->type == V4L2_BUF_TYPE_VIDEO_OUTPUT)
			TEST_PRINT_TRC("Video Display type\n");

		if (fmtdesc->pixelformat == V4L2_PIX_FMT_UYVY)
			TEST_PRINT_TRC("V4L2_PIX_FMT_UYVY\n");
		i++;
	}
	return retVal;
}

/*
* This function sets V4L2_DISPLAY width,height & pixel
*/
int st_v4l2_display_try_format(int dev, struct v4l2_format *st_fmt)
{
	int retVal = SUCCESS;

	retVal = ioctl(dev, VIDIOC_TRY_FMT, st_fmt);
	if (SUCCESS > retVal) {
		perror("VIDIOC_TRY_FMT");
		return retVal;
	}

	return retVal;
}

/*
* This function sets the pixel format
*/
int chk_pixel_format(int pixfmt)
{
	switch (pixfmt) {
	case RGB565:
		return V4L2_PIX_FMT_RGB565;
		break;
	case YUYV:
		return V4L2_PIX_FMT_YUYV;
		break;
	case UVVY:
		return V4L2_PIX_FMT_UYVY;
		break;
	case RGB24:
		return V4L2_PIX_FMT_RGB24;
		break;
	case RGB32:
		return V4L2_PIX_FMT_RGB32;
		break;
	case NV16:
		return V4L2_PIX_FMT_NV16;
		break;
	default:
		return V4L2_PIX_FMT_RGB565;
		break;
	}
}

/*
 * Get fixed screen info
 */
int st_fbdev_get_fscreeninfo_interface(int fd, struct fb_fix_screeninfo *st_fix)
{
	int retVal = SUCCESS;

	retVal = ioctl(fd, FBIOGET_FSCREENINFO, st_fix);
	if (SUCCESS > retVal)
		perror("FBIOGET_FSCREENINFO: ");

	return retVal;
}

/*
 * Get variable screen info
 */
int st_fbdev_get_vscreeninfo_interface(int fd, struct fb_var_screeninfo *st_var)
{
	int retVal = SUCCESS;

	retVal = ioctl(fd, FBIOGET_VSCREENINFO, st_var);
	if (SUCCESS > retVal)
		perror("FBIOGET_VSCREENINFO: ");

	return retVal;
}

/* Set the requested display output */
int st_set_output_interface(struct v4l2_display_testparams
		*options)
{

	/*
	 * In case of OMAP35x we are supporting 3 outputs
	 *	0 = Switch to LCD
	 *	1 = Switch to DVI
	 *	2 = Switch to TV
	 */
	switch (options->output) {
	case 0:
		TEST_PRINT_TRC("Switching output to LCD output\n");
		/* Disable overlays and displays and break the link */
		config_dss_sysfs("overlay0/enabled", "0");
		config_dss_sysfs("overlay1/enabled", "0");
		config_dss_sysfs("overlay2/enabled", "0");
		config_dss_sysfs("overlay0/manager", "\n");
		config_dss_sysfs("overlay1/manager", "\n");
		config_dss_sysfs("overlay2/manager", "\n");
		config_dss_sysfs("display0/enabled", "0");
		config_dss_sysfs("display1/enabled", "0");
		config_dss_sysfs("display2/enabled", "0");

		/* Set the links and enable overlays: Currentl only GFX is
		 * enabled */
		config_dss_sysfs("overlay0/manager", "lcd");
		config_dss_sysfs("overlay1/manager", "lcd");
		config_dss_sysfs("overlay2/manager", "lcd");
		config_dss_sysfs("manager0/display", "lcd");
		config_dss_sysfs("overlay0/enabled", "1");
		config_dss_sysfs("display0/enabled", "1");
		break;
	case 1:
		TEST_PRINT_TRC("Switching output to DVI output\n");
		/* Disable overlays and displays and break the link */
		config_dss_sysfs("overlay0/enabled", "0");
		config_dss_sysfs("overlay1/enabled", "0");
		config_dss_sysfs("overlay2/enabled", "0");
		config_dss_sysfs("overlay0/manager", "\n");
		config_dss_sysfs("overlay1/manager", "\n");
		config_dss_sysfs("overlay2/manager", "\n");
		config_dss_sysfs("display0/enabled", "0");
		config_dss_sysfs("display1/enabled", "0");
		config_dss_sysfs("display2/enabled", "0");

		/* Set the links and enable overlays: Currentl only GFX is
		 * enabled */
		config_dss_sysfs("manager0/display", "dvi");
		config_dss_sysfs("overlay0/manager", "lcd");
		config_dss_sysfs("overlay1/manager", "lcd");
		config_dss_sysfs("overlay2/manager", "lcd");
		config_dss_sysfs("overlay0/enabled", "1");
		config_dss_sysfs("display2/enabled", "1");
		break;
	case 2:
		TEST_PRINT_TRC("Switching output to TV output\n");
		/* Disable overlays and displays and break the link */
		config_dss_sysfs("overlay0/enabled", "0");
		config_dss_sysfs("overlay1/enabled", "0");
		config_dss_sysfs("overlay2/enabled", "0");
		config_dss_sysfs("overlay0/manager", "\n");
		config_dss_sysfs("overlay1/manager", "\n");
		config_dss_sysfs("overlay2/manager", "\n");
		config_dss_sysfs("display0/enabled", "0");
		config_dss_sysfs("display1/enabled", "0");
		config_dss_sysfs("display2/enabled", "0");

		config_dss_sysfs("overlay0/manager", "tv");
		config_dss_sysfs("overlay1/manager", "tv");
		config_dss_sysfs("overlay2/manager", "tv");
		config_dss_sysfs("overlay0/enabled", "1");
		config_dss_sysfs("display1/enabled", "1");
		break;
	default:
		TEST_PRINT_ERR("Wrong output device\n");
		return -EINVAL;
	}

	return 0;
}


/*
* This function displays the help/usage
*/
void display_v4l2_display_test_suite_help(void)
{
	printf("v4l2DisplayTestSuite");
	printf("Usage:\n"
			"./v4l2_display_tests <options>\n\n"
			"-device_disp          --device_disp    Display device node\n"
			"\t\t\t\tPossible values-/dev/video2, /dev/video1\n"
			"-pixelformat          --pixelformat    Pixelformat\n"
			"\t\t\t\tPossible values-1:RGB565, 2:YUYV,\n"
			"\t\t\t\t3:UVVY, 4:RGB24, 5:RGB32, 6:NV16\n\n"
			"-width                --width          Width to display\n"
			"-height               --height         Height to display\n"
			"-loop                 --loop           Number of loop\n"
			"-buffernos            --buffernos      Number of buffers\n"
			"-buf_type             --buf_type       1:OUTPUT, 2:OVERLAY\n"
			"-id                   --id             Testcase ID\n"
			"-control_value        --control_value  Value for control\n"
			"\t\t\t\tPossible values-0,90,180,270 in case rotation to be set\n\n"
			"-control_id           --control_id      See following values\n"
			"\t\t\t\tPossible values-1: For rotation, 2: For Background color\n\n"
			"-fmt_width            --fmt_width      Windowing width\n"
			"-fmt_height           --fmt_height     Windowing height\n"
			"-fmt_top              --fmt_top        Windowing top\n"
			"-fmt_left             --fmt_left       Windowing left\n"
			"-crop_width           --crop_width     Cropping width\n"
			"-crop_height          --crop_height    Cropping height\n"
			"-crop_top             --crop_top       Cropping top\n"
			"-crop_left            --crop_left      Cropping left\n"
			"-userptr              --userptr        Flag for userpointer technique\n"	
			"-ioctl_no             --ioctl_no       IOCTL number for IOCTL test\n"
			"\t\t\t\tPossible values-\n"
			"\t\t\t\t 0:CROP_CAP, 1:QUERY_CAP, 2:TRY_FMT\n" 
			"\t\t\t\t 3:G_FMT, 4:G/S FMT, 5:G_CTRL, 6:S:CTRL \n" 
			"\t\t\t\t 7:G_CROP, 8:G/S CROP, 9:G/S FMT(SCALE)\n"
			"\t\t\t\t 10:REQBUF, 11:QUERYBUF, 12:ENQBUF\n"
			"\t\t\t\t 13:STREAM_ON/OFF\n");
}

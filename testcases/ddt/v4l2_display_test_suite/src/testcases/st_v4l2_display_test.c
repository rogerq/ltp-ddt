/*
 * st_v4l2_display_test.c
 *
 * This file calls V4L2 Display driver functions to display color bar
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

/* function prototypes for interface functions are defined here */
#include <st_v4l2_display_common.h>
 
static int calc_result_time(struct timeval *result, struct timeval *after,
		                                     struct timeval *before)
{
	TEST_PRINT_TRC("After=$after Before=$before");
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


/* Function to display image of given
*  width*height on the V4L2 supported windows
*/
int st_v4l2_display_test(struct v4l2_display_testparams *testoptions)
{
	int retVal = SUCCESS;
	int status = SUCCESS;
	static int buffer_size;
	unsigned int cpu_load;
	struct proc_stat cpuload;
	struct timeval before, after, result;

	/* variables to track device state,
	 *  mapping of buffers and state of V4L2 window
	 */

	TEST_PRINT_TST_START(testoptions->testcase_id);

	/* open V4L2 display device */
	retVal = st_v4l2_display_open(testoptions->device_name,
			&testoptions->st_dev);
	if (SUCCESS > retVal) {
		TEST_PRINT_ERR("Failed to open V4L2 device %s",
				testoptions->device_name);
		status = FAILURE;
		goto exit3;
	}
	TEST_PRINT_TRC("V4L2 device node %s opened",
			testoptions->device_name);

	/* get format */
	testoptions->st_fmt.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	retVal = st_v4l2_display_get_format(testoptions->st_dev,
			&testoptions->st_fmt);
	if (SUCCESS > retVal) {
		TEST_PRINT_ERR("G_FMT Ioctl Failed");
		status = FAILURE;
		goto exit2;
	}
	TEST_PRINT_TRC("G_FMT Ioctl Passed");

	TEST_PRINT_TRC("Initial values of format as below");

	TEST_PRINT_TRC("Width of display image :%d",
			testoptions->st_fmt.fmt.pix.width);
	TEST_PRINT_TRC("Height of display image:%d",
			testoptions->st_fmt.fmt.pix.height);
	TEST_PRINT_TRC("Size of display image:%d",
			testoptions->st_fmt.fmt.pix.sizeimage);
	TEST_PRINT_TRC("Bytesperline for display:%d",
			testoptions->st_fmt.fmt.pix.bytesperline);

	if (testoptions->st_control.id != 0) {
		/* set control */
		retVal = st_v4l2_set_control(testoptions->st_dev,
				&testoptions->st_control);
		if (SUCCESS > retVal) {
			TEST_PRINT_ERR("Set Control Failed");
			status = FAILURE;
			goto exit2;
		}
		TEST_PRINT_TRC("Set Control Passed");

		/* get rotation */
		retVal = st_v4l2_get_control(testoptions->st_dev,
				&testoptions->st_control);
		if (SUCCESS > retVal) {
			TEST_PRINT_ERR("GET Control Failed");
			status = FAILURE;
			goto exit2;
		}
		TEST_PRINT_TRC("GET Control Passed");
	}

	testoptions->st_fmt.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	if (testoptions->width != 0 || testoptions->height != 0) {
		testoptions->st_fmt.fmt.pix.width = testoptions->width;
		testoptions->st_fmt.fmt.pix.height = testoptions->height;
	}

	if (testoptions->pixelformat != 0)
		testoptions->st_fmt.fmt.pix.pixelformat
			= chk_pixel_format(testoptions->pixelformat);

	retVal = st_v4l2_display_set_format(testoptions->st_dev,
			&testoptions->st_fmt);
	if (SUCCESS > retVal) {
		TEST_PRINT_ERR("S_FMT Ioctl Failed");
		status = FAILURE;
		goto exit2;
	}
	TEST_PRINT_TRC("S_FMT Ioctl Passed");

	/* get format */
	testoptions->st_fmt.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	retVal = st_v4l2_display_get_format(testoptions->st_dev,
			&testoptions->st_fmt);
	if (SUCCESS > retVal) {
		TEST_PRINT_ERR("G_FMT Ioctl Failed");
		status = FAILURE;
		goto exit2;
	}
	TEST_PRINT_TRC("G_FMT Ioctl Passed");

	TEST_PRINT_TRC("New values of format as below");

	TEST_PRINT_TRC("Width of image :%d",
			testoptions->st_fmt.fmt.pix.width);
	TEST_PRINT_TRC("Height of image:%d",
			testoptions->st_fmt.fmt.pix.height);
	TEST_PRINT_TRC("Size of image:%d",
			testoptions->st_fmt.fmt.pix.sizeimage);
	TEST_PRINT_TRC("Bytesperline:%d",
			testoptions->st_fmt.fmt.pix.bytesperline);

	if (testoptions->crop_width != 0
			|| testoptions->crop_height != 0
			|| testoptions->crop_top != 0
			|| testoptions->crop_left != 0) {
		testoptions->st_crop.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;

		/* Get Crop */
		retVal = st_v4l2_display_get_crop(testoptions->st_dev,
				&testoptions->st_crop);
		if (SUCCESS > retVal) {
			TEST_PRINT_ERR("GET CROP Failed");
			status = FAILURE;
			goto exit2;
		}
		TEST_PRINT_TRC("GET CROP Passed");

		testoptions->st_crop.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
		testoptions->st_crop.c.width = testoptions->crop_width;
		testoptions->st_crop.c.height = testoptions->crop_height;
		testoptions->st_crop.c.top = testoptions->crop_top;
		testoptions->st_crop.c.left = testoptions->crop_left;

		retVal = st_v4l2_display_set_crop(testoptions->st_dev,
				&testoptions->st_crop);
		if (SUCCESS > retVal) {
			TEST_PRINT_ERR("SET CROP Failed");
			status = FAILURE;
			goto exit2;
		}
		TEST_PRINT_TRC("SET CROP Passed");

		/* Get Crop */
		retVal = st_v4l2_display_get_crop(testoptions->st_dev,
				&testoptions->st_crop);
		if (SUCCESS > retVal) {
			TEST_PRINT_ERR("GET CROP Failed");
			status = FAILURE;
			goto exit2;
		}
		TEST_PRINT_TRC("GET CROP Passed");

		TEST_PRINT_TRC("The corp height of the image | %d",
				testoptions->st_crop.c.height);
		TEST_PRINT_TRC("The corp width of the image | %d",
				testoptions->st_crop.c.width);
		TEST_PRINT_TRC("The corp top of the image | %d",
				testoptions->st_crop.c.top);
		TEST_PRINT_TRC("The corp left of the image | %d",
				testoptions->st_crop.c.left);

	}

	if (testoptions->fmt_width != 0
			|| testoptions->fmt_height != 0
			|| testoptions->fmt_top != 0
			|| testoptions->fmt_left != 0) {
		/* get format */
		testoptions->st_fmt.type = V4L2_BUF_TYPE_VIDEO_OVERLAY;
		retVal = st_v4l2_display_get_format(testoptions->st_dev,
				&testoptions->st_fmt);
		if (SUCCESS > retVal) {
			TEST_PRINT_ERR("G_FMT Ioctl Failed");
			status = FAILURE;
			goto exit2;
		}
		TEST_PRINT_TRC("G_FMT Ioctl Passed");

		testoptions->st_fmt.type =
			V4L2_BUF_TYPE_VIDEO_OVERLAY;
		testoptions->st_fmt.fmt.win.w.width =
			testoptions->fmt_width;
		testoptions->st_fmt.fmt.win.w.height =
			testoptions->fmt_height;
		testoptions->st_fmt.fmt.win.w.top =
			testoptions->fmt_top;
		testoptions->st_fmt.fmt.win.w.left =
			testoptions->fmt_left;

		retVal = st_v4l2_display_set_format(testoptions->st_dev,
				&testoptions->st_fmt);
		if (SUCCESS > retVal) {
			TEST_PRINT_ERR("S_FMT Ioctl Failed");
			status = FAILURE;
			goto exit2;
		}
		TEST_PRINT_TRC("S_FMT Ioctl Passed");

		/* get format */
		testoptions->st_fmt.type = V4L2_BUF_TYPE_VIDEO_OVERLAY;
		retVal = st_v4l2_display_get_format(testoptions->st_dev,
				&testoptions->st_fmt);
		if (SUCCESS > retVal) {
			TEST_PRINT_ERR("G_FMT Ioctl Failed");
			status = FAILURE;
			goto exit2;
		}
		TEST_PRINT_TRC("G_FMT Ioctl Passed");

		TEST_PRINT_TRC("Window Width:%d",
				testoptions->st_fmt.fmt.win.w.width);
		TEST_PRINT_TRC("Window Height:%d",
				testoptions->st_fmt.fmt.win.w.height);
		TEST_PRINT_TRC("Window Top:%d",
				testoptions->st_fmt.fmt.win.w.top);
		TEST_PRINT_TRC("Window Left:%d",
				testoptions->st_fmt.fmt.win.w.left);

		/* get format */
		testoptions->st_fmt.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
		retVal = st_v4l2_display_get_format(testoptions->st_dev,
				&testoptions->st_fmt);
		if (SUCCESS > retVal) {
			TEST_PRINT_ERR("G_FMT Ioctl Failed");
			status = FAILURE;
			goto exit2;
		}
		TEST_PRINT_TRC("G_FMT Ioctl Passed");
	}

	if (testoptions->userptr == FALSE) {
		testoptions->st_reqbuf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
		testoptions->st_reqbuf.memory = V4L2_MEMORY_MMAP;

		retVal = st_v4l2_display_request_buffer_mmap
			(testoptions->st_dev, &testoptions->st_reqbuf);
		if (SUCCESS > retVal) {
			TEST_PRINT_ERR("REQBUF Ioctl Failed");
			status = FAILURE;
			goto exit2;
		}

		TEST_PRINT_TRC("REQBUF Ioctl Passed");
		TEST_PRINT_TRC("Buffer nos: %d", testoptions->st_reqbuf.count);

		/* Query buffer */
		testoptions->st_buf.type =  V4L2_BUF_TYPE_VIDEO_OUTPUT;
		testoptions->st_buf.memory = V4L2_MEMORY_MMAP;

		retVal = st_v4l2_display_query_buffer_mmap(testoptions->st_dev,
				testoptions->st_reqbuf.count,
				&testoptions->st_fmt, &testoptions->st_buf,
				testoptions->pixelformat);

		if (SUCCESS > retVal) {
			TEST_PRINT_ERR("QUERYBUF Ioctl and MMAP Failed");
			status = FAILURE;
			goto exit1;
		}
		TEST_PRINT_TRC("QUERYBUF Ioctl and MMAP Passed");

		/* Enqueue buffers */
		testoptions->st_buf.type =  V4L2_BUF_TYPE_VIDEO_OUTPUT;
		testoptions->st_buf.memory = V4L2_MEMORY_MMAP;

		retVal = st_v4l2_display_queue_buffers_mmap
			(testoptions->st_dev, testoptions->st_reqbuf.count,
			 &testoptions->st_buf);
		if (SUCCESS > retVal) {
			TEST_PRINT_ERR("Queue Failed");
			status = FAILURE;
			goto exit1;
		}
		TEST_PRINT_TRC("Queuing Buffers Passed");
	}

	else if (testoptions->userptr == TRUE) {
		TEST_PRINT_TRC("Using userpointer technique");

		/* open V4L2 display device */
		retVal = st_fbdev_display_open(testoptions->device_name_fb,
				&testoptions->st_dev_fb);
		if (SUCCESS > retVal) {
			TEST_PRINT_ERR("Failed to open framebuffer device %s",
					testoptions->device_name_fb);
			status = FAILURE;
			goto exit4;
		}
		TEST_PRINT_TRC("Framebuffer device node %s opened",
				testoptions->device_name_fb);

		testoptions->st_reqbuf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
		testoptions->st_reqbuf.memory = V4L2_MEMORY_USERPTR;

		retVal = st_v4l2_display_request_buffer_mmap
			(testoptions->st_dev, &testoptions->st_reqbuf);
		if (SUCCESS > retVal) {
			TEST_PRINT_ERR("REQBUF Ioctl Failed");
			status = FAILURE;
			goto exit2;
		}
		TEST_PRINT_TRC("REQBUF Ioctl Passed");
		TEST_PRINT_TRC("Buffer nos: %d", testoptions->st_reqbuf.count);

		/* get variable screen info */
		retVal = st_fbdev_get_vscreeninfo_interface
			(testoptions->st_dev_fb, &testoptions->st_var);
		if (retVal) {
			status = FAILURE;
			goto exit4;
		}
		TEST_PRINT_TRC("FBIOGET_VSCREENINFO Ioctl passed");

		/* get fix screen info */
		retVal = st_fbdev_get_fscreeninfo_interface
			(testoptions->st_dev_fb, &testoptions->st_fix);
		if (retVal) {
			status = FAILURE;
			goto exit4;
		}
		TEST_PRINT_TRC("FBIOGET_FSCREENINFO Ioctl Passed");

		buffer_size = testoptions->st_fix.line_length *
			testoptions->st_var.yres;
		TEST_PRINT_TRC("Buffer size = %d, X - %d, Y - %d",
				buffer_size,
				testoptions->st_fix.line_length,
				testoptions->st_var.yres);

		retVal = st_v4l2_display_buffer_userptr
			(testoptions->st_dev_fb, buffer_size);
		if (retVal) {
			TEST_PRINT_TRC("User Pointer maping buffer failed");
			status = FAILURE;
			goto exit5;
		}
		TEST_PRINT_TRC("User Pointer maping buffer Passed");

		/* Enqueue buffers */
		testoptions->st_buf.type =  V4L2_BUF_TYPE_VIDEO_OUTPUT;
		testoptions->st_buf.memory = V4L2_MEMORY_USERPTR;
		testoptions->st_buf.length = buffer_size;

		retVal = st_v4l2_display_queue_buffers_userptr
			(testoptions->st_dev, testoptions->st_reqbuf.count,
			 &testoptions->st_buf);
		if (SUCCESS > retVal) {
			TEST_PRINT_ERR("Queue Failed");
			status = FAILURE;
			goto exit5;
		}
		TEST_PRINT_TRC("Queuing Buffers Passed");
	}

	/* If CPU load requested */
	if (testoptions->cpuload == TRUE)
		start_cpuload_measurement(&cpuload);

	if (testoptions->framerate == TRUE)
		gettimeofday(&before, NULL);


	/* Stream on */
	retVal = st_v4l2_display_streamon(testoptions->st_dev);
	if (SUCCESS > retVal) {
		TEST_PRINT_ERR("STREAM ON Failed");
		status = FAILURE;
		goto exit1;
	}

	TEST_PRINT_TRC("Streaming on for frames | %d",
			testoptions->loop);
	TEST_PRINT_TRC("Display frame");

	if (testoptions->userptr == FALSE) {
		/* Display frames */
		testoptions->st_buf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
		testoptions->st_buf.memory = V4L2_MEMORY_MMAP;
		retVal = st_v4l2_display_color_bar
			(testoptions->st_dev, testoptions->loop,
			 &testoptions->st_fmt, &testoptions->st_buf,
			 testoptions->pixelformat);
		if (SUCCESS > retVal) {
			TEST_PRINT_ERR("Display Failed");
			status = FAILURE;
			goto exit1;
		}
		TEST_PRINT_TRC("Display Successful");
	}

	if (testoptions->userptr == TRUE) {
		/* Display frames */
		testoptions->st_buf.type =  V4L2_BUF_TYPE_VIDEO_OUTPUT;
		testoptions->st_buf.memory = V4L2_MEMORY_USERPTR;
		testoptions->st_buf.length = buffer_size;
		retVal = st_v4l2_display_color_bar_userptr
			(testoptions->st_dev, testoptions->loop,
			 &testoptions->st_fmt, &testoptions->st_buf,
			 buffer_size, testoptions->pixelformat);
		if (SUCCESS > retVal) {
			TEST_PRINT_ERR("Display Failed");
			status = FAILURE;
			goto exit5;
		}
		TEST_PRINT_TRC("Display Successful");
	}

	

	/* Stream off */
	retVal = st_v4l2_display_streamoff(testoptions->st_dev);
	if (SUCCESS > retVal) {
		TEST_PRINT_ERR("STREAM OFF Failed");
		status = FAILURE;
		goto exit1;
	}
	TEST_PRINT_TRC("Streaming off.....Stopping display");

	if (testoptions->framerate == TRUE) {
		gettimeofday(&after, NULL);

		calc_result_time(&result, &after, &before);
		TEST_PRINT_TRC("The V4L2 Display : Frame rate = %lu",
					testoptions->loop/result.tv_sec);
	}


	/* If CPU load requested */
	if(testoptions->cpuload == TRUE) {
		cpu_load = stop_cpuload_measurement(&cpuload);
		TEST_PRINT_TRC("The V4L2 Display : Percentage CPU Load = %u%%",
					cpu_load);
	}


exit1:
	if (testoptions->userptr == FALSE) {
		/* unmap buffers if mapped */
		/* close v4l2 device */
		retVal = st_v4l2_display_unmap_mmap(
				testoptions->st_reqbuf.count);
		if (SUCCESS > retVal) {
			TEST_PRINT_ERR("Buffer unmap failed");
			status = FAILURE;
			goto exit2;
		}
		TEST_PRINT_TRC("Buffers unmapped Successfully");
	}
exit5:
	if (testoptions->userptr == TRUE) {
		/* unmap buffers if mapped */
		/* close v4l2 device */
		retVal = st_v4l2_display_unmap_userptr(buffer_size,
				testoptions->st_reqbuf.count);
		if (SUCCESS > retVal) {
			TEST_PRINT_ERR("Buffer unmap failed");
			status = FAILURE;
			goto exit2;
		}
		TEST_PRINT_TRC("Buffers unmapped Successfully");
	}
exit2:
	/* close device */
	retVal = st_v4l2_display_close(testoptions->st_dev);
	if (SUCCESS > retVal) {
		TEST_PRINT_ERR("V4L2 Device could not be closed");
		status = FAILURE;
		goto exit3;
	}
	TEST_PRINT_TRC("V4L2 device node %s closed",
			testoptions->device_name);
exit4:
	if (testoptions->userptr == TRUE) {
		/* close device */
		retVal = st_fbdev_display_close(testoptions->st_dev_fb);
		if (SUCCESS > retVal) {
			TEST_PRINT_ERR("Framebuffer Device not closed");
			status = FAILURE;
			goto exit3;
		}
		TEST_PRINT_TRC("Framebuffer device node %s closed",
				testoptions->device_name_fb);
	}
exit3:
	/* print status/result of the test case */
	TEST_PRINT_TST_RESULT(status, testoptions->testcase_id);

	/* end test case */
	TEST_PRINT_TST_END(testoptions->testcase_id);
	return status;
}

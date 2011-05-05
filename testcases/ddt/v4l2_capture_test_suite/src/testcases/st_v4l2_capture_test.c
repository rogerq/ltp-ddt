/*
 * st_v4l2_capture_test.c
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

/* function prototypes for interface functions are defined here*/
#include <st_v4l2_capture_common.h>
 
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
int st_v4l2_capture_test(struct v4l2_capture_testparams *testoptions)
{
	int retVal = SUCCESS;
	int status = SUCCESS;
	int video2_numbuffers = 6, video2_fd = 0;
	unsigned int cpu_load;
	struct proc_stat cpuload;
	struct timeval before, after, result;

	TEST_PRINT_TST_START(testoptions->testcase_id);

	/* open V4L2 capture device */
	retVal = st_v4l2_capture_open(testoptions->device_name_cap,
			&testoptions->st_dev_cap);
	if (SUCCESS > retVal) {
		TEST_PRINT_ERR("Failed to open V4L2 capture device %s",
				testoptions->device_name_cap);
		status = FAILURE;
		goto exit5;
	}
	TEST_PRINT_TRC("V4L2 capture device node %s opened",
			testoptions->device_name_cap);

	/* get input */
	retVal = st_v4l2_capture_get_input_interface(testoptions->st_dev_cap,
			&testoptions->st_input);
	if (SUCCESS > retVal) {
		TEST_PRINT_ERR("G_INPUT Ioctl Failed");
		status = FAILURE;
		goto exit4;
	}
	TEST_PRINT_TRC("G_INPUT Ioctl Passed");
	TEST_PRINT_TRC("Index is %d", testoptions->st_input.index);

	/* enum input */
	retVal = st_v4l2_capture_enum_input_interface(testoptions->st_dev_cap,
			&testoptions->st_input);
	if (SUCCESS > retVal) {
		TEST_PRINT_ERR("ENUM_INPUT Ioctl Failed");
		status = FAILURE;
		goto exit4;
	}
	TEST_PRINT_TRC("ENUM_INPUT Ioctl Passed");
	TEST_PRINT_TRC("Current input is: %s", testoptions->st_input.name);

	/* set input */
	retVal = st_v4l2_capture_set_input_interface(testoptions->st_dev_cap,
			&testoptions->st_input);
	if (SUCCESS > retVal) {
		TEST_PRINT_ERR("S_INPUT Ioctl Failed");
		status = FAILURE;
		goto exit4;
	}
	TEST_PRINT_TRC("S_INPUT Ioctl Passed");

	/* query standard */
	retVal = st_v4l2_capture_querystd_interface(testoptions->st_dev_cap,
			&testoptions->st_std_id);
	if (SUCCESS > retVal) {
		TEST_PRINT_ERR("QUERY_STD Ioctl Failed");
		status = FAILURE;
		goto exit4;
	}
	TEST_PRINT_TRC("QUERY_STD Ioctl Passed");

	/* get standard */
	retVal = st_v4l2_capture_get_std_interface(testoptions->st_dev_cap,
			&testoptions->st_std_id);
	if (SUCCESS > retVal) {
		TEST_PRINT_ERR("G_STD Ioctl Failed");
		status = FAILURE;
		goto exit4;
	}
	TEST_PRINT_TRC("G_STD Ioctl Passed");

	/* enum standard */
	retVal = st_v4l2_capture_enum_std_interface(testoptions->st_dev_cap,
			&testoptions->st_standard, testoptions->st_std_id);
	if (SUCCESS > retVal) {
		TEST_PRINT_ERR("ENUM_STD Ioctl Failed");
		status = FAILURE;
		goto exit4;
	}
	TEST_PRINT_TRC("ENUM_STD Ioctl Passed");
	TEST_PRINT_TRC("Current standard is %s", testoptions->st_standard.name);

	/* query cap */
	retVal = st_v4l2_display_querycap(testoptions->st_dev_cap,
			&testoptions->st_capability);
	if (SUCCESS > retVal) {
		TEST_PRINT_ERR("QUERY_CAPABILITIES Ioctl Failed");
		status = FAILURE;
		goto exit4;
	}
	TEST_PRINT_TRC("QUERY_CAPABILITIES Ioctl Passed");

	/* get format */
	testoptions->st_fmt_cap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	retVal = st_v4l2_display_get_format(testoptions->st_dev_cap,
			&testoptions->st_fmt_cap);
	if (SUCCESS > retVal) {
		TEST_PRINT_ERR("G_FMT Ioctl Failed");
		status = FAILURE;
		goto exit4;
	}
	TEST_PRINT_TRC("G_FMT Ioctl Passed");

	TEST_PRINT_TRC("Initial values of format are below");

	TEST_PRINT_TRC("Width of capture image:%d",
			testoptions->st_fmt_cap.fmt.pix.width);
	TEST_PRINT_TRC("Height of capture image:%d",
			testoptions->st_fmt_cap.fmt.pix.height);
	TEST_PRINT_TRC("Size of capture image:%d",
			testoptions->st_fmt_cap.fmt.pix.sizeimage);
	TEST_PRINT_TRC("Bytesperline for capture:%d",
			testoptions->st_fmt_cap.fmt.pix.bytesperline);

	/* set format */
	testoptions->st_fmt_cap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if (strcmp((char *)testoptions->st_standard.name, "PAL") == 0) {
		testoptions->st_fmt_cap.fmt.pix.width = 720;
		testoptions->st_fmt_cap.fmt.pix.height = 576;
	} else if (strcmp((char *)testoptions->st_standard.name, "NTSC")
			== 0) {
		testoptions->st_fmt_cap.fmt.pix.width = 720;
		testoptions->st_fmt_cap.fmt.pix.height = 480;
	}
	testoptions->st_fmt_cap.fmt.pix.pixelformat
		= chk_pixel_format(testoptions->pixelformat);

	retVal = st_v4l2_display_set_format(testoptions->st_dev_cap,
			&testoptions->st_fmt_cap);
	if (SUCCESS > retVal) {
		TEST_PRINT_ERR("S_FMT Ioctl Failed");
		status = FAILURE;
		goto exit4;
	}
	TEST_PRINT_TRC("S_FMT Ioctl Passed");

	/* get format */
	testoptions->st_fmt_cap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	retVal = st_v4l2_display_get_format(testoptions->st_dev_cap,
			&testoptions->st_fmt_cap);
	if (SUCCESS > retVal) {
		TEST_PRINT_ERR("G_FMT Ioctl Failed");
		status = FAILURE;
		goto exit4;
	}
	TEST_PRINT_TRC("G_FMT Ioctl Passed");

	TEST_PRINT_TRC("New values of format are below");

	TEST_PRINT_TRC("Width of capture image:%d",
			testoptions->st_fmt_cap.fmt.pix.width);
	TEST_PRINT_TRC("Height of capture image:%d",
			testoptions->st_fmt_cap.fmt.pix.height);
	TEST_PRINT_TRC("Size of capture image:%d",
			testoptions->st_fmt_cap.fmt.pix.sizeimage);
	TEST_PRINT_TRC("Bytesperline for capture:%d",
			testoptions->st_fmt_cap.fmt.pix.bytesperline);


	if (testoptions->userptr == TRUE) {
		retVal = initVideo2Plane(&video2_fd, &video2_numbuffers,
			  (char *)testoptions->st_standard.name, &testoptions->st_fmt_disp);
		if (SUCCESS > retVal) {
			TEST_PRINT_ERR("Failed to open V4L2 capture device %s",
					testoptions->device_name_disp);
			status = FAILURE;
			goto exit5;
		}
	}

	if(testoptions->userptr != TRUE){
		testoptions->st_reqbuf_cap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		testoptions->st_reqbuf_cap.memory = V4L2_MEMORY_MMAP;

		retVal = st_v4l2_display_request_buffer_mmap
			(testoptions->st_dev_cap, &testoptions->st_reqbuf_cap);
		if (SUCCESS > retVal) {
			TEST_PRINT_ERR("REQBUF Ioctl Failed");
			status = FAILURE;
			goto exit4;
		}
		TEST_PRINT_TRC("REQBUF Ioctl Passed");
		TEST_PRINT_TRC("No of buffers:%d", testoptions->st_reqbuf_cap.count);
	}

	if(testoptions->userptr == TRUE){
		testoptions->st_reqbuf_cap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		testoptions->st_reqbuf_cap.memory = V4L2_MEMORY_USERPTR;

		retVal = st_v4l2_display_request_buffer_mmap
			(testoptions->st_dev_cap, &testoptions->st_reqbuf_cap);
		if (SUCCESS > retVal) {
			TEST_PRINT_ERR("REQBUF Ioctl Failed");
			status = FAILURE;
			goto exit4;
		}
		TEST_PRINT_TRC("USERPTR REQBUF Ioctl Passed");
		TEST_PRINT_TRC("No of buffers:%d", testoptions->st_reqbuf_cap.count);
	}
	
	clean_buffers();

	if(testoptions->userptr != TRUE){
		/* Query buffer */
		testoptions->st_buf_cap.type =  V4L2_BUF_TYPE_VIDEO_CAPTURE;
		testoptions->st_buf_cap.memory = V4L2_MEMORY_MMAP;

		retVal = st_v4l2_display_query_buffer_mmap_cap(testoptions->st_dev_cap,
				testoptions->st_reqbuf_cap.count,
				&testoptions->st_fmt_cap, &testoptions->st_buf_cap,
				testoptions->pixelformat);
		if (SUCCESS > retVal) {
			TEST_PRINT_ERR("QUERYBUF Ioctl and MMAP Failed");
			status = FAILURE;
			goto exit3;
		}
		TEST_PRINT_TRC("QUERYBUF Ioctl and MMAP Passed");
	}

	if(testoptions->userptr == TRUE){
		/* Query buffer */
		testoptions->st_buf_cap.type =  V4L2_BUF_TYPE_VIDEO_CAPTURE;
		testoptions->st_buf_cap.memory = V4L2_MEMORY_USERPTR;

		retVal = st_v4l2_display_query_buffer_userptr_cap(testoptions->st_dev_cap,
				testoptions->st_reqbuf_cap.count,
				&testoptions->st_fmt_cap, &testoptions->st_buf_cap,
				testoptions->pixelformat);

		if (SUCCESS > retVal) {
			TEST_PRINT_ERR("Buffer Maping Failed");
			status = FAILURE;
			goto exit3;
		}
		TEST_PRINT_TRC("Buffer Maping Passed");
	}

	/* open V4L2 display device */
	retVal = st_v4l2_display_open(testoptions->device_name_disp,
			&testoptions->st_dev_disp);
	if (SUCCESS > retVal) {
		TEST_PRINT_ERR("Failed to open V4L2 device %s",
				testoptions->device_name_disp);
		status = FAILURE;
		goto exit3;
	}
	TEST_PRINT_TRC("V4L2 device node %s opened",
			testoptions->device_name_disp);

	/* get format */
	testoptions->st_fmt_disp.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	retVal = st_v4l2_display_get_format(testoptions->st_dev_disp,
			&testoptions->st_fmt_disp);
	if (SUCCESS > retVal) {
		TEST_PRINT_ERR("G_FMT Ioctl Failed");
		status = FAILURE;
		goto exit2;
	}
	TEST_PRINT_TRC("G_FMT Ioctl Passed");

	TEST_PRINT_TRC("Initial values of format are below");

	TEST_PRINT_TRC("Width of display image :%d",
			testoptions->st_fmt_disp.fmt.pix.width);
	TEST_PRINT_TRC("Height of display image:%d",
			testoptions->st_fmt_disp.fmt.pix.height);
	TEST_PRINT_TRC("Size of display image:%d",
			testoptions->st_fmt_disp.fmt.pix.sizeimage);
	TEST_PRINT_TRC("Bytesperline for display:%d",
			testoptions->st_fmt_disp.fmt.pix.bytesperline)

		testoptions->st_control.id = V4L2_CID_ROTATE;
	testoptions->st_control.value = 90;
	/* set control */
	retVal = st_v4l2_set_control(testoptions->st_dev_disp,
			&testoptions->st_control);
	if (SUCCESS > retVal) {
		TEST_PRINT_ERR("Set Control Failed");
		status = FAILURE;
		goto exit2;
	}
	TEST_PRINT_TRC("Set Control Passed");

	testoptions->st_fmt_disp.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	if (strcmp((char *)testoptions->st_standard.name, "PAL") == 0) {
		testoptions->st_fmt_disp.fmt.pix.width = 720;
		testoptions->st_fmt_disp.fmt.pix.height = 576;
	} else if (strcmp((char *)testoptions->st_standard.name, "NTSC")
			== 0) {
		testoptions->st_fmt_disp.fmt.pix.width = 720;
		testoptions->st_fmt_disp.fmt.pix.height = 480;
	}
	testoptions->st_fmt_disp.fmt.pix.pixelformat
		= chk_pixel_format(testoptions->pixelformat);

	retVal = st_v4l2_display_set_format(testoptions->st_dev_disp,
			&testoptions->st_fmt_disp);
	if (SUCCESS > retVal) {
		TEST_PRINT_ERR("S_FMT Ioctl Failed");
		status = FAILURE;
		goto exit2;
	}
	TEST_PRINT_TRC("S_FMT Ioctl Passed");

	/* get format */
	testoptions->st_fmt_disp.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	retVal = st_v4l2_display_get_format(testoptions->st_dev_disp,
			&testoptions->st_fmt_disp);
	if (SUCCESS > retVal) {
		TEST_PRINT_ERR("G_FMT Ioctl Failed");
		status = FAILURE;
		goto exit2;
	}
	TEST_PRINT_TRC("G_FMT Ioctl Passed");

	TEST_PRINT_TRC("New values of format are below");

	TEST_PRINT_TRC("Width of display image :%d",
			testoptions->st_fmt_disp.fmt.pix.width);
	TEST_PRINT_TRC("Height of display image:%d",
			testoptions->st_fmt_disp.fmt.pix.height);
	TEST_PRINT_TRC("Size of display image:%d",
			testoptions->st_fmt_disp.fmt.pix.sizeimage);
	TEST_PRINT_TRC("Bytesperline for display:%d",
			testoptions->st_fmt_disp.fmt.pix.bytesperline)

	testoptions->st_reqbuf_disp.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	testoptions->st_reqbuf_disp.memory = V4L2_MEMORY_MMAP;
	testoptions->st_reqbuf_disp.count = 6;

	retVal = st_v4l2_display_request_buffer_mmap
		(testoptions->st_dev_disp, &testoptions->st_reqbuf_disp);
	if (SUCCESS > retVal) {
		TEST_PRINT_ERR("REQBUF Ioctl Failed");
		status = FAILURE;
		goto exit2;
	}
	TEST_PRINT_TRC("REQBUF Ioctl Passed");
	TEST_PRINT_TRC("No of buffers:%d", testoptions->st_reqbuf_disp.count);

	/* Query buffer */
	testoptions->st_buf_disp.type =  V4L2_BUF_TYPE_VIDEO_OUTPUT;
	testoptions->st_buf_disp.memory = V4L2_MEMORY_MMAP;

	retVal = st_v4l2_display_query_buffer_mmap_disp
		(testoptions->st_dev_disp,
		 testoptions->st_reqbuf_disp.count,
		 &testoptions->st_fmt_disp, &testoptions->st_buf_disp,
		 testoptions->pixelformat);

	if (SUCCESS > retVal) {
		TEST_PRINT_ERR("QUERYBUF Ioctl and MMAP Failed");
		status = FAILURE;
		goto exit1;
	}
	TEST_PRINT_TRC("QUERYBUF Ioctl and MMAP Passed");

	/* If CPU load requested */
	if (testoptions->cpuload == TRUE)
		start_cpuload_measurement(&cpuload);

	if (testoptions->framerate == TRUE)
		gettimeofday(&before, NULL);

	/* Stream on */
	retVal = st_v4l2_display_streamon(testoptions->st_dev_disp);
	if (SUCCESS > retVal) {
		TEST_PRINT_ERR("STREAM ON display failed");
		status = FAILURE;
		goto exit1;
	}
	TEST_PRINT_TRC("STREAM ON display passed");

	retVal = st_v4l2_capture_streamon(testoptions->st_dev_cap);
	if (SUCCESS > retVal) {
		TEST_PRINT_ERR("STREAM ON capture failed");
		status = FAILURE;
		goto exit1;
	}
	TEST_PRINT_TRC("STREAM ON capture passed");

	TEST_PRINT_TRC("Streaming on for frames | %d",
			testoptions->loop);
	TEST_PRINT_TRC("Display frame");

	testoptions->st_buf_cap.type =  V4L2_BUF_TYPE_VIDEO_CAPTURE;
	testoptions->st_buf_cap.memory = V4L2_MEMORY_MMAP;
	testoptions->st_buf_cap.index = 0;

	testoptions->st_buf_disp.type =  V4L2_BUF_TYPE_VIDEO_OUTPUT;
	testoptions->st_buf_disp.memory = V4L2_MEMORY_MMAP;
	testoptions->st_buf_disp.index = 0;

	retVal = st_v4l2_capture_display_interface(&testoptions->st_buf_cap,
			&testoptions->st_buf_disp, testoptions->loop,
			testoptions->st_dev_cap, testoptions->st_dev_disp,
			&testoptions->st_fmt_cap,
			&testoptions->st_fmt_disp); 
	if (SUCCESS > retVal) {
		TEST_PRINT_ERR("Q/DQ BUF failed");
		status = FAILURE;
		goto exit1;
	}
	TEST_PRINT_TRC("Q/DQ BUF passed");

	/* Stream off */
	retVal = st_v4l2_capture_streamoff(testoptions->st_dev_cap);
	if (SUCCESS > retVal) {
		TEST_PRINT_ERR("Capture STREAM OFF Failed");
		status = FAILURE;
		goto exit1;
	}
	TEST_PRINT_TRC("Streaming off.....Stopping capture");

	/* Stream off */
	retVal = st_v4l2_display_streamoff(testoptions->st_dev_disp);
	if (SUCCESS > retVal) {
		TEST_PRINT_ERR("Display STREAM OFF Failed");
		status = FAILURE;
		goto exit1;
	}
	TEST_PRINT_TRC("Streaming off.....Stopping display");

	if (testoptions->framerate == TRUE) {
		gettimeofday(&after, NULL);

		calc_result_time(&result, &after, &before);
		TEST_PRINT_TRC("Frame rate = %lu",
					testoptions->loop/result.tv_sec);
	}

	/* If CPU load requested */
	if(testoptions->cpuload == TRUE) {
		cpu_load = stop_cpuload_measurement(&cpuload);
		TEST_PRINT_TRC("Percentage CPU Load = %u%%",
					cpu_load);
	}


exit1:
	if(testoptions->userptr != TRUE) { 
		/* unmap buffers if mapped */
		retVal = st_v4l2_display_unmap_mmap_cap(
				testoptions->st_reqbuf_cap.count);
		if (SUCCESS > retVal) {
			TEST_PRINT_ERR("Buffer unmap failed");
			status = FAILURE;
			goto exit2;
		}
	}

        if(testoptions->userptr == TRUE) {
                /* unmap buffers if mapped */
                retVal = st_v4l2_display_unmap_userptr_disp(
                                testoptions->st_reqbuf_disp.count);
                if (SUCCESS > retVal) {
                        TEST_PRINT_ERR("USERPTR Buffer unmap failed");
                        status = FAILURE;
                        goto exit2;
                }
        }
        TEST_PRINT_TRC("Buffers unmapped Successfully");

exit2:
	/* close device */
	retVal = st_v4l2_display_close(testoptions->st_dev_disp);
	if (SUCCESS > retVal) {
		TEST_PRINT_ERR("V4L2 display device could not be closed");
		status = FAILURE;
		goto exit3;
	}
	TEST_PRINT_TRC("V4L2 display device node %s closed",
			testoptions->device_name_disp);
exit3:
	/* unmap buffers if mapped */
	retVal = st_v4l2_display_unmap_mmap_cap(
			testoptions->st_reqbuf_cap.count);
	if (SUCCESS > retVal) {
		TEST_PRINT_ERR("Capture buffer unmap failed");
		status = FAILURE;
		goto exit4;
	}
	TEST_PRINT_TRC("Capture buffers unmapped Successfully");
exit4:
	/* close device */
	retVal = st_v4l2_capture_close(testoptions->st_dev_cap);
	if (SUCCESS > retVal) {
		TEST_PRINT_ERR("V4L2 Capture device could not be closed");
		status = FAILURE;
		goto exit3;
	}
	TEST_PRINT_TRC("V4L2 capture device node %s closed",
			testoptions->device_name_cap);

exit5:
	/* print status/result of the test case */
	TEST_PRINT_TST_RESULT(status, testoptions->testcase_id);

	/* end test case */
	TEST_PRINT_TST_END(testoptions->testcase_id);
	return status;
}

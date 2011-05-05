/*
 * st_v4l2_capture_ioctl_test.c
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

/* Function to display image of given
*  width*height on the V4L2 supported windows
*/
int st_v4l2_capture_ioctl_test(struct v4l2_capture_testparams *testoptions)
{
	int retVal = SUCCESS;
	int status = SUCCESS;
	unsigned int cpu_load;
	struct proc_stat cpuload;


	TEST_PRINT_TST_START(testoptions->testcase_id);

	/* open V4L2 capture device */
	retVal = st_v4l2_capture_open(testoptions->device_name_cap,
			&testoptions->st_dev_cap);
	if (SUCCESS > retVal) {
		TEST_PRINT_ERR("Failed to open V4L2 capture device %s",
				testoptions->device_name_cap);
		status = FAILURE;
		goto exit3;
	}
	TEST_PRINT_TRC("V4L2 capture device node %s opened",
			testoptions->device_name_cap);

	switch (testoptions->ioctl_no) {
	case QUERY_CAP:
		/* QUERY_CAPABILITIES */
		retVal = st_v4l2_display_querycap(testoptions->st_dev_cap,
			&testoptions->st_capability);
		if (SUCCESS > retVal) {
			TEST_PRINT_ERR("QUERY_CAPABILITIES Ioctl Failed");
			status = FAILURE;
			goto exit2;
		}
		TEST_PRINT_TRC("QUERY_CAPABILITIES Ioctl Passed");
		goto exit2;
		break;
	case GET_INPUT:
		/* get input */
		retVal = st_v4l2_capture_get_input_interface
				(testoptions->st_dev_cap,
					 &testoptions->st_input);
		if (SUCCESS > retVal) {
			TEST_PRINT_ERR("G_INPUT Ioctl Failed");
			status = FAILURE;
			goto exit2;
		}
		TEST_PRINT_TRC("G_INPUT Ioctl Passed");
		TEST_PRINT_TRC("Index is %d", testoptions->st_input.index);
		goto exit2;
		break;
	case ENUM_INPUT:		
		/* enum input */
		retVal = st_v4l2_capture_enum_input_interface
				(testoptions->st_dev_cap,
                                	&testoptions->st_input);
		if (SUCCESS > retVal) {
			TEST_PRINT_ERR("ENUM_INPUT Ioctl Failed");
			status = FAILURE;
			goto exit2;
		}
		TEST_PRINT_TRC("ENUM_INPUT Ioctl Passed");
		TEST_PRINT_TRC("Current input is: %s",
				testoptions->st_input.name);
		goto exit2;
		break;
	case SET_INPUT:
		/* set input */
		retVal = st_v4l2_capture_set_input_interface
				(testoptions->st_dev_cap,
                                	&testoptions->st_input);
		if (SUCCESS > retVal) {
			TEST_PRINT_ERR("S_INPUT Ioctl Failed");
			status = FAILURE;
			goto exit2;
		}
		TEST_PRINT_TRC("S_INPUT Ioctl Passed");
		goto exit2;
		break;
	case SET_CRTL:
		/* set control */

		retVal = st_v4l2_query_crtl_interface(testoptions->st_dev_cap,
							&testoptions->st_control);
		if (SUCCESS > retVal) {
			TEST_PRINT_ERR("QUERY_CTRL Ioctl Failed");
			status = FAILURE;
			goto exit2;
		}


		retVal = st_v4l2_set_crtl_interface(testoptions->st_dev_cap,
							&testoptions->st_control);
			if (SUCCESS > retVal) {
				TEST_PRINT_ERR("S_CRTL Ioctl Failed");
				status = FAILURE;
				goto exit2;
			}
		TEST_PRINT_TRC("S_CTRL Ioctl Passed");
		goto exit2;
		break;
	case GET_CRTL:

		retVal = st_v4l2_query_crtl_interface(testoptions->st_dev_cap,
							&testoptions->st_control);
		if (SUCCESS > retVal) {
			TEST_PRINT_ERR("QUERY_CTRL Ioctl Failed");
			status = FAILURE;
			goto exit2;
		}

		/* get control */

		retVal = st_v4l2_get_crtl_interface(testoptions->st_dev_cap,
							&testoptions->st_control);
			if (SUCCESS > retVal) {
				TEST_PRINT_ERR("G_CRTL Ioctl Failed");
				status = FAILURE;
				goto exit2;
			}
		TEST_PRINT_TRC("G_CTRL Ioctl Passed");
		goto exit2;
		break;
	case GET_STD:
		/* get the standard */
		retVal = st_v4l2_capture_get_std_interface
				(testoptions->st_dev_cap,
                               		&testoptions->st_std_id);
		if (SUCCESS > retVal) {
			TEST_PRINT_ERR("G_STD Ioctl Failed");
			status = FAILURE;
			goto exit2;
		}
		TEST_PRINT_TRC("G_STD Ioctl Passed");
		goto exit2;
		break;
	case ENUM_STD: 
		/* get the standard */
		retVal = st_v4l2_capture_get_std_interface
				(testoptions->st_dev_cap,
                                	&testoptions->st_std_id);
		if (SUCCESS > retVal) {
			TEST_PRINT_ERR("G_STD Ioctl Failed");
			status = FAILURE;
			goto exit2;
		}
		TEST_PRINT_TRC("G_STD Ioctl Passed");

		/* enum standard */
		retVal = st_v4l2_capture_enum_std_interface
				(testoptions->st_dev_cap,
                                	&testoptions->st_standard,
						testoptions->st_std_id);
		if (SUCCESS > retVal) {
			TEST_PRINT_ERR("QUERYSTD Ioctl Failed");
			status = FAILURE;
			goto exit2;
		}
		TEST_PRINT_TRC("QUERYSTD Ioctl Passed");
		TEST_PRINT_TRC("Current standard is %s",
					testoptions->st_standard.name);
		goto exit2;
		break;
	case GET_FMT:
		/* get format */
		testoptions->st_fmt_cap.type = testoptions->buffer_type;
		retVal = st_v4l2_display_get_format(testoptions->st_dev_cap,
			&testoptions->st_fmt_cap);
		if (SUCCESS > retVal) {
			TEST_PRINT_ERR("G_FMT Ioctl Failed");
			status = FAILURE;
			goto exit2;
		}

		TEST_PRINT_TRC("Width of image :%d",
			 testoptions->st_fmt_cap.fmt.pix.width);
		TEST_PRINT_TRC("Height of image:%d",
			 testoptions->st_fmt_cap.fmt.pix.height);
		TEST_PRINT_TRC("Pixel format:%d",
			 testoptions->st_fmt_cap.fmt.pix.pixelformat);
		TEST_PRINT_TRC("Size of image:%d",
			 testoptions->st_fmt_cap.fmt.pix.sizeimage);
		TEST_PRINT_TRC("Bytesperline:%d",
			 testoptions->st_fmt_cap.fmt.pix.bytesperline);

		TEST_PRINT_TRC("G_FMT Ioctl Passed");
		goto exit2;
		break;
	case SET_FMT:
		/* get format */
		testoptions->st_fmt_cap.type = testoptions->buffer_type;
		retVal = st_v4l2_display_get_format(testoptions->st_dev_cap,
			&testoptions->st_fmt_cap);
		if (SUCCESS > retVal) {
			TEST_PRINT_ERR("G_FMT Ioctl Failed");
			status = FAILURE;
			goto exit2;
		}
		TEST_PRINT_TRC("G_FMT Ioctl Passed");
		testoptions->st_fmt_cap.type = testoptions->buffer_type;
		if (testoptions->width != 0 || testoptions->height != 0
				|| testoptions->pixelformat != 0) {
			testoptions->st_fmt_cap.fmt.pix.width
				= testoptions->width;
			testoptions->st_fmt_cap.fmt.pix.height
				= testoptions->height;
		}

		retVal = st_v4l2_display_set_format(testoptions->st_dev_cap,
			&testoptions->st_fmt_cap);
		if (SUCCESS > retVal) {
			TEST_PRINT_ERR("S_FMT Ioctl Failed");
			status = FAILURE;
			goto exit2;
		}
		TEST_PRINT_TRC("S_FMT Ioctl Passed");
		goto exit2;
		break;
	case REQBUF:
		/*REQBUF*/
		testoptions->st_reqbuf_cap.type = testoptions->buffer_type;
		testoptions->st_reqbuf_cap.memory = V4L2_MEMORY_MMAP;

		retVal = st_v4l2_display_request_buffer_mmap
			(testoptions->st_dev_cap, &testoptions->st_reqbuf_cap);
		if (SUCCESS > retVal) {
			TEST_PRINT_ERR("REQBUF Ioctl Failed");
			status = FAILURE;
			goto exit2;
		}
		TEST_PRINT_TRC("REQBUF ioctl passed");
		TEST_PRINT_TRC("No of buffers:%d",
					 testoptions->st_reqbuf_cap.count);
		goto exit2;
		break;
	case QUERYBUF:
		/*REQBUF*/
		testoptions->st_reqbuf_cap.type = testoptions->buffer_type;
		testoptions->st_reqbuf_cap.memory = V4L2_MEMORY_MMAP;

		retVal = st_v4l2_display_request_buffer_mmap
			 (testoptions->st_dev_cap, &testoptions->st_reqbuf_cap);
		if (SUCCESS > retVal) {
			TEST_PRINT_ERR("REQBUF Ioctl Failed");
			status = FAILURE;
			goto exit2;
		}

		/* Query buffer */
		testoptions->st_buf_cap.type =  testoptions->buffer_type;
		testoptions->st_buf_cap.memory = V4L2_MEMORY_MMAP;

		retVal = st_v4l2_display_query_buffer_mmap_cap
				(testoptions->st_dev_cap,
			 	testoptions->st_reqbuf_cap.count,
	 			&testoptions->st_fmt_cap,
				&testoptions->st_buf_cap,
				testoptions->pixelformat);
		if (SUCCESS > retVal) {
			TEST_PRINT_ERR("QUERYBUF Ioctl and MMAP Failed");
			status = FAILURE;
			goto exit1;
		}
		TEST_PRINT_TRC("QUERYBUF Ioctl and MMAP Passed");
		break;
	case ENQBUF:
		/*REQBUF*/
		testoptions->st_reqbuf_cap.type = testoptions->buffer_type;
		testoptions->st_reqbuf_cap.memory = V4L2_MEMORY_MMAP;

		retVal = st_v4l2_display_request_buffer_mmap
				(testoptions->st_dev_cap,
					&testoptions->st_reqbuf_cap);
		if (SUCCESS > retVal) {
			TEST_PRINT_ERR("REQBUF Ioctl Failed");
			status = FAILURE;
			goto exit1;
		}

		/* Query buffer */
		testoptions->st_buf_cap.type =  testoptions->buffer_type;
		testoptions->st_buf_cap.memory = V4L2_MEMORY_MMAP;

		retVal = st_v4l2_display_query_buffer_mmap_cap
				(testoptions->st_dev_cap,
				testoptions->st_reqbuf_cap.count,
				&testoptions->st_fmt_cap,
				&testoptions->st_buf_cap,
				testoptions->pixelformat);
		if (SUCCESS > retVal) {
			TEST_PRINT_ERR("QUERYBUF Ioctl and MMAP Failed");
			status = FAILURE;
			goto exit1;
		}
		TEST_PRINT_TRC("QUERYBUF Ioctl and MMAP Passed");
		TEST_PRINT_TRC("Enqueuing Buffers Passed");
		break;
	case STREAM_ON_OFF:
		/*REQBUF*/
		testoptions->st_reqbuf_cap.type = testoptions->buffer_type;
		testoptions->st_reqbuf_cap.memory = V4L2_MEMORY_MMAP;

		retVal = st_v4l2_display_request_buffer_mmap
			(testoptions->st_dev_cap, &testoptions->st_reqbuf_cap);
		if (SUCCESS > retVal) {
			TEST_PRINT_ERR("REQBUF Ioctl Failed");
			status = FAILURE;
			goto exit1;
		}

		/* Query buffer */
		testoptions->st_buf_cap.type =  testoptions->buffer_type;
		testoptions->st_buf_cap.memory = V4L2_MEMORY_MMAP;

		retVal = st_v4l2_display_query_buffer_mmap_cap
				(testoptions->st_dev_cap,
				testoptions->st_reqbuf_cap.count,
				&testoptions->st_fmt_cap,
				&testoptions->st_buf_cap,
				testoptions->pixelformat);
		if (SUCCESS > retVal) {
			TEST_PRINT_ERR("QUERYBUF Ioctl and MMAP Failed");
			status = FAILURE;
			goto exit1;
		}
		TEST_PRINT_TRC("QUERYBUF Ioctl and MMAP Passed");
		
		/* If CPU load requested */
		if (testoptions->cpuload == TRUE){
			TEST_PRINT_TRC("Calculating CPULOAD");
			start_cpuload_measurement(&cpuload);
		}

		/* Stream on */
		retVal = st_v4l2_capture_streamon(testoptions->st_dev_cap);
		if (SUCCESS > retVal) {
			TEST_PRINT_ERR("STREAM ON Failed");
			status = FAILURE;
			goto exit1;
		}
		TEST_PRINT_TRC("Streaming on for frames | %d",
			testoptions->loop);

		/* Stream off */
		retVal = st_v4l2_capture_streamoff(testoptions->st_dev_cap);
		if (SUCCESS > retVal) {
			TEST_PRINT_ERR("STREAM OFF Failed");
			status = FAILURE;
			goto exit1;
		}
		TEST_PRINT_TRC("Streaming off.....Stopping capture");

		if(testoptions->cpuload){
			cpu_load = stop_cpuload_measurement(&cpuload);
			TEST_PRINT_TRC("The V4L2 Capture : Percentage CPU Load = %u%%",
				cpu_load);
		}

		break;
	}
exit1:
	/* unmap buffers if mapped */
	retVal = st_v4l2_display_unmap_mmap_cap(
		testoptions->st_reqbuf_cap.count);
	if (SUCCESS > retVal) {
		TEST_PRINT_ERR("Buffer unmap failed");
		status = FAILURE;
		goto exit2;
	}
	TEST_PRINT_TRC("Buffers unmapped Successfully");

exit2:
	/* close device */
	retVal = st_v4l2_capture_close(testoptions->st_dev_cap);
	if (SUCCESS > retVal) {
		TEST_PRINT_ERR("V4L2 Device could not be closed");
		status = FAILURE;
		goto exit3;
	}
	TEST_PRINT_TRC("V4L2 device node %s closed",
			testoptions->device_name_cap);

exit3:
	/* print status/result of the test case */
	TEST_PRINT_TST_RESULT(status, testoptions->testcase_id);

	/* end test case */
	TEST_PRINT_TST_END(testoptions->testcase_id);
	return status;
}

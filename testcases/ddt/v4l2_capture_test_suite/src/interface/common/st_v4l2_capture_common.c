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
#include "st_v4l2_capture_common.h"

/* Buffers for display */
static struct buf_info display_buff_info[MAX_BUFFERS];
static struct buf_info capture_buff_info[MAX_BUFFERS];
static struct buf_info video2_buff_info[MAX_BUFFERS];
static unsigned char *cap_ptr, *dis_ptr;

#define VIDEO2_DEVICE		"/dev/video2"
#define VIDEO2_NAME		"Mem-Allocator"
#define CAPTURE_NAME		"Capture"

#define DEF_PIX_FMT		V4L2_PIX_FMT_UYVY
#define IMG_WIDTH 720
#define IMG_HEIGHT 576

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
* This function opens V4L2_CAPTURE window
*/
int st_v4l2_capture_open(char *name, int *dev)
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
* This function closes V4L2_CAPTURE window
*/
int st_v4l2_capture_close(int dev)
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
		return retVal;
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
int st_v4l2_display_query_buffer_mmap_cap(int dev, int count,
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
		capture_buff_info[i].length = st_buf->length;
		capture_buff_info[i].index = i;

		capture_buff_info[i].start =
			mmap(NULL, st_buf->length, PROT_READ |
				PROT_WRITE, MAP_SHARED, dev, st_buf->m.offset);
		if ((unsigned int *)capture_buff_info[i].start == MAP_FAILED ||
			(unsigned int)capture_buff_info[i].start ==
					 MAP_SHARED) {
			return retVal;
		}

		memset(capture_buff_info[i].start, 0x80, st_buf->length);

		retVal = ioctl(dev, VIDIOC_QBUF, st_buf);
		if (retVal < 0) {
			perror("VIDIOC_QBUF");
			return retVal;
		}
	}

	return retVal;
}

/*
* This function implements the QUERYBUF ioctl and mmap
*/
int st_v4l2_display_query_buffer_mmap_disp(int dev, 
						  int count,
					         struct v4l2_format *st_fmt, 
						  struct v4l2_buffer *st_buf, 
						  int pixelfmt)
{
	int retVal = SUCCESS;
	int i = 0;

	for (i = 0; i < 3 ; i++) {
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
					PROT_WRITE, MAP_SHARED, dev, st_buf->m.offset);
		if ((unsigned int *)display_buff_info[i].start == MAP_FAILED ||
				(unsigned int)display_buff_info[i].start ==
				MAP_SHARED) {
			return retVal;
		}

		memset(display_buff_info[i].start, 0x80, st_buf->length);

		retVal = ioctl(dev, VIDIOC_QBUF, st_buf);
		if (retVal < 0) {
			perror("VIDIOC_QBUF");
			return retVal;
		}
	}

	return retVal;
}

/*
* This function enqueues the buffers
*/
int st_v4l2_display_queue_buffers_mmap(int dev,
					     int count,
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
int st_v4l2_capture_streamon(int dev)
{
	int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
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
* This function implements the STREAMON ioctl
*/
int st_v4l2_capture_streamoff(int dev)
{
	int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
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
int st_v4l2_display_unmap_mmap_cap(int count)
{
	int i = 0;
	int retVal = SUCCESS;

	for (i = 0; i < count; i++)
		retVal = munmap(capture_buff_info[i].start,
				capture_buff_info[i].length);
	if (SUCCESS > retVal)
		return retVal;

	return retVal;
}

/*
* This function implements the unmap call
*/
int st_v4l2_display_unmap_mmap_disp(int count)
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
int st_v4l2_display_unmap_userptr_disp(int count)
{
        int i = 0;
        int retVal = SUCCESS;

        for (i = 0; i < count; i++)
                retVal = munmap(video2_buff_info[i].start,
                                video2_buff_info[i].length);
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
* This function initilaizes the default values for
*/
void init_v4l2_capture_test_params(struct v4l2_capture_testparams *testoptions)
{
	testoptions->device_name_cap = DEFAULT_NODE_CAP;
	testoptions->device_name_disp = DEFAULT_NODE_DISP;
	testoptions->st_reqbuf_cap.count = DEFAULT_NO_BUFFERS;
	testoptions->st_reqbuf_disp.count = DEFAULT_NO_BUFFERS;
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
	} else if (capability->capabilities & V4L2_CAP_VIDEO_CAPTURE) {
		TEST_PRINT_TRC("Capture capability is supported");
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
* Enum input for capture
*/
int st_v4l2_capture_enum_input_interface(int dev,
	 struct v4l2_input *st_input)
{
	int retVal = SUCCESS;
	retVal = ioctl(dev, VIDIOC_ENUMINPUT, st_input);
	if (SUCCESS > retVal) {
		perror("VIDIOC_ENUMINPUT");
		return retVal;
	}
	return retVal;
}

/*
* Get input for capture
*/
int st_v4l2_capture_get_input_interface(int dev,
	 struct v4l2_input *st_input)
{
	int retVal = SUCCESS;
	int index;

	retVal = ioctl(dev, VIDIOC_G_INPUT, &index);
	if (SUCCESS > retVal) {
		perror("VIDIOC_G_INPUT");
		return retVal;
	}

	st_input->index = index;
	return retVal;
}

/*
* Set input for cpapture
*/
int st_v4l2_capture_set_input_interface(int dev,
	 struct v4l2_input *st_input)
{
	int retVal = SUCCESS;
	int index = st_input->index;

	retVal = ioctl(dev, VIDIOC_S_INPUT, &index);
	if (SUCCESS > retVal) {
		perror("VIDIOC_S_INPUT");
		return retVal;
	}

	return retVal;
}

/*
* Capture querystd
*/
int st_v4l2_capture_querystd_interface(int dev, v4l2_std_id *st_std_id)
{
	int retVal = SUCCESS;
	retVal = ioctl(dev, VIDIOC_QUERYSTD, st_std_id);

	if (SUCCESS > retVal) {
		perror("VIDIOC_QUERYSTD");
		return retVal;
	}

	return retVal;
}

/*
* Capture get standard
*/
int st_v4l2_capture_get_std_interface(int dev, v4l2_std_id *st_std_id)
{
	int retVal = SUCCESS;

	retVal = ioctl(dev, VIDIOC_G_STD, st_std_id);
	if (SUCCESS > retVal) {
		perror("VIDIOC_G_STD");
		return retVal;
	}

	return retVal;
}

/*
* Capture set standard
*/
int st_v4l2_capture_enum_std_interface(int dev,
		 			     struct v4l2_standard *st_standard, 
					     v4l2_std_id st_std_id)
{
	int retVal = SUCCESS;
	st_standard->index = 0;

	while (1) {

		retVal = ioctl(dev, VIDIOC_ENUMSTD, st_standard);
		if ((st_standard->id) & (st_std_id))
			break;
		st_standard->index++;

	}

	if (SUCCESS > retVal) {
		perror("VIDIOC_ENUMSTD");
		return retVal;
	}

	return retVal;
}

/*
* Capture Q/DQ BUF and swaping buffers and display
*/
int st_v4l2_capture_display_interface(struct v4l2_buffer *st_buf_cap,
		 			    struct v4l2_buffer *st_buf_disp, 
					    int loop,
					    int dev_cap,
					    int dev_disp,
					    struct v4l2_format *st_fmt_cap,
					    struct v4l2_format *st_fmt_disp)
{
	int retVal = SUCCESS;
	int i, h;
	/*unsigned int cpu_load;

	struct timeval before, after, result;
	struct proc_stat cpuload;

	start_cpuload_measurement(&cpuload);
	
	gettimeofday(&before, NULL);*/

	for (i = 0; i < loop; i++) {
		/* Dequeue display buffer */
		retVal = ioctl(dev_cap, VIDIOC_DQBUF, st_buf_cap);
		if (SUCCESS > retVal) {
			perror("VIDIOC_DQBUF");
			return retVal;
		}

		/* Dequeue capture buffer */
		retVal = ioctl(dev_disp, VIDIOC_DQBUF, st_buf_disp);
		if (SUCCESS > retVal) {
			perror("VIDIOC_DQBUF");
			return retVal;
		}

		cap_ptr = (unsigned char *)
			capture_buff_info[st_buf_cap->index].start;
		dis_ptr = (unsigned char *)
			display_buff_info[st_buf_disp->index].start;

		for (h = 0; h < st_fmt_disp->fmt.pix.height; h++) {
			memcpy(dis_ptr, cap_ptr,
					st_fmt_disp->fmt.pix.width * 2);

			cap_ptr += st_fmt_cap->fmt.pix.width * 2;
			dis_ptr += st_fmt_disp->fmt.pix.width * 2;
		}

		/* Queue display buffer */
		retVal = ioctl(dev_cap, VIDIOC_QBUF, st_buf_cap);
		if (SUCCESS > retVal) {
			perror("VIDIOC_QBUF");
			return retVal;
		}

		/* Queue capture buffer */
		retVal = ioctl(dev_disp, VIDIOC_QBUF, st_buf_disp);
		if (SUCCESS > retVal) {
			perror("VIDIOC_QBUF");
			return retVal;
		}

	}
	/*cpu_load = stop_cpuload_measurement(&cpuload);
	TEST_PRINT_TRC("The V4L2 Capture : Percentage CPU Load = %u%%",
				cpu_load);
       gettimeofday(&after, NULL);
	timeval_subtract(&result, &after, &before);

       TEST_PRINT_TRC("Calculated Frame Rate:\t%ld Fps", loop/result.tv_sec);	*/

	return retVal;
}

/*
* This function implements the userpointer buffer maping
*/
int st_v4l2_display_query_buffer_userptr_cap(int dev, 
						    int count,
						    struct v4l2_format *st_fmt, 
						    struct v4l2_buffer *st_buf, 
						    int pixelfmt)
{
	int retVal = SUCCESS;
	int i = 0;
	unsigned int buf_length;

	
	buf_length = st_fmt->fmt.pix.width * st_fmt->fmt.pix.height * 2;
	buf_length = (buf_length + 4096) & (~0xFFF);

	for (i = 0; i < count ; i++) {

		/* Mmap */
		capture_buff_info[i].length = st_buf->length;
		capture_buff_info[i].index = i;
		capture_buff_info[i].start = video2_buff_info[i+3].start;

		st_buf->index = i;
		st_buf->length = buf_length;
		st_buf->m.userptr = (unsigned int)capture_buff_info[i].start;

		retVal = ioctl(dev, VIDIOC_QBUF, st_buf);
		if (retVal < 0) {
			perror("VIDIOC_QBUF");
			return retVal;
		}
	}

	return retVal;
}

/*
* clean buffers
*/

void clean_buffers(void)
{
	int i;

	for (i = 0; i < MAX_BUFFERS; i++)
		capture_buff_info[i].start = NULL;

	for (i = 0; i < MAX_BUFFERS; i++)
		display_buff_info[i].start = NULL;
}

/*
* This IOCTL is used to set the value for a particular
*/
int st_v4l2_query_crtl_interface(int dev, struct v4l2_control *st_ctrl)
{
    int ret = SUCCESS;
    st_ctrl->id = V4L2_CID_CONTRAST;
    ret = ioctl(dev, VIDIOC_QUERYCTRL, st_ctrl);
    if (SUCCESS != ret) {
	perror("VIDIOC_QUERYCTRL");
	return FAILURE;
    }
    TEST_PRINT_TRC("The V4L2 Capture : Default Contrast = %u%%",
				st_ctrl->value);

    st_ctrl->id = V4L2_CID_BRIGHTNESS;
    ret = ioctl(dev, VIDIOC_QUERYCTRL, st_ctrl);
    if (SUCCESS != ret) {
	perror("VIDIOC_QUERYCTRL");
	return FAILURE;
    }
    TEST_PRINT_TRC("The V4L2 Capture : Default Brightness = %u%%",
				st_ctrl->value);

    st_ctrl->id = V4L2_CID_SATURATION;
    ret = ioctl(dev, VIDIOC_QUERYCTRL, st_ctrl);
    if (SUCCESS != ret) {
	perror("VIDIOC_QUERYCTRL");
	return FAILURE;
    }
    TEST_PRINT_TRC("The V4L2 Capture : Default Saturation = %u%%",
				st_ctrl->value);

    st_ctrl->id = V4L2_CID_HUE;
    ret = ioctl(dev, VIDIOC_QUERYCTRL, st_ctrl);
    if (SUCCESS != ret) {
	perror("VIDIOC_QUERYCTRL");
	return FAILURE;
    }
    TEST_PRINT_TRC("The V4L2 Capture : Default Hue = %u%%",
				st_ctrl->value);

   return SUCCESS;
}


/*
* This IOCTL is used to set the value for a particular
*/
int st_v4l2_set_crtl_interface(int dev, struct v4l2_control *st_ctrl)
{
    int ret = SUCCESS;
    st_ctrl->id = V4L2_CID_CONTRAST;
    st_ctrl->value = 100;

    ret = ioctl(dev, VIDIOC_S_CTRL, st_ctrl);
    if (SUCCESS != ret) {
		perror("VIDIOC_S_CTRL");
        return FAILURE;
    }
    return SUCCESS;
}

/*
* This IOCTL is used to get the value for a particular
*/
int st_v4l2_get_crtl_interface(int dev, struct v4l2_control *st_ctrl)
{
    int ret = SUCCESS;

    ret = ioctl(dev, VIDIOC_G_CTRL, st_ctrl);
    if (SUCCESS != ret) {
		perror("VIDIOC_G_CTRL");
        return FAILURE;
    }
    return SUCCESS;
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
 * This function displays the help/usage
 */
void display_v4l2_capture_test_suite_help(void)
{
	printf("v4l2CaptureTestSuite");
	printf("Usage:\n"
			"./v4l2_capture_tests <options>\n\n"
			"-device_disp          --device_disp    Display device node\n"
			"-device_cap           --device_cap     Capture device node\n"
			"\t\t\t\tPossible values-/dev/video2, /dev/video1\n"
			"-pixelformat          --pixelformat    Pixelformat\n"
			"\t\t\t\tPossible values-1:RGB565, 2:YUYV,\n"
			"\t\t\t\t3:UVVY, 4:RGB24, 5:RGB32, 6:NV16\n\n"
			"-width                --width          Width to display\n"
			"-height               --height         Height to display\n"
			"-loop                 --loop           Number of loop\n"
			"-buffernos            --buffernos      Number of buffers\n"
			"-buf_type             --buf_type       1:OUTPUT, 2:OVERLAY, 3:CAPTURE\n"
			"-crtl_value           --crtl_value     Control Value 0 to 255 only\n"
			"-crtl_id              --crtl_id        Control ID\n"
			"\t\t\t\tPossible values-\n"	
			"\t\t\t\t 1:Contrast, 2:Brightness, 3:Hue, \n" 
			"\t\t\t\t 4:Saturation, 5:Autogain\n"			
			"-id                   --id             Testcase ID\n"
			"-ioctl_no             --ioctl_no       IOCTL number for IOCTL test\n"
			"\t\t\t\tPossible values-\n"
			"\t\t\t\t 0:QUERY_CAP, 1:G_INPUT, 2:ENUM_INPUT, \n" 
			"\t\t\t\t 3:S_INPUT, 4:G_STD, 5:ENUM_STD, 6:G_FMT, 7:S_FMT,\n"
			"\t\t\t\t 8:REQBUF, 9:QUERYBUF, 10:ENQBUF, 11:STREAM_ON/OFF\n");
}

int initVideo2Plane(int *video2_fd, int *numbuffers, char *stdname,
				struct v4l2_format *fmt)
{
	int ret, i;
	struct v4l2_requestbuffers reqbuf;
	struct v4l2_buffer buf;
	unsigned int buf_length;

	/* Open the video 2 device */
	*video2_fd = open((const char *) VIDEO2_DEVICE, O_RDWR);
	if (*video2_fd <= 0) {
		TEST_PRINT_ERR("Cannot open = %s device\n", VIDEO2_DEVICE);
		return -1;
	}
	TEST_PRINT_TRC("\n%s: Opened Channel\n", VIDEO2_NAME);

	/* Get the format */
	fmt->type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	ret = ioctl(*video2_fd, VIDIOC_G_FMT, fmt);
	if (ret < 0) {
		perror("VIDIOC_G_FMT");
		goto ERROR;
	}

	fmt->fmt.pix.width = IMG_WIDTH;
	fmt->fmt.pix.height = IMG_HEIGHT;
	fmt->fmt.pix.pixelformat = DEF_PIX_FMT;

	fmt->type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	ret = ioctl(*video2_fd, VIDIOC_S_FMT, fmt);
	if (ret < 0) {
		perror("VIDIOC_S_FMT");
		goto ERROR;
	}

	ret = ioctl(*video2_fd, VIDIOC_G_FMT, fmt);
	if (ret < 0) {
		perror("VIDIOC_G_FMT");
		goto ERROR;
	}
	buf_length = fmt->fmt.pix.width * fmt->fmt.pix.height * 2;
	buf_length = (buf_length + 4096) & (~0xFFF);

	if (fmt->fmt.pix.pixelformat != DEF_PIX_FMT) {
		TEST_PRINT_ERR("%s: Requested pixel format not supported\n",
				CAPTURE_NAME);
		goto ERROR;
	}

	/* Buffer allocation
	 * Buffer can be allocated either from capture driver or
	 * user pointer can be used
	 */
	/* Request for MAX_BUFFER input buffers. As far as Physically contiguous
	 * memory is available, driver can allocate as many buffers as
	 * possible. If memory is not available, it returns number of
	 * buffers it has allocated in count member of reqbuf.
	 * HERE count = number of buffer to be allocated.
	 * type = type of device for which buffers are to be allocated.
	 * memory = type of the buffers requested i.e. driver allocated or
	 * user pointer */
	reqbuf.count = *numbuffers;
	reqbuf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	reqbuf.memory = V4L2_MEMORY_MMAP;
	ret = ioctl(*video2_fd, VIDIOC_REQBUFS, &reqbuf);
	if (ret < 0) {
		perror("Cannot allocate memory");
		goto ERROR;
	}
	/* Store the numbfer of buffers allocated */
	*numbuffers = reqbuf.count;
	TEST_PRINT_TRC("%s: Number of requested buffers = %d\n", VIDEO2_NAME,
	       (*numbuffers));

	memset(&buf, 0, sizeof(buf));

	/* Mmap the buffers
	 * To access driver allocated buffer in application space, they have
	 * to be mmapped in the application space using mmap system call */
	for (i = 0; i < (*numbuffers); i++) {
		buf.type = reqbuf.type;
		buf.index = i;
		buf.memory = reqbuf.memory;
		ret = ioctl(*video2_fd, VIDIOC_QUERYBUF, &buf);
		if (ret < 0) {
			perror("VIDIOC_QUERYBUF");
			*numbuffers = i;
			goto ERROR;
		}

		/* Mmap the buffers in application space */
		video2_buff_info[i].length = buf_length;
		video2_buff_info[i].index =  i;
		video2_buff_info[i].start =  mmap(NULL, buf.length,
			PROT_READ | PROT_WRITE,	MAP_SHARED, *video2_fd,
			buf.m.offset);

		if (video2_buff_info[i].start == MAP_FAILED) {
			TEST_PRINT_ERR("Cannot mmap = %d buffer\n", i);
			(*numbuffers) = i;
			goto ERROR;
		}
		memset((void *) video2_buff_info[i].start, 0x80,
		       video2_buff_info[i].length);
	}
	TEST_PRINT_TRC("%s: Init done successfully\n\n", VIDEO2_NAME);
	return 0;

ERROR:
	close(*video2_fd);

	return -1;
}

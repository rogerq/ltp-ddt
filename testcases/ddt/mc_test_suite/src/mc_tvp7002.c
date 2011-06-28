/* Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the
 * distribution.
 *
 * Neither the name of Texas Instruments Incorporated nor the names of
 * its contributors may be used to endorse or promote products derived
 * from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *NOTE :
 *       [TVP7002]--->[CCDC]--->[CCDC Video out node]--->[display video node]
 *       media pipeline will be set frames are loopback.
 */


#include "common.h"
#define CODE		V4L2_MBUS_FMT_YUYV10_1X20

#include <media/davinci/videohd.h>
/*******************************************************************************/

int mc_tvp7002_test(struct mc_capture_testparams *testoptions)
{
	int i, index, tvp_fd = 0, ccdc_fd = 0, capt_fd = 0, ret, capture_pitch, frame_count, cap_numbuffers = 0, disp_numbuffers = 0;
	struct v4l2_subdev_format fmt;
	struct v4l2_input input;	
	struct v4l2_format v4l2_fmt;
	struct v4l2_requestbuffers req;
	enum v4l2_buf_type type;
	char *src, *dst;
	struct v4l2_buffer cap_buf;
	struct v4l2_buffer in_querybuf;
	int display_fd = 0;
	int disp_buf_index = 0;
	int display_pitch = 0;
	struct v4l2_format format;
	struct v4l2_requestbuffers out_reqbuff;
	struct v4l2_buffer out_querybuf,disp_qbuf;
	struct v4l2_output output;
	struct v4l2_dv_preset reqpreset;
	struct media_dev media;

	/* defaults */
	reqpreset.preset = V4L2_DV_720P60;
	int inp_width = 1280, inp_height = 720;
/*
	for (;;) {
		c = getopt_long(argc, argv, shortoptions, (void *) NULL,
				&index);
		if (-1 == c)
			break;
		switch (c) {
		case 'i':
			choice = atoi(optarg);
			printf("choice:%d\n", choice);
			switch (choice){
			case 0:
				inp_width=720;inp_height=480;
				reqpreset.preset = V4L2_DV_480P59_94;
				break;
			case 1:
				inp_width=720;inp_height=576;
				reqpreset.preset = V4L2_DV_576P50;
				break;
			case 2:
				inp_width=1280;inp_height= 720;
				reqpreset.preset = V4L2_DV_720P60;
				break;
			default:
				usage();
				exit(1);
			}
			break;
		default:
			usage();
			exit(1);
		}
	}
*/

	inp_width = testoptions->inp_width;
	inp_height = testoptions->inp_height;
	if(inp_height == 480)
		reqpreset.preset = V4L2_DV_480P59_94;		
	else if(inp_height == 576)
		reqpreset.preset = V4L2_DV_576P50;
	else
		reqpreset.preset = V4L2_DV_720P60;


	int buf_size = ALIGN((inp_width*inp_height*2), 4096);

	memset(&media, 0, sizeof(struct media_dev));

	/* 3.open media device */
	ret  = media_device_open(&media);
	if (ret < 0)
		return ret;

	media.hd = 1;
	/* 4.enumerate media-entities */
	ret = enumerate_all_entities(&media);
	if (ret < 0)
		goto err_0;
	printf("**********************************************\n");

	/* 5.enumerate links for each entity */
	ret = setup_media_links(&media);
	if (ret < 0)
		goto err_0;

	media_device_close(media.media_fd);

	printf("**********************************************\n");

	/* 6. open capture device */
	if ((capt_fd = open("/dev/video0", O_RDWR | O_NONBLOCK, 0)) <= -1) {
		printf("failed to open %s \n", "/dev/video0");
		goto cleanup;
	}

	/* 7.enumerate inputs supported by capture via tvp514x[wh s active through mc] */
	printf("enumerating INPUTS\n");
	bzero(&input, sizeof(struct v4l2_input));
	input.type = V4L2_INPUT_TYPE_CAMERA;
	input.index = 0;
	index = 0;
  	while (1) {

		ret = ioctl(capt_fd, VIDIOC_ENUMINPUT, &input);
		if(ret != 0)
			break;

		printf("[%x].%s\n", index, input.name);

		bzero(&input, sizeof(struct v4l2_input));
		index++;
		input.index = index;
  	}
  	printf("**********************************************\n");

	/* 8.setting COMPONENT input */
	printf("setting COMPONENT input. . .\n");
	bzero(&input, sizeof(struct v4l2_input));
	input.type = V4L2_INPUT_TYPE_CAMERA;
	input.index = 0;
	if (-1 == ioctl (capt_fd, VIDIOC_S_INPUT, &input.index)) {
		printf("failed to set COMPONENT with capture device\n");
		goto cleanup;
	} else
		printf("successfully set COMPONENT input\n");

	printf("**********************************************\n");

	/* 9.setting dv preset */
 	printf("setting dv preset. . .\n");
 	if (-1 == ioctl(capt_fd, VIDIOC_S_DV_PRESET, &reqpreset)) {
 		printf("failed to set preset on capture device\n");
 		goto cleanup;
 	} else {
 		printf("successfully set dv preset on capture\n");
 	}

	printf("**********************************************\n");


	/* 10. open tvp7002 device */
	tvp_fd = open("/dev/v4l-subdev1", O_RDWR);
	if(tvp_fd == -1) {
		printf("failed to open %s\n", "/dev/v4l-subdev0");
		goto cleanup;
	}

	/* 11. set format on TV7002 pad */
	printf("setting format on pad of tvp7002 entity. . .\n");
	memset(&fmt, 0, sizeof(fmt));

	fmt.pad = P_TVP7002;
	fmt.which = V4L2_SUBDEV_FORMAT_ACTIVE;
	fmt.format.code = CODE;
	fmt.format.width = inp_width;
	fmt.format.height = inp_height;
	fmt.format.field = V4L2_FIELD_NONE;

	ret = ioctl(tvp_fd, VIDIOC_SUBDEV_S_FMT, &fmt);
	if(ret) {
		printf("failed to set format on pad %x\n", fmt.pad);
		goto cleanup;
	}
	else
		printf("successfully format is set on pad %x\n", fmt.pad);


	/* 12 set format on sink-pad of ccdc */
	ccdc_fd = open("/dev/v4l-subdev2", O_RDWR);
	if(ccdc_fd == -1) {
		printf("failed to open %s\n", "/dev/v4l-subdev1");
		goto cleanup;
	}

	printf("setting format on sink-pad of ccdc entity. . .\n");
	memset(&fmt, 0, sizeof(fmt));

	fmt.pad = P_CCDC_SINK;
	fmt.which = V4L2_SUBDEV_FORMAT_ACTIVE;
	fmt.format.code = CODE;
	fmt.format.width = inp_width;
	fmt.format.height = inp_height;
	fmt.format.field = V4L2_FIELD_NONE;

	ret = ioctl(ccdc_fd, VIDIOC_SUBDEV_S_FMT, &fmt);
	if(ret) {
		printf("failed to set format on pad %x\n", fmt.pad);
		goto cleanup;
	}
	else
		printf("successfully format is set on pad %x\n", fmt.pad);

	/* 13. set format on OF-pad of ccdc */
	printf("setting format on OF-pad of ccdc entity. . . \n");
	memset(&fmt, 0, sizeof(fmt));

	fmt.pad = P_CCDC_SOURCE;
	fmt.which = V4L2_SUBDEV_FORMAT_ACTIVE;
	fmt.format.code = CODE;
	fmt.format.width = inp_width;
	fmt.format.height = inp_height;
	fmt.format.field = V4L2_FIELD_NONE;

	ret = ioctl(ccdc_fd, VIDIOC_SUBDEV_S_FMT, &fmt);
	if(ret) {
		printf("failed to set format on pad %x\n", fmt.pad);
		goto cleanup;
	}
	else
		printf("successfully format is set on pad %x\n", fmt.pad);

	printf("**********************************************\n");


	/* 14.setting format */
	printf("setting format V4L2_PIX_FMT_UYVY\n");
	CLEAR(v4l2_fmt);
	v4l2_fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	v4l2_fmt.fmt.pix.width = inp_width;
	v4l2_fmt.fmt.pix.height = inp_height;
	v4l2_fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_UYVY;
	v4l2_fmt.fmt.pix.field = V4L2_FIELD_NONE;

	if (-1 == ioctl(capt_fd, VIDIOC_S_FMT, &v4l2_fmt)) {
		printf("failed to set format on captute device \n");
		goto cleanup;
	} else
		printf("successfully set the format\n");

	/* 15.call G_FMT for knowing picth */
	if (-1 == ioctl(capt_fd, VIDIOC_G_FMT, &v4l2_fmt)) {
		printf("failed to get format from captute device \n");
		goto cleanup;
	} else {
		printf("capture_pitch: %x\n", v4l2_fmt.fmt.pix.bytesperline);
		capture_pitch = v4l2_fmt.fmt.pix.bytesperline;
	}
	

	printf("**********************************************\n");

	/* 16.make sure 3 buffers are supported for streaming */
	CLEAR(req);
	req.count = APP_NUM_BUFS;
	req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	req.memory = V4L2_MEMORY_MMAP;

	if (-1 == ioctl(capt_fd, VIDIOC_REQBUFS, &req)) {
		printf("call to VIDIOC_REQBUFS failed\n");
		goto cleanup;
	}

	if (req.count != APP_NUM_BUFS) {
		printf("%d buffers not supported by capture device\n", APP_NUM_BUFS);
		printf("%d buffers supported\n", req.count);
	} else
		printf("%d buffers are supported for streaming\n", req.count);

	cap_numbuffers = req.count;

	printf("**********************************************\n");
	
	/* 17. calling ioctl QUERYBUF and MMAP*/
	printf("Querying capture buffers and MMAPing capture\n");
	for(i = 0;  i < cap_numbuffers; i++) {
		CLEAR(in_querybuf);
		in_querybuf.index = i;
		in_querybuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		in_querybuf.memory = V4L2_MEMORY_MMAP;
		
		ret = ioctl(capt_fd, VIDIOC_QUERYBUF, &in_querybuf);
		if(ret < 0) {
			printf("capture QUERYBUF failed\n");
			exit(0);
		}
		printf("buffer %d queried \n", i);
		capture_buffers[i].length = in_querybuf.length;
		printf("buf length : %d\n",in_querybuf.length);
		capture_buffers[i].start = (char*) mmap(NULL, in_querybuf.length,
							      PROT_READ | PROT_WRITE,
							      MAP_SHARED, capt_fd,
							      in_querybuf.m.offset); 
		if((void*)capture_buffers[i].start == (void*) -1) {
			printf("capture MMAP failed\n");
			exit(0);
		}
		printf("Capture Buffer %u mapped at address %p.\n",i,
		       capture_buffers[i].start);
	}
	
	/* 18. open display device */
	display_fd = open(DISPLAY_DEVICE, O_RDWR);
	if(display_fd < 0) {
		printf("Failed to open DISPLAY_DEVICE device\n");
		exit(0);
	}
	printf("DISPLAY_DEVICE opened\n");
	
	/* 19.setting COMPONENT output */
	printf("setting COMPONENT output. . .\n");
	bzero(&output, sizeof(struct v4l2_output));
	output.index = 1;
	if (-1 == ioctl (display_fd, VIDIOC_S_OUTPUT, &output.index)) {
		printf("failed to set COMPONENT with display device\n");
		goto cleanup;
	} else
		printf("successfully set COMPONENT display\n");
	
	/* 20.setting dv preset */
 	printf("setting dv preset. . .\n");
 	if (-1 == ioctl(display_fd, VIDIOC_S_DV_PRESET, &reqpreset)) {
 		printf("failed to set preset on display device\n");
 		goto cleanup;
 	} else {
 		printf("successfully set dv preset on display\n");
 	}
	
	/* 21. calling S_FMT on display*/
	printf("Test S_FMT display\n");
	bzero((void *)&format,sizeof( format));
	format.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	format.fmt.pix.width = inp_width;
	format.fmt.pix.height = inp_height;
	format.fmt.pix.pixelformat = V4L2_PIX_FMT_UYVY;
	ret = ioctl(display_fd, VIDIOC_S_FMT, &format);
	if(ret) {
		printf("display S_FMT failed\n");
		exit(0);
	}
	
	printf("display S_FMT successful\n");
	printf("size image : %d\n",format.fmt.pix.sizeimage);
	
	/* 22. calling G_FMT on display*/
	printf("Test G_FMT display\n");
	bzero((void *)&format,sizeof( format));
	format.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	ret = ioctl(display_fd, VIDIOC_G_FMT, &format);
	if(ret < 0) {
		printf("display G_FMT failed\n");
		exit(0);
	}
	printf("display G_FMT successful\n");

	printf("**********************************************\n");

	/* 23. calling ioctl REQBUF on display*/
	printf("Test REQBUF display\n");
	out_reqbuff.count = cap_numbuffers;
	out_reqbuff.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	out_reqbuff.memory = V4L2_MEMORY_MMAP;	
	ret = ioctl(display_fd, VIDIOC_REQBUFS, &out_reqbuff);
	if(ret)
		printf("REQBUFS call failed \n");
	
	if (out_reqbuff.count != cap_numbuffers) {
		printf("%d buffers not supported by capture device\n", cap_numbuffers);
		printf("%d buffers supported\n", out_reqbuff.count);
		goto cleanup;
	} else
		printf("%d buffers are supported for streaming\n", out_reqbuff.count);

	disp_numbuffers = out_reqbuff.count;

	/* 24. calling ioctl QUERYBUF and MMAP on display*/
	printf("Querying capture buffers and MMAPing capture\n");
	for(i = 0;  i < disp_numbuffers; i++) {
		CLEAR(out_querybuf);
		out_querybuf.index = i;
		out_querybuf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
		out_querybuf.memory = V4L2_MEMORY_MMAP;
		
		ret = ioctl(display_fd, VIDIOC_QUERYBUF, &out_querybuf);
		if(ret < 0) {
			printf("display QUERYBUF failed\n");
			exit(0);
		}
		printf("buffer %d queried \n", i);
		display_buffers[i].length = out_querybuf.length;
		printf("buf length : %d\n",out_querybuf.length);
		display_buffers[i].start = (char*) mmap(NULL, out_querybuf.length,
							      PROT_READ | PROT_WRITE,
							      MAP_SHARED, display_fd,
							      out_querybuf.m.offset); 
		if((void*)display_buffers[i].start == (void*) -1) {
			printf("Display MMAP failed\n");
			exit(0);
		}
		printf("Display Buffer %u mapped at address %p.\n",i,
		       display_buffers[i].start);
	}

	struct v4l2_buffer buf;
	/* 25. queue the capture buffers*/
	for (i = 0; i < cap_numbuffers; i++) {
		CLEAR(buf);
		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;
		buf.index = i;
		buf.length = buf_size;
		//printf("Q capture BUF %x---%x---%x\n",i,capture_buffers[i]);

		if (-1 == ioctl(capt_fd, VIDIOC_QBUF, &buf)) {
			printf("capture:call to VIDIOC_QBUF failed\n");
			goto cleanup;
		}
	}
	/* 26.queue the display buffers */
	for(disp_buf_index= 0; disp_buf_index < disp_numbuffers; disp_buf_index++) {
		CLEAR(disp_qbuf);
		disp_qbuf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
		disp_qbuf.memory = V4L2_MEMORY_MMAP;
		disp_qbuf.index = disp_buf_index;
		disp_qbuf.length = buf_size;
		//printf("q display buf:index: %x, %x: %x\n",i, display_buffers[i]);

		if (-1 == ioctl(display_fd, VIDIOC_QBUF, &disp_qbuf)) {
			printf("display:call to VIDIOC_QBUF failed\n");
			goto cleanup;
		}
	}
	printf("qbuf\n");
	/* 27.start streaming on capture*/
	CLEAR(type);
	type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if (-1 == ioctl(capt_fd, VIDIOC_STREAMON, &type)) {
		printf("failed to start streaming on capture device");
		goto cleanup;
	} else
		printf("streaming started successfully\n");
	/* 28. calling streamon on display*/
	printf("Test STREAMON display\n");
	CLEAR(type);
	type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	ret = ioctl(display_fd, VIDIOC_STREAMON, &type);
	if(ret < 0) {
		printf("display STREAMON failed\n");
		exit(0);
	}
	printf("display STREAMON successful\n");

	capture_pitch = inp_width * BYTESPERPIXEL;
	display_pitch = inp_width * BYTESPERPIXEL;

	frame_count = 0;
	/* 29.get 20 frames from capture device and store in a file */
	while(frame_count < 200) {

		CLEAR(cap_buf);
		CLEAR(disp_qbuf);

		cap_buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		cap_buf.memory = V4L2_MEMORY_MMAP;
try_again:
		ret = ioctl(capt_fd, VIDIOC_DQBUF, &cap_buf);
		if (ret < 0) {
			if (errno == EAGAIN) {
				goto try_again;
			}
			printf("failed to DQ buffer from capture device\n");
			goto cleanup;
		}

		disp_qbuf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
		disp_qbuf.memory = V4L2_MEMORY_MMAP;
		ret = ioctl(display_fd, VIDIOC_DQBUF, &disp_qbuf);
		if(ret < 0) {
			printf("display DQBUF failed\n");
			exit(0);
		}
		
		src = capture_buffers[cap_buf.index].start;
		dst = display_buffers[disp_qbuf.index].start;

		for(i=0 ; i < inp_height; i++) {
			memcpy(dst, src, display_pitch);
			src += capture_pitch;
			dst += display_pitch;
		}

		ret = ioctl(capt_fd, VIDIOC_QBUF, &cap_buf);
		if (ret < 0) {
			printf("failed to Q buffer onto capture device\n");
			goto cleanup;
		}
		
		disp_qbuf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
		disp_qbuf.memory = V4L2_MEMORY_MMAP;
		ret = ioctl(display_fd, VIDIOC_QBUF, &disp_qbuf);
		if(ret < 0) {
			printf("display QBUF failed\n");
			printf("errno : %d\n",errno);
			exit(0);
		}

		frame_count++;
	}

	printf("**********************************************\n");

	/* 30. do stream off on capture device */
	CLEAR(type);
	type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if (-1 == ioctl(capt_fd, VIDIOC_STREAMOFF, &type)) {
		printf("failed to stop streaming on capture device");
		goto cleanup;
	} else
		printf("streaming stopped successfully\n");
	/* 31. calling STREAMOFF on display*/
	printf("Test STREAMOFF display\n");
	CLEAR(type);
	type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	ret = ioctl(display_fd, VIDIOC_STREAMOFF, &type);
	if(ret < 0) {
		printf("display STREAMOFF failed\n");
		exit(0);
	}
	printf("display STREAMOFF successful\n");
cleanup:

	/* 32. unmapping mmapped buffers */
	for(i=0;i<cap_numbuffers;i++)
		munmap(capture_buffers[i].start, capture_buffers[i].length);

	printf("unmapped capture buffers\n");

	for(i=0;i<disp_numbuffers;i++)
		munmap(display_buffers[i].start, display_buffers[i].length);

	printf("unmapped display buffers\n");

	/* 25.close all the file descriptors */
	printf("closing all the file descriptors. . .\n");
	if(capt_fd) {
		close(capt_fd);
		printf("closed capture device\n");
	}
	if(ccdc_fd) {
		close(ccdc_fd);
		printf("closed ccdc sub-device\n");
	}
	if(tvp_fd) {
		close(tvp_fd);
		printf("closed tvp514x sub-device\n");
	}
	
	if(display_fd) {
		close(display_fd);
		printf("closed display device\n");
	}

	reset_media_links(&media);
err_0:
	media_device_close(media.media_fd);

	return ret;
}
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
 *NOTE : This aplication needs CMEM.
 *       [TVP7002]--->[CCDC]--->[PREVIEWER]--->[Video node] media pipeline will be set
 *       and frames captured.
 */

#include <media/davinci/imp_previewer.h>
#include <media/davinci/imp_resizer.h>
#include <media/davinci/dm365_ipipe.h>
#include "common.h"
/*******************************************************************************/
#define ALIGN(x, y)	(((x + (y-1))/y)*y)
#define CLEAR(x) memset (&(x), 0, sizeof (x))
#define APP_NUM_BUFS	3

#define RZR_DEV_NODE "/dev/v4l-subdev4"
#define PRV_DEV_NODE "/dev/v4l-subdev3"
#define CAP_DEV_NODE "/dev/video4"
#define CAP_TVP514_DEV_NODE "/dev/v4l-subdev1"
#define CCDC_DEV_NODE "/dev/v4l-subdev2"

/* structure to hold address */
struct mmap_buffer {
	void *start;
	size_t length;
};

struct mmap_buffer	capture_buffers_prv[APP_NUM_BUFS];
struct mmap_buffer	display_buffers_prv[APP_NUM_BUFS];

struct media_entity_desc	entity[15];
struct media_links_enum		links;
int				entities_count;

#define CODE		V4L2_MBUS_FMT_YUYV8_2X8

static unsigned long long prev_ts;
static unsigned long long curr_ts;
static unsigned long fp_period_average;
static unsigned long fp_period_max;
static unsigned long fp_period_min;
static unsigned long fp_period;

int mc_tvp514_prv_rzr(struct mc_capture_testparams *testoptions)
{
	int i, index,  tvp_fd = 0, ccdc_fd = 0, prv_fd = 0, rsz_fd = 0;
	int capt_fd = 0, ret=0, capture_pitch, frame_count, disp_second_output = 0, output_format;
	struct media_link_desc link;
	struct v4l2_subdev_format fmt;
	struct v4l2_input input;
	struct v4l2_format v4l2_fmt;
	struct v4l2_requestbuffers req;
	enum v4l2_buf_type type;
	struct v4l2_buffer cap_buf;
	struct prev_channel_config prev_chan_config;
	struct prev_continuous_config prev_cont_config;
	struct prev_cap cap;
	struct prev_module_param mod_param;
	int display_fd = 0;
	int disp_buf_index = 0;
	int display_pitch = 0;
	struct v4l2_output output;
	struct v4l2_format format;
	struct v4l2_requestbuffers out_reqbuff;
	struct v4l2_buffer disp_qbuf;
	struct rsz_channel_config rsz_chan_config;
	struct rsz_continuous_config rsz_cont_config;
	v4l2_std_id cur_std;
	int cap_numbuffers = 0;
	char *src, *dst;
	int disp_numbuffers = 0;
	v4l2_std_id standard_id = V4L2_STD_NTSC;
	int width = 720, height = 480;
	int out_width, out_height;
	int buf_size;
	struct media_dev media;

	out_width = testoptions->inp_width;
	out_height = testoptions->inp_height;
	printf("Width %d and Height %d\n",out_width,out_height);
	buf_size = ALIGN((out_width*out_height*2),4096);

	memset(&media, 0, sizeof(struct media_dev));

	/* 3.open media device */
	ret  = media_device_open(&media);
	if (ret < 0)
		return ret;

	/* 4.enumerate media-entities */
	ret = enumerate_all_entities_rsz(&media);
	if (ret < 0)
		media_device_close(media.media_fd);
	printf("**********************************************\n");

	/* 5.enumerate links for each entity */
	ret = setup_media_links_rsz(&media);
	if (ret < 0)
		media_device_close(media.media_fd);

	media_device_close(media.media_fd);
	/* 13.open rsz device */
	rsz_fd = open(RZR_DEV_NODE, O_RDWR);
	if(prv_fd == -1) {
		printf("failed to open %s\n", RZR_DEV_NODE);
		goto cleanup;
	}

	/* 14. set default configuration in rsz */
	//rsz_chan_config.oper_mode = IMP_MODE_CONTINUOUS;
	rsz_chan_config.chain  = 1;
	rsz_chan_config.len = 0;
	rsz_chan_config.config = NULL;
	if (ioctl(rsz_fd, RSZ_S_CONFIG, &rsz_chan_config) < 0) {
		perror("failed to set default configuration in resizer\n");
		goto cleanup;
	} else
		printf("default configuration setting in resizer successfull\n");

	/* 15. get configuration from rsz */
	bzero(&rsz_cont_config, sizeof(struct rsz_continuous_config));
	//rsz_chan_config.oper_mode = IMP_MODE_CONTINUOUS;
	rsz_chan_config.chain = 1;
	rsz_chan_config.len = sizeof(struct rsz_continuous_config);
	rsz_chan_config.config = &rsz_cont_config;

	if (ioctl(rsz_fd, RSZ_G_CONFIG, &rsz_chan_config) < 0) {
		perror("failed to get resizer channel configuration\n");
		goto cleanup;
	} else
		printf("congiration got from resizer successfully\n");

	/* 16. set configuration in rsz */
	rsz_cont_config.output1.enable = 1;
	if (disp_second_output) {
		rsz_cont_config.output2.enable = 1;
		rsz_cont_config.output2.width = out_width;
		rsz_cont_config.output2.height = out_height;


		if (output_format)
			rsz_cont_config.output2.pix_fmt = IPIPE_YUV420SP;
		else
			rsz_cont_config.output2.pix_fmt = IPIPE_UYVY;
	} else
		rsz_cont_config.output2.enable = 0;

	//rsz_chan_config.oper_mode = IMP_MODE_CONTINUOUS;
	rsz_chan_config.chain = 1;
	rsz_chan_config.len = sizeof(struct rsz_continuous_config);
	rsz_chan_config.config = &rsz_cont_config;
	if (ioctl(rsz_fd, RSZ_S_CONFIG, &rsz_chan_config) < 0) {
		perror("failed to set configuration in resizer\n");
		goto cleanup;
	} else
		printf("successfully set configuration in rsz \n");

	/* 20.open prv device */
	prv_fd = open(PRV_DEV_NODE, O_RDWR);
	if(prv_fd == -1) {
		printf("failed to open %s\n", PRV_DEV_NODE);
		goto cleanup;
	}

	/* 21. set default configuration in PRV */
	//prev_chan_config.oper_mode = IMP_MODE_CONTINUOUS;
	prev_chan_config.len = 0;
	prev_chan_config.config = NULL; /* to set defaults in driver */
	if (ioctl(prv_fd, PREV_S_CONFIG, &prev_chan_config) < 0) {
		perror("failed to set default configuration on prv\n");
		goto cleanup;
	} else
		printf("default configuration is set successfully on prv\n");

	/* 22. get configuration from prv */
	//prev_chan_config.oper_mode = IMP_MODE_CONTINUOUS;
	prev_chan_config.len = sizeof(struct prev_continuous_config);
	prev_chan_config.config = &prev_cont_config;

	if (ioctl(prv_fd, PREV_G_CONFIG, &prev_chan_config) < 0) {
		perror("failed to get default configuration from prv\n");
		goto cleanup;
	} else
		printf("got configuration from prv successfully\n");

	/* 23. set configuration in prv */
	//prev_chan_config.oper_mode = IMP_MODE_CONTINUOUS;
	prev_chan_config.len = sizeof(struct prev_continuous_config);
	prev_chan_config.config = &prev_cont_config;

	if (ioctl(prv_fd, PREV_S_CONFIG, &prev_chan_config) < 0) {
		perror("failed to set configuration on prv\n");
		goto cleanup;
	} else
		printf("configuration is set successfully on prv\n");

	/* 24. setting default prv-params */
	cap.index=0;
	while (1) {
		ret = ioctl(prv_fd , PREV_ENUM_CAP, &cap);
		if (ret < 0) {
			break;
		}

		strcpy(mod_param.version,cap.version);
		mod_param.module_id = cap.module_id;

		printf("setting default for prv-module %s\n", cap.module_name);
			mod_param.param = NULL;

		if (ioctl(prv_fd, PREV_S_PARAM, &mod_param) < 0) {
			printf("error in Setting %s prv-params from driver\n", cap.module_name);
			goto cleanup;
		}
		cap.index++;
	}
	printf("**********************************************\n");

	/* 27.open capture device */
	{
	if ((capt_fd = open(CAP_DEV_NODE, O_RDWR | O_NONBLOCK, 0)) <= -1) {
		printf("failed to open %s \n", CAP_DEV_NODE);
		goto cleanup;
	}

	/* 28.enumerate inputs supported by capture via tvp514x[wh s active through mc] */
	printf("12.enumerating INPUTS\n");
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

	/* 29.setting COMPOSITE input */
	printf("13.setting COMPOSITE input. . .\n");
	bzero(&input, sizeof(struct v4l2_input));
	input.type = V4L2_INPUT_TYPE_CAMERA;
	input.index = 0;
	if (-1 == ioctl (capt_fd, VIDIOC_S_INPUT, &input.index)) {
		printf("failed to set COMPOSITE with capture device\n");
		goto cleanup;
	} else
		printf("successfully set COMPOSITE input\n");

	printf("**********************************************\n");

	/* 14.setting NTSC std */
	printf("setting NTSC std. . .\n");
	bzero(&cur_std, sizeof(cur_std));
	cur_std = V4L2_STD_NTSC;
	if (-1 == ioctl(capt_fd, VIDIOC_S_STD, &cur_std)) {
		printf("failed to set NTSC std on capture device\n");
		goto cleanup;
	} else {
		printf("successfully set NTSC std\n");
	}


	printf("**********************************************\n");


	/* 10. set format on pad of tvp514x */
	tvp_fd = open(CAP_TVP514_DEV_NODE, O_RDWR);
	if(tvp_fd == -1) {
		printf("failed to open %s\n", CAP_TVP514_DEV_NODE);
		goto cleanup;
	}

	printf("8. setting format on pad of tvp514x entity. . .\n");
	memset(&fmt, 0, sizeof(fmt));

	fmt.pad = P_TVP514X;
	fmt.which = V4L2_SUBDEV_FORMAT_ACTIVE;
	fmt.format.code = CODE;
	fmt.format.width = width;
	fmt.format.height = height;
	fmt.format.colorspace = V4L2_COLORSPACE_SMPTE170M;
	fmt.format.field = V4L2_FIELD_INTERLACED;

	ret = ioctl(tvp_fd, VIDIOC_SUBDEV_S_FMT, &fmt);
	if(ret) {
		printf("failed to set format on pad %x\n", fmt.pad);
		goto cleanup;
	}
	else
		printf("successfully format is set on pad %x\n", fmt.pad);
	}

	/* 11. set format on sink-pad of ccdc */
	{
	ccdc_fd = open(CCDC_DEV_NODE, O_RDWR);
	if(ccdc_fd == -1) {
		printf("failed to open %s\n", CCDC_DEV_NODE);
		goto cleanup;
	}

	printf("9. setting format on sink-pad of ccdc entity. . .\n");
	memset(&fmt, 0, sizeof(fmt));

	fmt.pad = P_CCDC_SINK;
	fmt.which = V4L2_SUBDEV_FORMAT_ACTIVE;
	fmt.format.code = CODE;
	fmt.format.width = width;
	fmt.format.height = height;
	fmt.format.colorspace = V4L2_COLORSPACE_SMPTE170M;
	fmt.format.field = V4L2_FIELD_INTERLACED;

	ret = ioctl(ccdc_fd, VIDIOC_SUBDEV_S_FMT, &fmt);
	if(ret) {
		printf("failed to set format on pad %x\n", fmt.pad);
		goto cleanup;
	}
	else
		printf("successfully format is set on pad %x\n", fmt.pad);

	/* 12. set format on OF-pad of ccdc */
	printf("10. setting format on OF-pad of ccdc entity. . . \n");
	memset(&fmt, 0, sizeof(fmt));

	fmt.pad = P_CCDC_SOURCE;
	fmt.which = V4L2_SUBDEV_FORMAT_ACTIVE;
	fmt.format.code = CODE;
	fmt.format.width = width;
	fmt.format.height = height;
	fmt.format.colorspace = V4L2_COLORSPACE_SMPTE170M;
	fmt.format.field = V4L2_FIELD_INTERLACED;

	ret = ioctl(ccdc_fd, VIDIOC_SUBDEV_S_FMT, &fmt);
	if(ret) {
		printf("failed to set format on pad %x\n", fmt.pad);
		goto cleanup;
	}
	else
		printf("successfully format is set on pad %x\n", fmt.pad);

	/* 25. set format on sink-pad of prv */
	printf("10. setting format on sink-pad of prv entity. . . \n");
	memset(&fmt, 0, sizeof(fmt));

	fmt.pad = P_PRV_SINK;
	fmt.which = V4L2_SUBDEV_FORMAT_ACTIVE;
	fmt.format.code = CODE;
	fmt.format.width = width;
	fmt.format.height = height;
	fmt.format.colorspace = V4L2_COLORSPACE_SMPTE170M;
	fmt.format.field = V4L2_FIELD_INTERLACED;

	ret = ioctl(prv_fd, VIDIOC_SUBDEV_S_FMT, &fmt);
	if(ret) {
		printf("failed to set format on pad %x\n", fmt.pad);
		goto cleanup;
	}
	else
		printf("successfully format is set on pad %x\n", fmt.pad);

	/* 26. set format on source-pad of prv */
	printf("10. setting format on source-pad of prv entity. . . \n");
	memset(&fmt, 0, sizeof(fmt));

	fmt.pad = P_PRV_SOURCE;
	fmt.which = V4L2_SUBDEV_FORMAT_ACTIVE;
	fmt.format.code = CODE;
	fmt.format.width = width;
	fmt.format.height = height;
	fmt.format.colorspace = V4L2_COLORSPACE_SMPTE170M;
	fmt.format.field = V4L2_FIELD_INTERLACED;

	ret = ioctl(prv_fd, VIDIOC_SUBDEV_S_FMT, &fmt);
	if(ret) {
		printf("failed to set format on pad %x\n", fmt.pad);
		goto cleanup;
	}
	else
		printf("successfully format is set on pad %x\n", fmt.pad);

	printf("**********************************************\n");
	}

	/* 18. set format on sink-pad of rsz */
	printf("10. setting format on sink-pad of rsz entity. . . \n");
	memset(&fmt, 0, sizeof(fmt));

	fmt.pad = P_RSZ_SINK;
	fmt.which = V4L2_SUBDEV_FORMAT_ACTIVE;
	fmt.format.code = CODE;
	fmt.format.width = width;
	fmt.format.height = height;
	fmt.format.colorspace = V4L2_COLORSPACE_SMPTE170M;
	fmt.format.field = V4L2_FIELD_INTERLACED;

	ret = ioctl(rsz_fd, VIDIOC_SUBDEV_S_FMT, &fmt);
	if(ret) {
		printf("failed to set format on pad %x\n", fmt.pad);
		goto cleanup;
	}
	else
		printf("successfully format is set on pad %x\n", fmt.pad);

	/* 19. set format on source-pad of rsz */
	printf("10. setting format on source-pad of rsz entity. . . \n");
	memset(&fmt, 0, sizeof(fmt));

	fmt.pad = P_RSZ_SOURCE;
	fmt.which = V4L2_SUBDEV_FORMAT_ACTIVE;
	fmt.format.code = CODE;
	fmt.format.width = out_width;
	fmt.format.height = out_height;
	fmt.format.colorspace = V4L2_COLORSPACE_SMPTE170M;
	fmt.format.field = V4L2_FIELD_INTERLACED;

	ret = ioctl(rsz_fd, VIDIOC_SUBDEV_S_FMT, &fmt);
	if(ret) {
		printf("failed to set format on pad %x\n", fmt.pad);
		goto cleanup;
	}
	else
		printf("successfully format is set on pad %x\n", fmt.pad);

	/* 31.setting format */
	printf("15.setting format V4L2_PIX_FMT_UYVY\n");
	CLEAR(v4l2_fmt);
	v4l2_fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	v4l2_fmt.fmt.pix.width = out_width;
	v4l2_fmt.fmt.pix.height = out_height;
	v4l2_fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_UYVY;
	v4l2_fmt.fmt.pix.field = V4L2_FIELD_INTERLACED;

	if (-1 == ioctl(capt_fd, VIDIOC_S_FMT, &v4l2_fmt)) {
		printf("failed to set format on captute device \n");
		goto cleanup;
	} else
		printf("successfully set the format\n");

	/* 32.call G_FMT for knowing picth */
	if (-1 == ioctl(capt_fd, VIDIOC_G_FMT, &v4l2_fmt)) {
		printf("failed to get format from captute device \n");
		goto cleanup;
	} else {
		printf("capture_pitch: %x\n", v4l2_fmt.fmt.pix.bytesperline);
		capture_pitch = v4l2_fmt.fmt.pix.bytesperline;
//		capture_pitch = 176;

	}
	printf("**********************************************\n");

	/* 33.make sure 3 buffers are supported for streaming */

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
	
	/*calling ioctl QUERYBUF and MMAP ON CAPTURE*/
	printf("Querying capture buffers and MMAPing capture\n");
	for(i = 0;  i < cap_numbuffers; i++) {
		cap_buf.index = i;
		cap_buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		cap_buf.memory = V4L2_MEMORY_MMAP;
		ret = ioctl(capt_fd, VIDIOC_QUERYBUF, &cap_buf);
		if(ret < 0) {
			printf("capture QUERYBUF failed\n");
			goto cleanup;
		}
		printf("buffer %d queried \n", i);
		printf("buf length : %d\n",cap_buf.length);
		capture_buffers_prv[i].length = cap_buf.length;
		capture_buffers_prv[i].start = (char*) mmap(NULL, cap_buf.length,
							      PROT_READ | PROT_WRITE,
							      MAP_SHARED, capt_fd,
							      cap_buf.m.offset);
		if((void*)capture_buffers_prv[i].start == (void*) -1) {
			printf("capture MMAP failed, buffer index:%d\n", i);
			goto cleanup;
		}
		printf("Capture Buffer %u mapped at address %p.\n",i,
		       capture_buffers_prv[i].start);
	}
	printf("**********************************************\n");

	/* 19.queue the buffers */
	printf("19. queing buffers\n");
	for (i = 0; i < 3; i++) {
		struct v4l2_buffer buf;
		CLEAR(buf);
		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;
		buf.index = i;
		buf.length = buf_size;

		if (-1 == ioctl(capt_fd, VIDIOC_QBUF, &buf)) {
			printf("call to VIDIOC_QBUF failed\n");
			goto cleanup;
		}
	}

	printf("**********************************************\n");

	printf("configuring display device...\n");
	standard_id =	V4L2_STD_NTSC;
	display_fd = open(DISPLAY_DEVICE, O_RDWR);
	if(display_fd < 0) {
		printf("failed to open display device \n");
		exit(0);
	}
		printf("setting COMPOSITE output. . .\n");
		bzero(&output, sizeof(struct v4l2_output));
		output.index = 0;
		if (-1 == ioctl (display_fd, VIDIOC_S_OUTPUT, &output.index)) {
			printf("failed to set COMPOSITE with display device\n");
		} else
			printf("successfully set COMPOSITE display\n");
	
		/* Set Standard for display */
		printf("setting std on display device\n");
		ret = ioctl(display_fd, VIDIOC_S_STD, &standard_id);
		if(ret < 0) {
			printf("display S_STD failed\n");
			goto cleanup;
		}
		printf("successfully set std on display device\n");
	printf("setting format on display display device\n");
	bzero((void *)&format,sizeof( format));
	format.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	format.fmt.pix.width = out_width;
	format.fmt.pix.height = out_height;
	format.fmt.pix.pixelformat = V4L2_PIX_FMT_UYVY;
	ret = ioctl(display_fd, VIDIOC_S_FMT, &format);
	if(ret) {
		printf("failed to set format on display device\n");
		goto cleanup;
	}

	printf("display S_FMT successful\n");
	printf("size image : %d\n",format.fmt.pix.sizeimage);
	printf("**********************************************\n");

	printf("getting the format from display device\n");
	bzero((void *)&format,sizeof( format));
	format.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	ret = ioctl(display_fd, VIDIOC_G_FMT, &format);
	if(ret < 0) {
		printf("failed to get format on dispaly device\n");
		goto cleanup;
	} else {
		
		display_pitch = format.fmt.pix.bytesperline;
		printf("successfully got format from display device\n");
		printf("w:%d\th:%d\tdisplay pitch = %d\n", format.fmt.pix.width, format.fmt.pix.height, display_pitch);
	}
	printf("**********************************************\n");

	printf("requesting for %d buffers on display device\n", cap_numbuffers);
	out_reqbuff.count = cap_numbuffers;
	out_reqbuff.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	out_reqbuff.memory = V4L2_MEMORY_MMAP;

	ret = ioctl(display_fd, VIDIOC_REQBUFS, &out_reqbuff);
	if(ret < 0) {
		printf("requesting for %d buffers on display device failed\n", cap_numbuffers);
		goto cleanup;
	}

	if (cap_numbuffers != out_reqbuff.count) {
		printf("%d buffers not supported by display device\n", cap_numbuffers);
		printf("%d buffers are supported by display device\n", out_reqbuff.count);
	}else
		printf("requesting for %d buffers on display device is successful\n", cap_numbuffers);

	disp_numbuffers = out_reqbuff.count;
	printf("**********************************************\n");

	/*calling ioctl QUERYBUF and MMAP ON display device*/
	printf("querying display buffers and MMAPing\n");
	for(i = 0;  i < disp_numbuffers; i++) {
		disp_qbuf.index = i;
		disp_qbuf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
		disp_qbuf.memory = V4L2_MEMORY_MMAP;
		ret = ioctl(display_fd, VIDIOC_QUERYBUF, &disp_qbuf);
		if(ret < 0) {
			printf("display QUERYBUF failed\n");
			goto cleanup;
		}
		printf("buffer %d queried \n", i);
		printf("buf length : %d\n",disp_qbuf.length);
		display_buffers_prv[i].length = disp_qbuf.length;
		display_buffers_prv[i].start = (char*) mmap(NULL, disp_qbuf.length,
							      PROT_READ | PROT_WRITE,
							      MAP_SHARED, display_fd,
							      disp_qbuf.m.offset);
		if((void*)display_buffers_prv[i].start == (void*) -1) {
			printf("display MMAP failed, buffer index:%d\n", i);
			goto cleanup;
		}
		printf("display Buffer %u mapped at address %p.\n",i,
		       display_buffers_prv[i].start);
	}
	printf("**********************************************\n");

	/* 18.queue the display buffers */
	for(disp_buf_index= 0; disp_buf_index < disp_numbuffers; disp_buf_index++) {
		CLEAR(disp_qbuf);
		disp_qbuf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
		disp_qbuf.memory = V4L2_MEMORY_MMAP;
		disp_qbuf.index = disp_buf_index;
		disp_qbuf.length = buf_size;
		if (-1 == ioctl(display_fd, VIDIOC_QBUF, &disp_qbuf)) {
			printf("failed to q buffer on display device\n");
			goto cleanup;
		}
	}

	/* start streaming on display device */
	CLEAR(type);
	type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	ret = ioctl(display_fd, VIDIOC_STREAMON, &type);
	if(ret < 0) {
		printf("failed to start streaming on display device\n");
		exit(0);
	}
		printf("streaming started successfully on display device\n");


	/* 20.start streaming */
	CLEAR(type);
	type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if (-1 == ioctl(capt_fd, VIDIOC_STREAMON, &type)) {
		printf("failed to start streaming on capture device");
		goto cleanup;
	} else
		printf("streaming started successfully\n");

	/* 21.get frames from capture device and store in a file */
	frame_count = 0;

	while(frame_count <50) {
		frame_count++;

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
			printf("failed to DQ buffer from display device\n");
			goto cleanup;
		}

		src = capture_buffers_prv[cap_buf.index].start;
		dst = display_buffers_prv[disp_qbuf.index].start;

		/* copy frame to a file */
		for(i=0 ; i < out_height; i++) {
			memcpy(dst, src, display_pitch);
			src += capture_pitch;
			dst += display_pitch;
		}

		cap_buf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
		cap_buf.memory = V4L2_MEMORY_MMAP;
		ret = ioctl(display_fd, VIDIOC_QBUF, &cap_buf);
		if(ret < 0) {
			printf("failed to Q buffer onto display device\n");
			goto cleanup;
		}

		/* Q the buffer for capture, again */
		disp_qbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		disp_qbuf.memory = V4L2_MEMORY_MMAP;
		ret = ioctl(capt_fd, VIDIOC_QBUF, &disp_qbuf);
		if (ret < 0) {
			printf("failed to Q buffer onto capture device\n");
			goto cleanup;
		}

		if (frame_count == 0)
			prev_ts = (cap_buf.timestamp.tv_sec*1000000) + cap_buf.timestamp.tv_usec;
		else {
			curr_ts = (cap_buf.timestamp.tv_sec*1000000) + cap_buf.timestamp.tv_usec;
			fp_period = curr_ts - prev_ts;
			if (frame_count == 1) {
				fp_period_max = fp_period_min = fp_period_average = fp_period;
			}
			else {
				/* calculate jitters and average */
				if (fp_period > fp_period_max)
					fp_period_max = fp_period;
				if (fp_period < fp_period_min)
					fp_period_min = fp_period;

				fp_period_average =
					((fp_period_average * frame_count) +
					fp_period)/(frame_count + 1);
			}
			prev_ts = curr_ts;
		}

		frame_count++;

		printf("frame:%5u, ", frame_count);
		printf("buf.timestamp:%lu:%lu\n",
			cap_buf.timestamp.tv_sec, cap_buf.timestamp.tv_usec);
	}

	printf("**********************************************\n");

	/* 21. do stream off */
	CLEAR(type);
	type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if (-1 == ioctl(capt_fd, VIDIOC_STREAMOFF, &type)) {
		printf("failed to stop streaming on capture device");
		goto cleanup;
	} else
		printf("streaming stopped successfully\n");

	CLEAR(type);
	type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	ret = ioctl(display_fd, VIDIOC_STREAMOFF, &type);
	if(ret < 0) {
		printf("failed to stop streaming on display device");
		goto cleanup;
	}
	printf("streaming stopped successfully on display device\n");

	printf("**********************************************\n");

cleanup:
	/* unmapping mmapped buffers */
	for(i=0;i<cap_numbuffers;i++)
		munmap(capture_buffers_prv[i].start, capture_buffers_prv[i].length);

	printf("unmapped capture buffers\n");

	for(i=0;i<disp_numbuffers;i++)
		munmap(display_buffers_prv[i].start, display_buffers_prv[i].length);

	printf("unmapped display buffers\n");

	printf("**********************************************\n");

	/* 24. de-enable all the links which are active right now */
	for(index = 0; index < entities_count; index++) {

		links.entity = entity[index].id;

		links.pads = malloc(sizeof( struct media_pad_desc) * entity[index].pads);
		links.links = malloc(sizeof(struct media_link_desc) * entity[index].links);

		ret = ioctl(media.media_fd, MEDIA_IOC_ENUM_LINKS, &links);
		if (ret < 0) {
			if (errno == EINVAL)
				break;
		}else{

			for(i = 0;i< entity[index].links; i++)
			{
				       if(links.links->flags & MEDIA_LNK_FL_ENABLED) {
					        /* de-enable the link */
					        memset(&link, 0, sizeof(link));

						link.flags |=  ~MEDIA_LNK_FL_ENABLED;
						link.source.entity = links.links->source.entity;
						link.source.index = links.links->source.index;
						link.source.flags = MEDIA_PAD_FL_OUTPUT;

						link.sink.entity = links.links->sink.entity;
						link.sink.index = links.links->sink.index;
						link.sink.flags = MEDIA_PAD_FL_INPUT;

						ret = ioctl(media.media_fd, MEDIA_IOC_SETUP_LINK, &link);
						if(ret) {
							printf("failed to de-enable link \n");
						}

				       }

				links.links++;
			}
		}
	//}
	}

	/* 42.close all the file descriptors */
//	printf("closing all the file descriptors. . .\n");
	if(rsz_fd) {
		close(rsz_fd);
		printf("closed reszier sub-device\n");
	}
	if(prv_fd) {
		close(prv_fd);
		printf("closed previewer sub-device\n");
	}
	if(ccdc_fd) {
		close(ccdc_fd);
		printf("closed ccdc sub-device\n");
	}
	if(tvp_fd) {
		close(tvp_fd);
		printf("closed tvp514x sub-device\n");
	}
	if(capt_fd) {
		close(capt_fd);
		printf("closed capture device\n");
	}
	if(display_fd) {
		close(display_fd);
		printf("closed display device\n");
	}

	media_device_close(media.media_fd);
	return ret;
}
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
 *       [TVP514x]--->[CCDC]--->[PREVIEWER]--->[PRV Video out node]
 *       media pipeline will be set and frames captured.
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/mman.h>
#include <getopt.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <linux/version.h>
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,38)
#include <linux/videodev.h>
#endif
#include <linux/videodev2.h>
#include <media/davinci/imp_previewer.h>
#include <media/davinci/dm365_ipipe.h>
#include <media/davinci/vpfe_capture.h>
#include <linux/media.h>
#include <linux/v4l2-subdev.h>
#include <media/davinci/videohd.h> //NAG
/*******************************************************************************/
#include "common.h"

#define ALIGN(x, y)	(((x + (y-1))/y)*y)
#define CLEAR(x) memset (&(x), 0, sizeof (x))
#define APP_NUM_BUFS	3

/* structure to hold address */
struct mmap_buffer {
	void *start;
	size_t length;
};

struct mmap_buffer	capture_buffers_prv[APP_NUM_BUFS];

struct media_entity_desc	entity[15];
struct media_links_enum		links;
int				entities_count;

#define CODE		V4L2_MBUS_FMT_YUYV8_2X8

int width = 720, height = 480;
int buf_size = ALIGN((720*480*2), 4096);

//int main(int argc, char *argp[])
int prv()
{
	int i, index, media_fd = 0, tvp_fd = 0, ccdc_fd = 0, prv_fd = 0, capt_fd = 0, ret, capture_pitch, frame_count,disp_numbuffers;
	struct media_link_desc link;
	struct v4l2_subdev_format fmt;
	struct v4l2_input input;
	struct v4l2_format v4l2_fmt;
	struct v4l2_requestbuffers req;
	enum v4l2_buf_type type;
//	char *source;
	struct v4l2_buffer cap_buf;
	struct prev_channel_config prev_chan_config;
	struct prev_continuous_config prev_cont_config;
	int display_fd = 0;
	int disp_buf_index = 0;
	int display_pitch = 0;
	struct v4l2_output output;
	int output_type = 0;
	struct v4l2_format format;
	struct v4l2_requestbuffers out_reqbuff;
	struct v4l2_buffer disp_qbuf;
	int buf_size;
	v4l2_std_id cur_std;
	int E_VIDEO;
	int E_TVP514X;
	int E_CCDC;
	int E_PRV;
	int cap_numbuffers = 0;
//	struct v4l2_buffer in_querybuf;
	char *src, *dst;

	FILE* file = (FILE*) malloc(sizeof(FILE*));	

	/* 2.allocate buffers using cmem */
	v4l2_std_id standard_id = V4L2_STD_NTSC;
/*
	if (allocate_user_buffers(ALIGN((width*height*2), 4096)) < 0) {
		printf("Unable to Allocate user buffers\n");
		goto cleanup;
	}
*/
	/* 3.open media device */

	media_fd = open("/dev/media0", O_RDWR);
	if (media_fd < 0) {
		printf("%s: Can't open media device %s\n", __func__, "/dev/media0");
		goto cleanup;
	}


	/* 4.enumerate media-entities */
	printf("4.enumerating media entities\n");

	index = 0;
	do {
		memset(&entity[index], 0, sizeof(entity));
		entity[index].id = index | MEDIA_ENT_ID_FLAG_NEXT;

		ret = ioctl(media_fd, MEDIA_IOC_ENUM_ENTITIES, &entity[index]);
		if (ret < 0) {
			if (errno == EINVAL)
				break;
		}else {
			if (!strcmp(entity[index].name, E_VIDEO_PRV_OUT_NAME)) {
				E_VIDEO =  entity[index].id;
			}
			else if (!strcmp(entity[index].name, E_TVP514X_NAME)) {
				E_TVP514X =  entity[index].id;
			}
			else if (!strcmp(entity[index].name, E_CCDC_NAME)) {
				E_CCDC =  entity[index].id;
			}
			else if (!strcmp(entity[index].name, E_PRV_NAME)) {
				E_PRV =  entity[index].id;
			}
			printf("[%x]:%s\n", entity[index].id, entity[index].name);
		}

		index++;
	}while (ret == 0);
	entities_count = index;
	printf("total number of entities: %x\n", entities_count);

	printf("**********************************************\n");

	/* 5.enumerate links for each entity */
	printf("5.enumerating links/pads for entities\n");
	for(index = 0; index < entities_count; index++) {

		links.entity = entity[index].id;

		links.pads = malloc(sizeof( struct media_pad_desc) * entity[index].pads);
		links.links = malloc(sizeof(struct media_link_desc) * entity[index].links);

		ret = ioctl(media_fd, MEDIA_IOC_ENUM_LINKS, &links);
		if (ret < 0) {
			if (errno == EINVAL)
				break;
		}else{
			/* display pads info first */
			if(entity[index].pads)
				printf("pads for entity %x=", entity[index].id);

			for(i = 0;i< entity[index].pads; i++)
			{
				printf("(%x, %s) ", links.pads->index,(links.pads->flags & MEDIA_PAD_FL_INPUT)?"INPUT":"OUTPUT");
				links.pads++;
			}

			printf("\n");

			/* display links now */
			for(i = 0;i< entity[index].links; i++)
			{
				printf("[%x:%x]-------------->[%x:%x]",links.links->source.entity,
				       links.links->source.index,links.links->sink.entity,links.links->sink.index);
				       if(links.links->flags & MEDIA_LNK_FL_ENABLED)
						printf("\tACTIVE\n");
				       else
						printf("\tINACTIVE \n");

				links.links++;
			}

			printf("\n");
		}
	}

	printf("**********************************************\n");

	for(index = 0; index < entities_count; index++) {

		links.entity = entity[index].id;

		links.pads = malloc(sizeof( struct media_pad_desc) * entity[index].pads);
		links.links = malloc(sizeof(struct media_link_desc) * entity[index].links);

		ret = ioctl(media_fd, MEDIA_IOC_ENUM_LINKS, &links);
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

						ret = ioctl(media_fd, MEDIA_IOC_SETUP_LINK, &link);
						if(ret) {
							printf("failed to de-enable link \n");
						}
				       }

				links.links++;
			}
		}
	}
	printf("**********************************************\n");
	/* 6. enable 'tvp514x-->ccdc' link */

	printf("6. ENABLEing link [tvp514x]----------->[ccdc]\n");
	memset(&link, 0, sizeof(link));

	link.flags |=  MEDIA_LNK_FL_ENABLED;
	link.source.entity = E_TVP514X;
	link.source.index = P_TVP514X;
	link.source.flags = MEDIA_PAD_FL_OUTPUT;

	link.sink.entity = E_CCDC;
	link.sink.index = P_CCDC_SINK;
	link.sink.flags = MEDIA_PAD_FL_INPUT;

	ret = ioctl(media_fd, MEDIA_IOC_SETUP_LINK, &link);
	if(ret) {
		printf("failed to enable link between tvp514x and ccdc\n");
		goto cleanup;
	} else
		printf("[tvp514x]----------->[ccdc]\tENABLED\n");

	/* 7.enable 'ccdc-->prv' link */
	printf("7. ENABLEing link [CCDC]----------->[PRV]\n");
	memset(&link, 0, sizeof(link));

	link.flags |=  MEDIA_LNK_FL_ENABLED;
	link.source.entity = E_CCDC;
	link.source.index = P_CCDC_SOURCE;
	link.source.flags = MEDIA_PAD_FL_OUTPUT;

	link.sink.entity = E_PRV;
	link.sink.index = P_PRV_SINK;
	link.sink.flags = MEDIA_PAD_FL_INPUT;

	ret = ioctl(media_fd, MEDIA_IOC_SETUP_LINK, &link);
	if(ret) {
		printf("failed to enable link between ccdc and previewer\n");
		goto cleanup;
	} else
		printf("[ccdc]----------->[prv]\tENABLED\n");

	/* 8. enable 'prv->memory' link */
	printf("8. ENABLEing link [prv]----------->[video_node]\n");
	memset(&link, 0, sizeof(link));

	link.flags |=  MEDIA_LNK_FL_ENABLED;
	link.source.entity = E_PRV;
	link.source.index = P_PRV_SOURCE;
	link.source.flags = MEDIA_PAD_FL_OUTPUT;

	link.sink.entity = E_VIDEO;
	link.sink.index = P_VIDEO;
	link.sink.flags = MEDIA_PAD_FL_INPUT;

	ret = ioctl(media_fd, MEDIA_IOC_SETUP_LINK, &link);
	if(ret) {
		printf("failed to enable link between prv and video node\n");
		goto cleanup;
	} else
		printf("[prv]----------->[video_node]\t ENABLED\n");

	printf("**********************************************\n");

	/* 12.open prv device */
	prv_fd = open("/dev/v4l-subdev3", O_RDWR);
	if(prv_fd == -1) {
		printf("failed to open %s\n", "/dev/v4l-subdev3");
		goto cleanup;
	}

	/* 13. set default configuration in PRV */
	//prev_chan_config.oper_mode = IMP_MODE_CONTINUOUS;
	prev_chan_config.len = 0;
	prev_chan_config.config = NULL; /* to set defaults in driver */
	if (ioctl(prv_fd, PREV_S_CONFIG, &prev_chan_config) < 0) {
		perror("failed to set default configuration on prv\n");
		goto cleanup;
	} else
		printf("default configuration is set successfully on prv\n");

	/* 14 get configuration from prv */
	//prev_chan_config.oper_mode = IMP_MODE_CONTINUOUS;
	prev_chan_config.len = sizeof(struct prev_continuous_config);
	prev_chan_config.config = &prev_cont_config;

	if (ioctl(prv_fd, PREV_G_CONFIG, &prev_chan_config) < 0) {
		perror("failed to get default configuration from prv\n");
		goto cleanup;
	} else
		printf("got configuration from prv successfully\n");

	/* 15. set configuration in prv */
	//prev_chan_config.oper_mode = IMP_MODE_CONTINUOUS;
	prev_chan_config.len = sizeof(struct prev_continuous_config);
	prev_chan_config.config = &prev_cont_config;

	if (ioctl(prv_fd, PREV_S_CONFIG, &prev_chan_config) < 0) {
		perror("failed to set configuration on prv\n");
		goto cleanup;
	} else
		printf("configuration is set successfully on prv\n");

	/* 19.open capture device */

	if ((capt_fd = open("/dev/video2", O_RDWR | O_NONBLOCK, 0)) <= -1) {
		printf("failed to open %s \n", "/dev/video2");
		goto cleanup;
	}

	/* 20.enumerate inputs supported by capture via tvp514x[wh s active through mc] */
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

	/* 21.setting COMPOSITE input */
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

	/* 9. set format on pad of tvp514x */
	tvp_fd = open("/dev/v4l-subdev0", O_RDWR);
	if(tvp_fd == -1) {
		printf("failed to open %s\n", "/dev/v4l-subdev1");
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


	/* 10. set format on sink-pad of ccdc */

	ccdc_fd = open("/dev/v4l-subdev2", O_RDWR);
	if(ccdc_fd == -1) {
		printf("failed to open %s\n", "/dev/v4l-subdev2");
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

	/* 11. set format on OF-pad of ccdc */
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



	/* 17. set format on sink-pad of prv */
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

	/* 18. set format on source-pad of ccdc */
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


	/* 23.setting format */
	printf("15.setting format V4L2_PIX_FMT_UYVY\n");
	CLEAR(v4l2_fmt);
	v4l2_fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	v4l2_fmt.fmt.pix.width = width;
	v4l2_fmt.fmt.pix.height = height;
	v4l2_fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_UYVY;
	v4l2_fmt.fmt.pix.field = V4L2_FIELD_INTERLACED;

	if (-1 == ioctl(capt_fd, VIDIOC_S_FMT, &v4l2_fmt)) {
		printf("failed to set format on captute device \n");
		goto cleanup;
	} else
		printf("successfully set the format\n");

	/* 24.call G_FMT for knowing picth */
	if (-1 == ioctl(capt_fd, VIDIOC_G_FMT, &v4l2_fmt)) {
		printf("failed to get format from capture device \n");
		goto cleanup;
	} else {
		printf("capture_pitch: %x\n", v4l2_fmt.fmt.pix.bytesperline);
		capture_pitch = v4l2_fmt.fmt.pix.bytesperline;
	}
	printf("**********************************************\n");

	/* 25.make sure 3 buffers are supported for streaming */

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

	printf("confiuring display device...\n");

	display_fd = open("/dev/video5", O_RDWR);
	if(display_fd < 0) {
		printf("failed to open display device \n");
		exit(0);
	}


		/* 13.setting COMPOSITE output */
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
	

	printf("**********************************************\n");

	printf("setting format on display display device\n");
	bzero((void *)&format,sizeof( format));
	format.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	format.fmt.pix.width = width;
	format.fmt.pix.height = height;
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
		display_buffers[i].length = disp_qbuf.length;
		display_buffers[i].start = (char*) mmap(NULL, disp_qbuf.length,
							      PROT_READ | PROT_WRITE,
							      MAP_SHARED, display_fd,
							      disp_qbuf.m.offset);
		if((void*)display_buffers[i].start == (void*) -1) {
			printf("display MMAP failed, buffer index:%d\n", i);
			goto cleanup;
		}
		printf("display Buffer %u mapped at address %p.\n",i,
		       display_buffers[i].start);
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

	/* 27.open file in which frames should be saved */

	file = fopen("tvp514x_ccdc_prv_output.yuv", "wa+");
        if (file == NULL) {
		perror("could not open the file to write");
		goto cleanup;
	}

	/* 28.get 20 frames from capture device and store in a file */
	frame_count = 0;

	while(frame_count < 200) {
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
		dst = display_buffers[disp_qbuf.index].start;

		/* copy frame to a file */
		for(i=0 ; i < height; i++) {
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
		printf("frame #%d done\n", frame_count);
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
		munmap(display_buffers[i].start, display_buffers[i].length);

	printf("unmapped display buffers\n");

	printf("**********************************************\n");

	/* 24. de-enable all the links which are active right now */
	for(index = 0; index < entities_count; index++) {

		links.entity = entity[index].id;

		links.pads = malloc(sizeof( struct media_pad_desc) * entity[index].pads);
		links.links = malloc(sizeof(struct media_link_desc) * entity[index].links);

		ret = ioctl(media_fd, MEDIA_IOC_ENUM_LINKS, &links);
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

						ret = ioctl(media_fd, MEDIA_IOC_SETUP_LINK, &link);
						if(ret) {
							printf("failed to de-enable link \n");
						}

				       }

				links.links++;
			}
		}
	}

	/* 33.close all the file descriptors */
	printf("closing all the file descriptors. . .\n");
	if(capt_fd) {
		close(capt_fd);
		printf("closed capture device\n");
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

	if(media_fd) {
		close(media_fd);
		printf("closed  media device\n");
	}
	if(display_fd) {
		close(display_fd);
		printf("closed display device\n");
	}
	return ret;
}

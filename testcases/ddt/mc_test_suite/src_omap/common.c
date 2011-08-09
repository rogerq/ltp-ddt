#include "common.h"
#if defined (CONFIG_OMAP3530)

int timeval_subtract(struct timeval *result, struct timeval *x,
                                     struct timeval *y)
{
        // Perform the carry for the later subtraction by updating y.
        
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

        // Compute the time remaining to wait.     tv_usec is certainly positive.
        result->tv_sec = x->tv_sec - y->tv_sec;
        result->tv_usec = x->tv_usec - y->tv_usec;

        // Return 1 if result is negative.
        return x->tv_sec < y->tv_sec;
}


int media_device_open(struct media_dev *media)
{
	/* Open the Media device */
	media->media_fd = open((const char *) MEDIA_DEVICE, O_RDWR);
	if (media->media_fd  <= 0) {
		printf("Cannot open = %s device\n", MEDIA_DEVICE);
		return -1;
	}
	printf("\n%s: Opened Media Device\n", MEDIA_NAME);
	return 0;
}

void media_device_close(int media_fd)
{
	/* Close the Media device */
	close(media_fd);
}

int enumerate_all_entities(struct media_dev *media)
{
	int ret, index;

	printf("Enumerating media entities\n");
	index = 0;
	do {
		memset(&media->entity[index], 0, sizeof(struct media_entity_desc));
		media->entity[index].id = index | MEDIA_ENTITY_ID_FLAG_NEXT;
		ret = ioctl(media->media_fd, MEDIA_IOC_ENUM_ENTITIES, &media->entity[index]);
		if (ret < 0) {
			break;
		} else {
			if (!strcmp(media->entity[index].name, ENTITY_VIDEO_CCDC_OUT_NAME))
				media->video =  media->entity[index].id;
			else if (!strcmp(media->entity[index].name, ENTITY_TVP514X_NAME))
				media->tvp5146 =  media->entity[index].id;
			else if (!strcmp(media->entity[index].name, ENTITY_MT9T111_NAME))
				media->mt9t111 =  media->entity[index].id;
			else if (!strcmp(media->entity[index].name, ENTITY_CCDC_NAME))
				media->ccdc =  media->entity[index].id;
			printf("[%d]:%s\n", media->entity[index].id, media->entity[index].name);
		}
		index++;
	} while (ret == 0);

	if ((ret < 0) && (index <= 0)) {
		printf("Failed to enumerate entities OR no entity registered - %d\n",
				ret);
		return ret;
	}
	media->num_entities = index;
	printf("Total number of entities: %d\n", media->num_entities);

	return 0;
}


int reset_media_links(struct media_dev *media)
{
	struct media_link_desc link;
	struct media_links_enum links;
	int ret, index, i;

	/* Open the Media device */
	ret  = media_device_open(media);
	if (ret < 0)
		return ret;

	printf("Resetting all links...\n");
	for(index = 0; index < media->num_entities; index++) {
		links.entity = media->entity[index].id;
		links.pads = malloc(sizeof( struct media_pad_desc) * media->entity[index].pads);
		links.links = malloc(sizeof(struct media_link_desc) * media->entity[index].links);
		ret = ioctl(media->media_fd, MEDIA_IOC_ENUM_LINKS, &links);
		if (ret < 0) {
			printf("Error while enumeration links/pads - %d\n", ret);
			break;
		} else {
			for(i = 0; i < media->entity[index].links; i++) {
				link.source.entity = links.links->source.entity;
				link.source.index = links.links->source.index;
				link.source.flags = MEDIA_PAD_FLAG_OUTPUT;
				link.sink.entity = links.links->sink.entity;
				link.sink.index = links.links->sink.index;
				link.sink.flags = MEDIA_PAD_FLAG_INPUT;
				link.flags = (link.flags & ~MEDIA_LINK_FLAG_ENABLED) |
					(link.flags & MEDIA_LINK_FLAG_IMMUTABLE);
				ret = ioctl(media->media_fd, MEDIA_IOC_SETUP_LINK, &link);
				if(ret)
					break;
				links.links++;
			}
		}
	}
	media_device_close(media->media_fd);
	return 0;
}

int setup_media_links(struct media_dev *media)
{
	struct media_link_desc link;
	struct media_links_enum links;
	int ret, index, i, input;

	printf("Enumerating links/pads for entities\n");
	for(index = 0; index < media->num_entities; index++) {
		links.entity = media->entity[index].id;
		links.pads = malloc(sizeof( struct media_pad_desc) * media->entity[index].pads);
		links.links = malloc(sizeof(struct media_link_desc) * media->entity[index].links);
		ret = ioctl(media->media_fd, MEDIA_IOC_ENUM_LINKS, &links);
		if (ret < 0) {
			printf("Error while enumeration links/pads - %d\n", ret);
			break;
		} else {
			if(media->entity[index].pads)
				printf("pads for entity %d=", media->entity[index].id);
			for(i = 0 ; i < media->entity[index].pads; i++) {
				printf("(%d %s) ", links.pads->index,
						(links.pads->flags & MEDIA_PAD_FLAG_INPUT) ?
						"INPUT" : "OUTPUT");
				links.pads++;
			}
			printf("\n");
			for(i = 0; i < media->entity[index].links; i++) {
				printf("[%d:%d]===>[%d:%d]",
						links.links->source.entity,
						links.links->source.index,
						links.links->sink.entity,
						links.links->sink.index);
				if(links.links->flags & MEDIA_LINK_FLAG_ENABLED)
					printf("\tACTIVE\n");
				else
					printf("\tINACTIVE \n");
				links.links++;
			}
			printf("\n");
		}
	}

	if (media->input_source != 0)
		input = media->mt9t111;
	else
		input = media->tvp5146;

	printf("Enabling link [tvp5146]===>[ccdc]\n");
	memset(&link, 0, sizeof(link));

	link.flags |=  MEDIA_LINK_FLAG_ENABLED;
	link.source.entity = input;
	link.source.index = 0;	/* Only 1 pad */
	link.source.flags = MEDIA_PAD_FLAG_OUTPUT;

	link.sink.entity = media->ccdc;
	link.sink.index = 0; /* Sink pad of CCDC, 0 */
	link.sink.flags = MEDIA_PAD_FLAG_INPUT;

	ret = ioctl(media->media_fd, MEDIA_IOC_SETUP_LINK, &link);
	if(ret) {
		printf("failed to enable link between tvp514x and ccdc\n");
		return ret;
	} else {
		printf("[tvp514x]===>[ccdc]\tenabled\n");
	}
	/* Enable 'ccdc===>memory' link */
	printf("Enabling link [ccdc]===>[video_node]\n");
	memset(&link, 0, sizeof(link));

	link.flags |=  MEDIA_LINK_FLAG_ENABLED;
	link.source.entity = media->ccdc;
	link.source.index = 1; /* Source pad of CCDC: 1 */
	link.source.flags = MEDIA_PAD_FLAG_OUTPUT;

	link.sink.entity = media->video;
	link.sink.index = 0;
	link.sink.flags = MEDIA_PAD_FLAG_INPUT;

	ret = ioctl(media->media_fd, MEDIA_IOC_SETUP_LINK, &link);
	if(ret)
		printf("failed to enable link between ccdc and video node\n");
	else
		printf("[ccdc]===>[video_node]\tenabled\n");

	return ret;
}

int set_subdev_format(struct media_dev *media,
				struct capture_dev *capture)
{
	struct v4l2_subdev_format fmt;
	int tvp_fd, ccdc_fd, ret;
	char subdev[20];

	/* TODO: Should be having some mechanism to select subdev */
	ccdc_fd = open("/dev/v4l-subdev2", O_RDWR);
	if(ccdc_fd == -1) {
		printf("failed to open %s\n", "/dev/v4l-subdev2");
		return -1;
	}
	memset(&fmt, 0, sizeof(fmt));
	fmt.pad = 0;
	fmt.which = V4L2_SUBDEV_FORMAT_ACTIVE;
	fmt.format.code = V4L2_MBUS_FMT_UYVY8_2X8;
	fmt.format.width = capture->width;
	fmt.format.height = capture->height;
	fmt.format.colorspace = V4L2_COLORSPACE_SMPTE170M;
	fmt.format.field = V4L2_FIELD_INTERLACED;
	ret = ioctl(ccdc_fd, VIDIOC_SUBDEV_S_FMT, &fmt);
	if(ret) {
		printf("failed to set format on pad %x\n", fmt.pad);
	}

	memset(&fmt, 0, sizeof(fmt));
	fmt.pad = 1;
	fmt.which = V4L2_SUBDEV_FORMAT_ACTIVE;
	fmt.format.code = V4L2_MBUS_FMT_UYVY8_2X8;
	fmt.format.width = capture->width;
	fmt.format.height = capture->height;
	fmt.format.colorspace = V4L2_COLORSPACE_SMPTE170M;
	fmt.format.field = V4L2_FIELD_INTERLACED;
	ret = ioctl(ccdc_fd, VIDIOC_SUBDEV_S_FMT, &fmt);
	if(ret) {
		printf("failed to set format on pad %x\n", fmt.pad);
		return ret;
	}

	/* Default to tvp5146 */
	if (media->input_source != 0)
		strcpy(subdev, "/dev/v4l-subdev9");
	else
		strcpy(subdev, "/dev/v4l-subdev8");

	tvp_fd = open(subdev, O_RDWR);
	if(tvp_fd == -1) {
		printf("failed to open %s\n", subdev);
		return -1;
	}

	memset(&fmt, 0, sizeof(fmt));
	fmt.pad = 0;
	fmt.which = V4L2_SUBDEV_FORMAT_ACTIVE;
	fmt.format.code = V4L2_MBUS_FMT_UYVY8_2X8;
	fmt.format.width = capture->width;
	fmt.format.height = capture->height;
	fmt.format.colorspace = V4L2_COLORSPACE_SMPTE170M;
	fmt.format.field = V4L2_FIELD_INTERLACED;

	ret = ioctl(tvp_fd, VIDIOC_SUBDEV_S_FMT, &fmt);
	if(ret) {
		printf("failed to set format on pad %x\n", fmt.pad);
		return ret;
	}

	printf("successfully format is set on all pad [WxH] - [%dx%d]\n",
			capture->width, capture->height);
	return ret;
}
#endif

int  open_video_dev(const char *dev, int *capture_fd)
{
	/* Open the capture device */
	*capture_fd  = open((const char *) dev, O_RDWR);
	if (*capture_fd  <= 0) {
		printf("Cannot open = %s device\n", dev);
		return -1;
	}
	return 0;
}

void close_video_dev(int capture_fd)
{
	/* close the device */
	close(capture_fd);
}

/*
 * Currently only return width and height.
 */
int get_current_capture_format(int input_src, struct capture_dev *capture)
{
	struct v4l2_input input;
	v4l2_std_id std_id;
	struct v4l2_standard standard;
	int index;

	/* In case of MT9T111 we only support VGA resoltion */
	if (input_src != 0) {
		capture->width = IMG_WIDTH_VGA;
		capture->height = IMG_HEIGHT_VGA;
		return 0;
	}

	/* Get any active input */
	if (ioctl(capture->capture_fd, VIDIOC_G_INPUT, &index) < 0) {
		perror("VIDIOC_G_INPUT");
	}

	/* Enumerate input to get the name of the input detected */
	memset(&input, 0, sizeof(struct v4l2_input));
	input.index = index;
	if (ioctl(capture->capture_fd, VIDIOC_ENUMINPUT, &input) < 0) {
		perror("VIDIOC_ENUMINPUT");
	}

	printf("%s: Current Input: %s\n", CAPTURE_NAME, input.name);

	index = capture->tvp_input;

	if (ioctl(capture->capture_fd, VIDIOC_S_INPUT, &index) < 0) {
		perror("VIDIOC_S_INPUT");
	}
	memset(&input, 0, sizeof(struct v4l2_input));
	input.index = index;
	if (ioctl(capture->capture_fd, VIDIOC_ENUMINPUT, &input) < 0) {
		perror("VIDIOC_ENUMINPUT");
	}
	printf("%s: Input changed to: %s\n", CAPTURE_NAME, input.name);


	/* Detect the standard in the input detected */
	if (ioctl(capture->capture_fd, VIDIOC_QUERYSTD, &std_id) < 0) {
		perror("VIDIOC_QUERYSTD");
	}

	/* Get the standard*/
	if (ioctl(capture->capture_fd, VIDIOC_G_STD, &std_id) < 0) {
		/* Note when VIDIOC_ENUMSTD always returns EINVAL this
		   is no video device or it falls under the USB exception,
		   and VIDIOC_G_STD returning EINVAL is no error. */
		perror("VIDIOC_G_STD");
	}
	memset(&standard, 0, sizeof(standard));
	standard.index = 0;
	while (1) {
		if (ioctl(capture->capture_fd, VIDIOC_ENUMSTD, &standard) < 0) {
			perror("VIDIOC_ENUMSTD");
		}

		/* Store the name of the standard */
		if (standard.id & std_id) {
			printf("%s: Current standard: %s\n",
					CAPTURE_NAME, standard.name);
			break;
		}
		standard.index++;
	}

	capture->width = IMG_WIDTH_PAL_NTSC;
	if (!strcmp((char *)standard.name, "PAL"))
		capture->height = IMG_HEIGHT_PAL;
	else
		capture->height = IMG_HEIGHT_NTSC;

	return 0;
}

/*
 * This function initializes capture device. It selects an active input
 * and detects the standard on that input. It then allocates buffers in the
 * driver's memory space and mmaps them in the application space.
 */
int capture_prepare_streaming(struct capture_dev *capture)
{
	struct v4l2_capability capability;
	struct v4l2_requestbuffers reqbuf;
	struct v4l2_buffer buf;
	struct v4l2_format *fmt = &capture->capture_fmt;
	int ret, i, j;

	/* Check if the device is capable of streaming */
	ret = ioctl(capture->capture_fd, VIDIOC_QUERYCAP, &capability);
	if (ret < 0) {
		perror("VIDIOC_QUERYCAP");
		return ret;
	}
	if (capability.capabilities & V4L2_CAP_STREAMING) {
		printf("%s: Capable of streaming\n", CAPTURE_NAME);
	} else {
		printf("%s: Not capable of streaming\n", CAPTURE_NAME);
		return -1;
	}

	fmt->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	ret = ioctl(capture->capture_fd, VIDIOC_G_FMT, fmt);
	if (ret < 0) {
		perror("VIDIOC_G_FMT");
		return ret;
	}

	fmt->fmt.pix.width = capture->width;
	fmt->fmt.pix.height = capture->height;
	fmt->fmt.pix.pixelformat = DEF_PIX_FMT;

	fmt->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	ret = ioctl(capture->capture_fd, VIDIOC_S_FMT, fmt);
	if (ret < 0) {
		perror("VIDIOC_S_FMT");
		return ret;
	}

	ret = ioctl(capture->capture_fd, VIDIOC_G_FMT, fmt);
	if (ret < 0) {
		perror("VIDIOC_G_FMT");
		return ret;
	}

	if (fmt->fmt.pix.pixelformat != DEF_PIX_FMT) {
		printf("%s: Requested pixel format not supported\n", CAPTURE_NAME);
		return -1;
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
	reqbuf.count = capture->num_bufs;
	reqbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	reqbuf.memory = V4L2_MEMORY_MMAP;
	ret = ioctl(capture->capture_fd, VIDIOC_REQBUFS, &reqbuf);
	if (ret < 0) {
		perror("Cannot allocate memory");
		return ret;
	}
	/* Store the number of buffers actually allocated */
	capture->num_bufs = reqbuf.count;
	printf("%s: Number of requested buffers = %d\n", CAPTURE_NAME,
			capture->num_bufs);

	memset(&buf, 0, sizeof(buf));

	/* Mmap the buffers
	 * To access driver allocated buffer in application space, they have
	 * to be mmapped in the application space using mmap system call */
	for (i = 0; i < (capture->num_bufs); i++) {
		buf.type = reqbuf.type;
		buf.index = i;
		buf.memory = reqbuf.memory;
		ret = ioctl(capture->capture_fd, VIDIOC_QUERYBUF, &buf);
		if (ret < 0) {
			perror("VIDIOC_QUERYCAP");
			capture->num_bufs = i;
			goto ERROR;
		}


		capture_buff_info[i].length = buf.length;
		capture_buff_info[i].index = i;
		capture_buff_info[i].start = mmap(NULL, buf.length,
				PROT_READ | PROT_WRITE, MAP_SHARED, capture->capture_fd,
				buf.m.offset);

		if (capture_buff_info[i].start == MAP_FAILED) {
			printf("Cannot mmap = %d buffer\n", i);
			capture->num_bufs = i;
			goto ERROR;
		}

		memset((void *) capture_buff_info[i].start, 0x80,
				capture_buff_info[i].length);
		/* Enqueue buffers
		 * Before starting streaming, all the buffers needs to be
		 * en-queued in the driver incoming queue. These buffers will
		 * be used by thedrive for storing captured frames. */
		ret = ioctl(capture->capture_fd, VIDIOC_QBUF, &buf);
		if (ret < 0) {
			perror("VIDIOC_QBUF");
			capture->num_bufs = i + 1;
			goto ERROR;
		}
	}

	printf("%s: Init done successfully\n\n", CAPTURE_NAME);
	return 0;

ERROR:
	for (j = 0; j < capture->num_bufs; j++)
		munmap(capture_buff_info[j].start,
				capture_buff_info[j].length);
	return -1;
}

/*
 * This function initializes memory allocator device. It sets output and standard for
 * LCD. These output and standard are same as those detected in capture device.
 * It, then, allocates buffers in the driver's memory space and mmaps them in
 * the application space
 */
int capture_display_prepare_buffers(struct mem_allocator_dev *mem_allocator)
{
	int ret, i, j;
	struct v4l2_requestbuffers reqbuf;
	struct v4l2_buffer buf;
	struct v4l2_format *fmt = &mem_allocator->display_fmt;
	unsigned int buf_length;

	/* Open the mem allocator device */
	mem_allocator->buffer_fd = open((const char *) VIDEO2_DEVICE, O_RDWR);
	if (mem_allocator->buffer_fd <= 0) {
		printf("Cannot open = %s device\n", VIDEO2_DEVICE);
		return -1;
	}
	printf("\n%s: Opened Channel\n", VIDEO2_NAME);

	/* Get the format */
	fmt->type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	ret = ioctl(mem_allocator->buffer_fd, VIDIOC_G_FMT, fmt);
	if (ret < 0) {
		perror("VIDIOC_G_FMT");
		goto ERROR;
	}

	fmt->fmt.pix.width = mem_allocator->width;
	fmt->fmt.pix.height = mem_allocator->height;
	fmt->fmt.pix.pixelformat = DEF_PIX_FMT;
	fmt->type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	ret = ioctl(mem_allocator->buffer_fd, VIDIOC_S_FMT, fmt);
	if (ret < 0) {
		perror("VIDIOC_S_FMT");
		goto ERROR;
	}

	ret = ioctl(mem_allocator->buffer_fd, VIDIOC_G_FMT, fmt);
	if (ret < 0) {
		perror("VIDIOC_G_FMT");
		goto ERROR;
	}

	buf_length = fmt->fmt.pix.width * fmt->fmt.pix.height * 2;
	buf_length = (buf_length + 4096) & (~0xFFF);

	if (fmt->fmt.pix.pixelformat != DEF_PIX_FMT) {
		printf("%s: Requested pixel format not supported\n",
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
	reqbuf.count = mem_allocator->num_bufs;
	reqbuf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	reqbuf.memory = V4L2_MEMORY_MMAP;
	ret = ioctl(mem_allocator->buffer_fd, VIDIOC_REQBUFS, &reqbuf);
	if (ret < 0) {
		perror("Cannot allocate memory");
		goto ERROR;
	}
	/* Store the numbfer of buffers allocated */
	mem_allocator->num_bufs = reqbuf.count;
	printf("%s: Number of requested buffers = %d\n", DISPLAY_NAME,
	       mem_allocator->num_bufs);

	memset(&buf, 0, sizeof(buf));

	/* Mmap the buffers
	 * To access driver allocated buffer in application space, they have
	 * to be mmapped in the application space using mmap system call */
	for (i = 0; i < mem_allocator->num_bufs; i++) {
		/* Query physical address of the buffers */
		buf.type = reqbuf.type;
		buf.index = i;
		buf.memory = reqbuf.memory;
		ret = ioctl(mem_allocator->buffer_fd, VIDIOC_QUERYBUF, &buf);
		if (ret < 0) {
			perror("VIDIOC_QUERYCAP");
			mem_allocator->num_bufs = i;
			goto ERROR1;
		}

		/* Mmap the buffers in application space */
		video2_buff_info[i].length = buf_length;
		video2_buff_info[i].index =  i;
		video2_buff_info[i].start = mmap(NULL, buf.length,
				PROT_READ | PROT_WRITE, MAP_SHARED, mem_allocator->buffer_fd,
				buf.m.offset);
		if (video2_buff_info[i].start == MAP_FAILED) {
			printf("Cannot mmap = %d buffer\n", i);
			mem_allocator->num_bufs = i;
			goto ERROR1;
		}
		memset((void *) video2_buff_info[i].start, 0x80,
		       video2_buff_info[i].length);

	}
	printf("%s: Init done successfully\n\n", DISPLAY_NAME);
	return 0;

ERROR1:
	for (j = 0; j < mem_allocator->num_bufs; j++)
		munmap(display_buff_info[j].start,
			display_buff_info[j].length);
ERROR:
	close(mem_allocator->buffer_fd);
	return -1;
}


/*
 * This function initializes display device. It sets output and standard for
 * LCD. These output and standard are same as those detected in capture device.
 * It, then, allocates buffers in the driver's memory space and mmaps them in
 * the application space
 */
int display_prepare_streaming(struct display_dev *display)
{
	int ret, i, j;
	struct v4l2_requestbuffers reqbuf;
	struct v4l2_buffer buf;
	struct v4l2_capability capability;
	struct v4l2_control control;
	struct v4l2_format *fmt = &display->display_fmt;

	/* Open the video display device */
	display->display_fd = open((const char *) DISPLAY_DEVICE, O_RDWR);
	if (display->display_fd <= 0) {
		printf("Cannot open = %s device\n", DISPLAY_DEVICE);
		return -1;
	}
	printf("\n%s: Opened Channel\n", DISPLAY_NAME);

	/* Check if the device is capable of streaming */
	if (ioctl(display->display_fd, VIDIOC_QUERYCAP, &capability) < 0) {
		perror("VIDIOC_QUERYCAP");
		goto ERROR;
	}

	if (capability.capabilities & V4L2_CAP_STREAMING)
		printf("%s: Capable of streaming\n", DISPLAY_NAME);
	else {
		printf("%s: Not capable of streaming\n", DISPLAY_NAME);
		goto ERROR;
	}

	/* Rotate by 90 degree so that 480x640 resolution will become 640x480 */

        control.id = V4L2_CID_ROTATE;
        control.value = 0;
	ret = ioctl(display->display_fd, VIDIOC_S_CTRL, &control);
	if (ret < 0) {
		perror("VIDIOC_S_CTRL");
		goto ERROR;
	}

	/* Get the format */
	fmt->type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	ret = ioctl(display->display_fd, VIDIOC_G_FMT, fmt);
	if (ret < 0) {
		perror("VIDIOC_G_FMT");
		goto ERROR;
	}

	fmt->fmt.pix.width = display->width;
	fmt->fmt.pix.height = display->height;
	fmt->fmt.pix.pixelformat = DEF_PIX_FMT;
	fmt->type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	ret = ioctl(display->display_fd, VIDIOC_S_FMT, fmt);
	if (ret < 0) {
		perror("VIDIOC_S_FMT");
		goto ERROR;
	}

	ret = ioctl(display->display_fd, VIDIOC_G_FMT, fmt);
	if (ret < 0) {
		perror("VIDIOC_G_FMT");
		goto ERROR;
	}

		if (fmt->fmt.pix.pixelformat != DEF_PIX_FMT) {
			printf("%s: Requested pixel format not supported\n",
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
	reqbuf.count = display->num_bufs;
	reqbuf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	reqbuf.memory = V4L2_MEMORY_MMAP;
	ret = ioctl(display->display_fd, VIDIOC_REQBUFS, &reqbuf);
	if (ret < 0) {
		perror("Cannot allocate memory");
		goto ERROR;
	}
	/* Store the numbfer of buffers allocated */
	display->num_bufs = reqbuf.count;
	printf("%s: Number of requested buffers = %d\n", DISPLAY_NAME,
	       display->num_bufs);

	memset(&buf, 0, sizeof(buf));

	/* Mmap the buffers
	 * To access driver allocated buffer in application space, they have
	 * to be mmapped in the application space using mmap system call */
	for (i = 0; i < display->num_bufs; i++) {
		/* Query physical address of the buffers */
		buf.type = reqbuf.type;
		buf.index = i;
		buf.memory = reqbuf.memory;
		ret = ioctl(display->display_fd, VIDIOC_QUERYBUF, &buf);
		if (ret < 0) {
			perror("VIDIOC_QUERYCAP");
			display->num_bufs = i;
			goto ERROR1;
		}

		/* Mmap the buffers in application space */
		display_buff_info[i].length = buf.length;
		display_buff_info[i].index =  i;
		display_buff_info[i].start = mmap(NULL, buf.length,
				PROT_READ | PROT_WRITE, MAP_SHARED, display->display_fd,
				buf.m.offset);

		if (display_buff_info[i].start == MAP_FAILED) {
			printf("Cannot mmap = %d buffer\n", i);
			display->num_bufs = i;
			goto ERROR1;
		}
		memset((void *) display_buff_info[i].start, 0x80,
		       display_buff_info[i].length);

		/* Enqueue buffers
		 * Before starting streaming, all the buffers needs to be
		 * en-queued in the driver incoming queue. These buffers will
		 * be used by thedrive for storing captured frames. */
		ret = ioctl(display->display_fd, VIDIOC_QBUF, &buf);
		if (ret < 0) {
			perror("VIDIOC_QBUF");
			display->num_bufs = i + 1;
			goto ERROR1;
		}
	}
	printf("%s: Init done successfully\n\n", DISPLAY_NAME);
	return 0;

ERROR1:
	for (j = 0; j < display->num_bufs; j++)
		munmap(display_buff_info[j].start,
			display_buff_info[j].length);
ERROR:
	close(display->display_fd);
	return -1;
}



#include "common.h"
/*
void usage(void)
{
	printf("Usage: saMmapLoopback [-i <tvp input>] [-h help]");
	printf("\t[-i <S-Vid/CVBS>]	: 0: CVBS 1 - S-Video\n");
}
int timeval_subtract(struct timeval *result, struct timeval *x,
		                     struct timeval *y)
{
	// Perform the carry for the later subtraction by updating y.
	 
	if (x->tv_usec < y->tv_usec) {
		int nsec = (y->tv_usec - x->tv_usec) /
			1000000 + 1;
		y->tv_usec -= 1000000 *	nsec;
		y->tv_sec += nsec;
	}
	if (x->tv_usec - y->tv_usec > 1000000) {
		int nsec = (x->tv_usec - y->tv_usec) /
			1000000;
		y->tv_usec += 1000000 * nsec;
		y->tv_sec -= nsec;
	}

	// Compute the time remaining to wait.	   tv_usec is certainly positive.
	result->tv_sec = x->tv_sec - y->tv_sec;
	result->tv_usec = x->tv_usec - y->tv_usec;

	// Return 1 if result is negative. 
	return x->tv_sec < y->tv_sec;
}
*/
//int main(int argc, char *argv[])
int mc_tvp514()
{
	char shortoptions[] = "s:l:n:i:h:";
	int i = 0, ret = 0, a, c, index;
	unsigned int loop_cnt = MAXLOOPCOUNT;
	struct media_dev media;
	struct capture_dev capture;
	struct display_dev display;
	struct timeval before, after, result;

	memset(&media, 0, sizeof(struct media_dev));
	memset(&capture, 0, sizeof(struct capture_dev));
	memset(&display, 0, sizeof(struct display_dev));

	/* Setup default init for all devices */
	/* Media */
	media.input_source = 0;	/* default to tvp5146 */
	/* Capture */
	capture.num_bufs = CAPTURE_MAX_BUFFER;
	/* Display */
	display.num_bufs = DISPLAY_MAX_BUFFER;
/*
	for (;;) {
		c = getopt_long(argc, argv, shortoptions, (void *) NULL,
				&index);
		if (-1 == c)
			break;
		switch (c) {
			case 0:
				break;
			case 's':
			case 'S':
				media.input_source = atoi(optarg);
				break;
			case 'l':
			case 'L':
				loop_cnt = atoi(optarg);
				break;
			case 'n':
			case 'N':
				capture.num_bufs = display.num_bufs = atoi(optarg);
				break;
			case 'i':
			case 'I':
				capture.tvp_input = atoi(optarg);
				break;
			case 'h':
			case 'H':
			default:
				usage();
				exit(1);
		}

	}
*/
	for(i = 0; i < capture.num_bufs; i++) {
		capture_buff_info[i].start = NULL;
	}
	for(i = 0; i < display.num_bufs; i++) {
		display_buff_info[i].start = NULL;
	}
	/*
	 * Initialization section
	 * In case of Media-Controller compliant device: Setup Links
	 * Initialize capture and display devices.
	 * Here one capture channel is opened and input and standard is
	 * detected on that channel.
	 * Display channel is opened with the same standard that is detected at
	 * capture channel.
	 * */
#if defined (CONFIG_OMAP3530)
	/* Open the Media device */
	ret  = media_device_open(&media);
	if (ret < 0)
		return ret;

	ret = enumerate_all_entities(&media);
	if (ret < 0)
		goto err_0;

	/* Setup Links */
	ret = setup_media_links(&media);
	if (ret < 0)
		goto err_0;

	media_device_close(media.media_fd);
#endif
	/* Open the capture device */
	ret = open_video_dev((const char *)CAPTURE_DEVICE, &capture.capture_fd);
	if (ret < 0)
		goto err_1;

	ret = get_current_capture_format(media.input_source, &capture);
	if (ret < 0)
		goto err_1;

	/* Make width/height capture = display*/
	display.width = capture.width;
	display.height = capture.height;

#if defined (CONFIG_OMAP3530)
	/*
	 * Now set the detected format at each pad
	 */
	ret = set_subdev_format(&media, &capture);
	if (ret < 0)
		goto err_2;
#endif
	ret = capture_prepare_streaming(&capture);
	if(ret < 0)
		goto err_3;

	/* open display channel */
	ret = display_prepare_streaming(&display);
	if(ret < 0) {
		printf("Error in opening display device\n");
		goto err_4;
	}

	/* run section
	 * STEP2:
	 * Here display and capture channels are started for streaming. After
	 * this capture device will start capture frames into enqueued
	 * buffers and display device will start displaying buffers from
	 * the qneueued buffers */

	/* Start Streaming. on display device */
	a = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	ret = ioctl(display.display_fd, VIDIOC_STREAMON, &a);
	if (ret < 0) {
		perror("VIDIOC_STREAMON");
		goto err_4;
	}
	printf("%s: Stream on...\n", DISPLAY_NAME);

	/* Start Streaming. on capture device */
	a = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	ret = ioctl(capture.capture_fd, VIDIOC_STREAMON, &a);
	if (ret < 0) {
		perror("VIDIOC_STREAMON");
		goto err_4;
	}
	printf("%s: Stream on...\n", CAPTURE_NAME);

	/* Set the display buffers for queuing and dqueueing operation */
	display.display_buf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	display.display_buf.index = 0;
	display.display_buf.memory = V4L2_MEMORY_MMAP;

	/* Set the capture buffers for queuing and dqueueing operation */
	capture.capture_buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	capture.capture_buf.index = 0;
	capture.capture_buf.memory = V4L2_MEMORY_MMAP;

	/* One buffer is dequeued from display and capture channels.
	 * Capture buffer will be copied to display buffer.
	 * All two buffers are put back to respective channels.
	 * This sequence is repeated in loop.
	 * After completion of this loop, channels are stopped.
	 */
	gettimeofday(&before, NULL);
	for (i = 0; i < loop_cnt; i++) {
		int h;
		unsigned char *cap_ptr, *dis_ptr;
		/* Dequeue display buffer */
		ret = ioctl(display.display_fd, VIDIOC_DQBUF, &display.display_buf);
		if (ret < 0) {
			perror("VIDIOC_DQBUF");
			goto err_4;
		}

		/* Dequeue capture buffer */
		ret = ioctl(capture.capture_fd, VIDIOC_DQBUF, &capture.capture_buf);
		if (ret < 0) {
			perror("VIDIOC_DQBUF");
			goto err_4;
		}

		cap_ptr = (unsigned char*)capture_buff_info[capture.capture_buf.index].start;
		dis_ptr = (unsigned char*)display_buff_info[display.display_buf.index].start;
		for (h = 0; h < display.height; h++) {
			memcpy(dis_ptr, cap_ptr, display.width * 2);
			cap_ptr += capture.width * 2;
			dis_ptr += display.width * 2;
		}

		ret = ioctl(capture.capture_fd, VIDIOC_QBUF, &capture.capture_buf);
		if (ret < 0) {
			perror("VIDIOC_QBUF");
			goto err_4;
		}

		ret = ioctl(display.display_fd, VIDIOC_QBUF, &display.display_buf);
		if (ret < 0) {
			perror("VIDIOC_QBUF");
			goto err_4;
		}
	}
	gettimeofday(&after, NULL);

	a = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	ret = ioctl(display.display_fd, VIDIOC_STREAMOFF, &a);
	if (ret < 0) {
		perror("VIDIOC_STREAMOFF");
		goto err_4;
	}
	printf("\n%s: Stream off!!\n", DISPLAY_NAME);

	a = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	ret = ioctl(capture.capture_fd, VIDIOC_STREAMOFF, &a);
	if (ret < 0) {
		perror("VIDIOC_STREAMOFF");
		goto err_4;
	}
	printf("%s: Stream off!!\n", CAPTURE_NAME);

	printf("\nLoopback Successful\n\n");
	printf("Timing Analysis:\n");
	printf("----------------\n");
	printf("Before Time:\t%lu %lu\n",before.tv_sec, before.tv_usec);
	printf("After Time:\t%lu %lu\n",after.tv_sec, after.tv_usec);
	timeval_subtract(&result, &after, &before);
	printf("Result Time:\t%ld %ld\n",result.tv_sec, result.tv_usec);
	printf("Calculated Frame Rate:\t%ld Fps\n\n", loop_cnt/result.tv_sec);

err_4:
	/* Un-map the buffers */
	for (i = 0; i < display.num_bufs; i++) {
		munmap(display_buff_info[i].start,
		       display_buff_info[i].length);
		display_buff_info[i].start = NULL;
	}

	/* Close the file handle */
	close(display.display_fd);
err_3:
	/* Un-map the buffers */
	for (i = 0; i < capture.num_bufs; i++) {
		munmap(capture_buff_info[i].start,
		       capture_buff_info[i].length);
		capture_buff_info[i].start = NULL;
	}
err_2:
	/* Close the file handle */
	close_video_dev(capture.capture_fd);
err_1:
#if defined (CONFIG_OMAP3530)
	/*TODO: Must reset the media before exiting here */
	reset_media_links(&media);
err_0:
	media_device_close(media.media_fd);
#endif
	return 0;
}

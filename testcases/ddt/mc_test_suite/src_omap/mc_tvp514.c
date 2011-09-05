#include "common.h"
int mc_tvp514()
{
	int i = 0, ret = 0, a;
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

	/*
	 * Now set the detected format at each pad
	 */
	ret = set_subdev_format(&media, &capture);
	if (ret < 0)
		goto err_2;
	
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
	reset_media_links(&media);
err_0:
	media_device_close(media.media_fd);
	return 0;
}

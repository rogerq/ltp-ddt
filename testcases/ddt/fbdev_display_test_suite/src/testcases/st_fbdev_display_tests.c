/*
 * st_fbdev_display_tests.c
 *
 * This file calls FBDEV driver functions to display color bar on the
 * selected output
 *
 * Copyright (C) 2010 Texas Instruments Incorporated - http://www.ti.com/
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

/*
 * Header files
 */
#include <st_fbdev_common.h>

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


/*
 * Function to display image (either from raw file or color bar[default])
 * of given width*height on the selected output
 */
int st_fbdev_display_test(struct fbdev_display_testparams *params)
{

	int i, ret, status = SUCCESS;
	unsigned char* buf_addr;
	unsigned int cpu_load;
	struct proc_stat cpuload;
	struct timeval before, after, result;

	/* Open the device */
	ret = st_fbdev_open_interface(params->device_name, &params->fd);
	if (ret != SUCCESS)
		return ret;

	/* Enable window */
	ret = st_fbdev_blank_interface(params->fd, VESA_NO_BLANKING);
	if (ret) {
		status = FAILURE;
		goto exit;
	}
	TEST_PRINT_TRC("FBIO_BLANK Ioctl passed");

	/* put variable screen info */
	ret = st_fbdev_put_vscreeninfo_interface(params->fd, &params->var);
	if (ret) {
		status = FAILURE;
		goto exit;
	}
	TEST_PRINT_TRC("FBIOPUT_VSCREENINFO Ioctl passed");

	/* get fix screen info */
	ret = st_fbdev_get_fscreeninfo_interface(params->fd, &params->fix);
	if (ret) {
		status = FAILURE;
		goto exit;
	}
	TEST_PRINT_TRC("FBIOGET_FSCREENINFO Ioctl Passed");

	/* map vid0 buffers to user space */
	ret = st_fbdev_mmap_interface(params->fd, params->fix.line_length,
			params->var.yres_virtual, &buf_addr);
	if (ret) {
		status = FAILURE;
		goto exit;
	}
	TEST_PRINT_TRC("mmap Ioctl passed");

	/* If CPU load requested */
	if (params->cpuload == TRUE)
		start_cpuload_measurement(&cpuload);

	if (params->framerate == TRUE)
		gettimeofday(&before, NULL);

	/* Create color bar pattern */
	for (i = 0; i < params->loopcount; i++) {
		fill_color_bar(buf_addr, params->fix.line_length,
				params->var.yres, params->var.bits_per_pixel);
		sleep(1);
		fill_reverse_color_bar(buf_addr, params->fix.line_length,
				params->var.yres, params->var.bits_per_pixel);
		sleep(1);
	}

	/* unmap buffers */
	ret |= st_fbdev_unmap_interface(params->fd, params->fix.line_length,
			params->var.yres_virtual, buf_addr);

	if (params->framerate == TRUE) {
		gettimeofday(&after, NULL);

		calc_result_time(&result, &after, &before);
		TEST_PRINT_TRC("The Fbdev Display : Frame rate = %lu",
					params->loopcount/result.tv_sec);
	}
	/* If CPU load requested */
	if(params->cpuload == TRUE) {
		cpu_load = stop_cpuload_measurement(&cpuload);
		TEST_PRINT_TRC("The Fbdev Display : Percentage CPU Load = %u%%",
					cpu_load);
	}

exit:
	/* close FBDEV device */
	ret |= st_fbdev_close_interface(params->device_name, &params->fd);
	if (ret)
		status = FAILURE;

	/* print status of the test case */
	st_fbdev_display_test_status(status, params->testcase_id);

	/* end test case */
	TEST_PRINT_TST_END(params->testcase_id);

	return ret;
}

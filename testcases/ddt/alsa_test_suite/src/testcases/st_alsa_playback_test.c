/*
 * st_alsa_playback_test.c 
 *
 * This file plays sine wave from the  buffer
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

#include "st_alsa_common.h"
#include <math.h>

/*
 * Function             - st_audio_playback
 * Functionality        - Function to configure the PCM device 
				and perfrom Playback.
 * Input Params         - Device name,stream,mode.
 * Return Value         - Success = 0
 * Note                 - None
 */

int st_audio_playback(tc_dev_params * pcm_param)
{

	long loops;
	long loop_index = 0;
	int rc;
	int size;
	unsigned int total_size = 0;
	char *buffer = NULL;
	short int *write_buff_ptr = NULL;
	struct timeval start_time;
	unsigned long elapsed_usecs = 0;
	double elapsed_secs = 0;
	struct proc_stat cpu_status_id;
	float percentage_cpu_load = 0;

	/*Open and Configure the PCM */
	size = st_audio_open_config_hw_interface(pcm_param);
	if (size == FAILURE) {
		TEST_PRINT_ERR("Unable to open pcm device");
		return FAILURE;
	}

	/*Allocating memory to store the data from audio input */
	buffer = (char *)malloc(size);
	if (NULL == buffer) {
		perror("buffer");
		rc = FAILURE;
		goto close_audio_intfc;
	}
	/*Number of times to loop */
	total_size = pcm_param->total_size;
	loops = total_size / size;
	write_buff_ptr = (short int *)buffer;

	for (loop_index = 0; loop_index < size / 4; loop_index++) {

		float y =
		    sin(2 * M_PI * LEFT_SINE_FREQ * loop_index /
			pcm_param->sampling_rate);
		float x =
		    sin(2 * M_PI * RIGHT_SINE_FREQ * loop_index /
			pcm_param->sampling_rate);

		write_buff_ptr[(2 * loop_index)] = st_scale(y);	/* left channel */
		write_buff_ptr[(2 * loop_index) + 1] = st_scale(x);	/* right channel */

	}

	if (pcm_param->cpuload_flag == TRUE)
		/* Start CPU Load calcaulation */
		start_cpuload_measurement(&cpu_status_id);
	if (pcm_param->throughput_flag == TRUE)
		/* Start the Timer */
		start_timer(&start_time);

	while (loops > 0) {
		loops--;
		rc = st_audio_write(buffer, pcm_param->access_type);
		if (rc == RUN_ERROR) {
			TEST_PRINT_ERR("Underrun Error");
			rc = st_audio_playback_prepare();
			if (rc < 0) {
				TEST_PRINT_ERR("xrun: prepare error");
				rc = FAILURE;
				goto free_mem;
			}
		}
	}
	if (pcm_param->throughput_flag == TRUE) {
		/* Stop the Timer and get the usecs elapsed */
		elapsed_usecs = stop_timer(&start_time);
		elapsed_secs = (double)elapsed_usecs / 1000000u;
		/* Get CPU Load figures */
	}
	if (pcm_param->cpuload_flag == TRUE)
		percentage_cpu_load = stop_cpuload_measurement(&cpu_status_id);

	if (pcm_param->throughput_flag == TRUE) {
		TEST_PRINT_TRC("write: Duration in Sec | %lf", elapsed_secs);
		TEST_PRINT_TRC("write: No. of bits/Sec | %.0lf",
			       ((double)(total_size * 8) / elapsed_secs));
	}
	if (pcm_param->cpuload_flag == TRUE) {
		if ((percentage_cpu_load >= 0) && (percentage_cpu_load <= 100)) {
			TEST_PRINT_TRC("write: percentage cpu load | %.2f%%",
				       percentage_cpu_load);
		}
	}

	st_audio_playback_drain();

free_mem:
	if (NULL != buffer) {
		free(buffer);
	}
close_audio_intfc:
	st_audio_playback_close();

	return rc;

}

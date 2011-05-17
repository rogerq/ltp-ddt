/*
 * st_alsa_file_playback_test.c 
 *
 * This file plays audio data  from the  file
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

/*
 * Function             - st_audio_file_playback
 * Functionality        - Function to configure the PCM device 
				and perfrom Playback from file.
 * Input Params         - Device name,stream,mode.
 * Return Value         - Success = 0
 * Note                 - None
 */

int st_audio_file_playback(tc_dev_params * pcm_param)
{

	long loops;
	int rc = SUCCESS;
	int size;
	char *buffer = NULL;
	int fd_audio = 0;
	unsigned int total_size = 0;
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
		TEST_PRINT_ERR("Unable to allocate memory");
		rc = FAILURE;
		goto close_audio_intfc;
	}

	/*Number of times to loop */
	total_size = pcm_param->total_size;
	loops = total_size / size;

	fd_audio = open((const char *)(pcm_param->file_name), O_RDONLY);
	if (-1 == fd_audio) {
		TEST_PRINT_ERR("file open failed ");
		rc = FAILURE;
		goto free_mem;
	}

	if (pcm_param->cpuload_flag == TRUE)
		/* Start CPU Load calcaulation */
		start_cpuload_measurement(&cpu_status_id);
	if (pcm_param->throughput_flag == TRUE)
		/* Start the Timer */
		start_timer(&start_time);

	while (loops > 0) {
		loops--;
		rc = st_read(fd_audio, buffer, size);
		if (size != rc) {
			TEST_PRINT_ERR("file read failed ");
			rc = FAILURE;
			goto close_file;
		}
		rc = st_audio_write(buffer, pcm_param->access_type);
		if (rc == RUN_ERROR) {
			TEST_PRINT_ERR("Underrun Error");
			rc = st_audio_playback_prepare();
			if (rc < 0) {
				TEST_PRINT_ERR("xrun: prepare error");
				rc = FAILURE;
				goto drain;
			}
		}
	}

	if (pcm_param->throughput_flag == TRUE) {
		/* Stop the Timer and get the usecs elapsed */
		elapsed_usecs = stop_timer(&start_time);
		elapsed_secs = (double)elapsed_usecs / 1000000u;
	}

	if (pcm_param->cpuload_flag == TRUE)
		/* Get CPU Load figures */
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

drain:
	st_audio_playback_drain();
close_file:
	rc = st_close(fd_audio);
	if (-1 == rc) {
		TEST_PRINT_ERR("file close failed ");
		rc = FAILURE;
	}
free_mem:
	if (NULL != buffer) {
		free(buffer);
	}
close_audio_intfc:
	st_audio_playback_close();
	return rc;

}

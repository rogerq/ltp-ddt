/*
 * st_alsa_interface.c 
 *
 * This file is an interface to the ALSA layer
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

#include "st_alsa_interface.h"
#include "st_alsa_common.h"

static snd_pcm_t *handle_playback;
static snd_pcm_t *handle_capture;
static snd_pcm_hw_params_t *params_playback;
static snd_pcm_hw_params_t *params_capture;
static snd_pcm_uframes_t frames_playback;
static snd_pcm_uframes_t frames_capture;
static int capture_dir;
static int playback_dir;
static unsigned int val;
static int err;

/*
 * Function          	- st_audio_read
 * Functionality     	- Wrapper function to the ALSA read API.
 * Input Params   	- buffer, access_type.
 * Return Value    	- Success = 0
 * Note               	- None
 */

int st_audio_read(char *buffer, int access_type)
{
	int rc = 0;
	/* RW_INTERLEAVED */
	if (access_type == PCM_ACCESS_RW_INTERLEAVED) {
		rc = snd_pcm_readi(handle_capture, buffer, frames_capture);
	}

	/* MMAP_INTERLEAVED */
	if (access_type == PCM_ACCESS_MMAP_INTERLEAVED) {
		rc = snd_pcm_mmap_readi(handle_capture, buffer, frames_capture);
	}
	if (rc == -EPIPE)
		return RUN_ERROR;
	else
		return SUCCESS;
}

/*
 * Function         	- st_audio_capture_prepare
 * Functionality    	- Wrapper function to the ALSA snd_pcm_prepare() which passes
 *					capture handle
 * Input Params   	- None
 * Return Value   	- 0 on Success and -1 on failure
 * Note              	- None
 */

int st_audio_capture_prepare(void)
{
	if ((err = snd_pcm_prepare(handle_capture)) < 0) {

		TEST_PRINT_ERR("cannot prepare audio interface for use (%s)",
			       snd_strerror(err));
		return FAILURE;
	} else {

		return SUCCESS;
	}
}

/*
 * Function         	- st_audio_capture_drain
 * Functionality    	- Wrapper function to the ALSA snd_pcm_drain() which passes
 *					capture handle
 * Input Params   	- None
 * Return Value    	- None
 * Note             	- None
 */

void st_audio_capture_drain(void)
{
	snd_pcm_drain(handle_capture);
}

/*
 * Function         	- st_audio_capture_close
 * Functionality    	- Wrapper function to the ALSA snd_pcm_close() which passes
 *					capture handle
 * Input Params   	- None
 * Return Value   	- None
 * Note              	- None
 */

void st_audio_capture_close(void)
{
	snd_pcm_close(handle_capture);
}

/*
 * Function          	- st_audio_write
 * Functionality    	- Wrapper function to the ALSA wirite API.
 * Input Params   	- buffer, access_type.
 * Return Value    	- Success = 0
 * Note              	- None
 */

int st_audio_write(char *buffer, int access_type)
{
	int rc = 0;
	/* RW_INTERLEAVED */
	if (access_type == PCM_ACCESS_RW_INTERLEAVED) {
		rc = snd_pcm_writei(handle_playback, buffer, frames_playback);
	}

	/* MMAP_INTERLEAVED */
	if (access_type == PCM_ACCESS_MMAP_INTERLEAVED) {
		rc = snd_pcm_mmap_writei(handle_playback, buffer,
					 frames_playback);
	}

	if (rc == -EPIPE)
		return RUN_ERROR;
	else
		return SUCCESS;
}

/*
 * Function         	- st_audio_playback_prepare
 * Functionality    	- Wrapper function to the ALSA snd_pcm_prepare() which passes
 *					playback handle
 * Input Params   	- None
 * Return Value    	- 0 on Success and -1 on failure
 * Note             	- None
 */

int st_audio_playback_prepare(void)
{
	if ((err = snd_pcm_prepare(handle_playback)) < 0) {

		TEST_PRINT_ERR("cannot prepare audio interface for use (%s)",
			       snd_strerror(err));
		return FAILURE;
	} else {

		return SUCCESS;
	}
}

/*
 * Function           	- st_audio_playback_drain
 * Functionality     	- Wrapper function to the ALSA snd_pcm_drain() which passes
 *					playback handle
 * Input Params   	- None
 * Return Value    	- None
 * Note               	- None
 */

void st_audio_playback_drain(void)
{
	snd_pcm_drain(handle_playback);
}

/*
 * Function           	- st_audio_playback_close
 * Functionality      	- Wrapper function to the ALSA snd_pcm_close() which passes
 *					playback handle
 * Input Params    	- None
 * Return Value     	- None
 * Note                	- None
 */

void st_audio_playback_close(void)
{
	snd_pcm_close(handle_playback);
}

/*
 * Function         	- st_audio_open_config_hw_interface
 * Functionality 	- Wrapper function to the ALSA APIs to set the params
 * Input Params 	- pcm_param
 * Return Value 	- App buffer size on success and -1 on Failure
 * Note                	- None
 */

int st_audio_open_config_hw_interface(tc_dev_params * pcm_param)
{
	int err;
	snd_pcm_uframes_t size;

	if (pcm_param->stream == CAPTURE) {

		if ((err =
		     snd_pcm_open(&handle_capture, pcm_param->card_name,
				  SND_PCM_STREAM_CAPTURE,
				  pcm_param->io_opmode)) < 0) {
			TEST_PRINT_ERR("cannot open audio device %s (%s)\n",
				       pcm_param->card_name, snd_strerror(err));
			return FAILURE;
		}

		/* Allocate a hardware parameters object. */

		if ((err = snd_pcm_hw_params_malloc(&params_capture)) < 0) {
			TEST_PRINT_ERR
			    ("cannot allocate hardware parameter structure (%s)\n",
			     snd_strerror(err));
			return FAILURE;
		}

		/* Fill it in with default values. */
		if ((err =
		     snd_pcm_hw_params_any(handle_capture,
					   params_capture)) < 0) {
			TEST_PRINT_ERR
			    ("cannot initialize hardware parameter structure (%s)\n",
			     snd_strerror(err));
			return FAILURE;
		}

		/* Set the desired hardware parameters. */

		/* Access mode */
		if ((err =
		     snd_pcm_hw_params_set_access(handle_capture,
						  params_capture,
						  pcm_param->access_type)) <
		    0) {
			TEST_PRINT_ERR("cannot set access type (%s)\n",
				       snd_strerror(err));
			return FAILURE;
		}

		/* format */
		if ((err =
		     snd_pcm_hw_params_set_format(handle_capture,
						  params_capture,
						  pcm_param->format)) < 0) {
			TEST_PRINT_ERR("cannot set sample format (%s)\n",
				       snd_strerror(err));
			return FAILURE;
		}

		/* channels  */
		if ((err =
		     snd_pcm_hw_params_set_channels(handle_capture,
						    params_capture,
						    pcm_param->channel)) < 0) {
			TEST_PRINT_ERR("cannot set channel count (%s)\n",
				       snd_strerror(err));
			return FAILURE;
		}

		/* Sample rate */
		val = pcm_param->sampling_rate;
		if ((err =
		     snd_pcm_hw_params_set_rate_near(handle_capture,
						     params_capture, &val,
						     &capture_dir)) < 0) {
			TEST_PRINT_ERR("cannot set sample rate (%s)\n",
				       snd_strerror(err));
			return FAILURE;
		}

		/* Set period size and buffer size */
		frames_capture = pcm_param->period_size;
		size = frames_capture * 2;
		if ((err =
		     snd_pcm_hw_params_set_period_size_near(handle_capture,
							    params_capture,
							    &frames_capture,
							    &capture_dir)) <
		    0) {
			TEST_PRINT_ERR("cannot set period size (%s)\n",
				       snd_strerror(err));
			return FAILURE;
		}
		if ((err =
		     snd_pcm_hw_params_set_buffer_size_near(handle_capture,
							    params_capture,
							    &size)) < 0) {
			TEST_PRINT_ERR("cannot set buffer size (%s)\n",
				       snd_strerror(err));
			return FAILURE;
		}

		/* Write the parameters to the driver */

		if ((err =
		     snd_pcm_hw_params(handle_capture, params_capture)) < 0) {
			TEST_PRINT_ERR("cannot set parameters (%s)\n",
				       snd_strerror(err));
			return FAILURE;
		}

		/* Use a buffer large enough to hold one period */
		snd_pcm_hw_params_get_period_size(params_capture,
						  &frames_capture,
						  &capture_dir);
		size = frames_capture * BYTES_PER_SAMPLE * pcm_param->channel;	/* 2 bytes/sample, 2 channels */
		return size;

	}

	else if (pcm_param->stream == PLAYBACK) {

		if ((err =
		     snd_pcm_open(&handle_playback, pcm_param->card_name,
				  SND_PCM_STREAM_PLAYBACK,
				  pcm_param->io_opmode)) < 0) {
			TEST_PRINT_ERR("cannot open audio device %s (%s)\n",
				       pcm_param->card_name, snd_strerror(err));
			return FAILURE;
		}

		/* Allocate a hardware parameters object. */

		if ((err = snd_pcm_hw_params_malloc(&params_playback)) < 0) {
			TEST_PRINT_ERR
			    ("cannot allocate hardware parameter structure (%s)\n",
			     snd_strerror(err));
			return FAILURE;
		}

		/* Fill it in with default values. */
		if ((err =
		     snd_pcm_hw_params_any(handle_playback,
					   params_playback)) < 0) {
			TEST_PRINT_ERR
			    ("cannot initialize hardware parameter structure (%s)\n",
			     snd_strerror(err));
			return FAILURE;
		}

		/* Set the desired hardware parameters. */

		/* Access mode */
		if ((err =
		     snd_pcm_hw_params_set_access(handle_playback,
						  params_playback,
						  pcm_param->access_type)) <
		    0) {
			TEST_PRINT_ERR("cannot set access type (%s)\n",
				       snd_strerror(err));
			return FAILURE;
		}

		/* format */
		if ((err =
		     snd_pcm_hw_params_set_format(handle_playback,
						  params_playback,
						  pcm_param->format)) < 0) {
			TEST_PRINT_ERR("cannot set sample format (%s)\n",
				       snd_strerror(err));
			return FAILURE;
		}

		/* channels  */
		if ((err =
		     snd_pcm_hw_params_set_channels(handle_playback,
						    params_playback,
						    pcm_param->channel)) < 0) {
			TEST_PRINT_ERR("cannot set channel count (%s)\n",
				       snd_strerror(err));
			return FAILURE;
		}

		/* Sample rate */
		val = pcm_param->sampling_rate;
		if ((err =
		     snd_pcm_hw_params_set_rate_near(handle_playback,
						     params_playback, &val,
						     &playback_dir)) < 0) {
			TEST_PRINT_ERR("cannot set sample rate (%s)\n",
				       snd_strerror(err));
			return FAILURE;
		}

		/* Set period size and buffer size */
		frames_playback = pcm_param->period_size;
		size = frames_playback * 2;
		if ((err =
		     snd_pcm_hw_params_set_period_size_near(handle_playback,
							    params_playback,
							    &frames_playback,
							    &playback_dir)) <
		    0) {
			TEST_PRINT_ERR("cannot set period size (%s)\n",
				       snd_strerror(err));
			return FAILURE;
		}
		if ((err =
		     snd_pcm_hw_params_set_buffer_size_near(handle_playback,
							    params_playback,
							    &size)) < 0) {
			TEST_PRINT_ERR("cannot set buffer size (%s)\n",
				       snd_strerror(err));
			return FAILURE;
		}

		/* Write the parameters to the driver */

		if ((err =
		     snd_pcm_hw_params(handle_playback, params_playback)) < 0) {
			TEST_PRINT_ERR("cannot set parameters (%s)\n",
				       snd_strerror(err));
			return FAILURE;
		}

		/* Use a buffer large enough to hold one period */
		snd_pcm_hw_params_get_period_size(params_playback,
						  &frames_playback,
						  &playback_dir);
		size = frames_playback * BYTES_PER_SAMPLE * pcm_param->channel;	/* 2 bytes/sample, 2 channels */
		return size;

	}

	return 0;
}

/*
 * Function         	- st_alsa_init_capture_test_params
 * Functionality 	- Initialize the test params for capture
 * Input Params 	- None
 * Return Value 	- None
 * Note                	- None
 */

void st_alsa_init_capture_test_params(void)
{
	testoptions_capture.access_type = PCM_ACCESS_RW_INTERLEAVED;
	testoptions_capture.buffer_size = DEFAULT_BUFFER_SIZE;
	testoptions_capture.card_name = DEFAULT_SND_CARD;
	testoptions_capture.channel = STEREO;
	testoptions_capture.device = DEFAULT_SND_DEVICE;
	testoptions_capture.format = PCM_FORMAT_S16_LE;
	testoptions_capture.file_name = "/mnt/default_capture.raw";
	testoptions_capture.io_opmode = BLOCKING;
	testoptions_capture.period_size = DEFAULT_PERIOD_SIZE;
	testoptions_capture.total_size = DEFAULT_TOTAL_SIZE;
	testoptions_capture.sampling_rate = DEFAULT_SAMPING_RATE;
	testoptions_capture.stream = CAPTURE;
	testoptions_capture.testcaseid = "ALSA_TEST";

}

/*
 * Function         	- st_alsa_init_playback_test_params
 * Functionality 	- Initialize the test params for playback
 * Input Params 	- None
 * Return Value 	- None
 * Note                 	- None
 */

void st_alsa_init_playback_test_params(void)
{
	testoptions_playback.access_type = PCM_ACCESS_RW_INTERLEAVED;
	testoptions_playback.buffer_size = DEFAULT_BUFFER_SIZE;
	testoptions_playback.card_name = DEFAULT_SND_CARD;
	testoptions_playback.channel = STEREO;
	testoptions_playback.device = DEFAULT_SND_DEVICE;
	testoptions_playback.format = PCM_FORMAT_S16_LE;
	testoptions_playback.file_name = "/mnt/default_capture.raw";
	testoptions_playback.io_opmode = BLOCKING;
	testoptions_playback.period_size = DEFAULT_PERIOD_SIZE;
	testoptions_playback.total_size = DEFAULT_TOTAL_SIZE;
	testoptions_playback.sampling_rate = DEFAULT_SAMPING_RATE;
	testoptions_playback.stream = PLAYBACK;
	testoptions_playback.testcaseid = "ALSA_TEST";

}

/*
 * Function         	- st_scale
 * Functionality 	- function used to generate sine wave
 * Input Params 	- data
 * Return Value 	- scaled data
 * Note                 - None
 */
short int st_scale(float data)
{
	short int ret = 0;

	ret = (short int)(data * AMPLITUDE);

	return ret;
}

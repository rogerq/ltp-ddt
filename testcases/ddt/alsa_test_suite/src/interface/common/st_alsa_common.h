/*
 * st_alsa_common.h 
 *
 * This file is a header file common for the test cases 
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

 /************************************************************************/

/********************  Header Files  ******************************************/

#ifndef _ST_AUDIO_COMMON_H_
#define _ST_AUDIO_COMMON_H_

#include "st_alsa_interface.h"
#include <st_defines.h>
#include <st_log.h>
#include <st_fileapi.h>
#include <st_timer.h>
#include <st_cpu_load.h>

#define TRUE 1
#define FALSE 0
#define ALSA_PCM_NEW_HW_PARAMS_API
#define STABILITY_LOOP_COUNT 10000
#define PLAYBACK 1
#define CAPTURE 0
#define DEFAULT_SND_CARD "hw:0,0"
#define DEFAULT_SND_DEVICE 0
#define DEFAULT_CAPTURE_PLAYBACK_TIME 10
#define DEFAULT_BUFFER_SIZE 2048
#define DEFAULT_PERIOD_SIZE 64
#define DEFAULT_TOTAL_SIZE 1024*1024*10

#define DEFAULT_SAMPING_RATE 44100

#define ALSA_LIB_VERSION SND_LIB_VERSION_STR
#define ST_STREAM_LAST SND_PCM_STREAM_LAST
#define ST_ACCESS_LAST SND_PCM_ACCESS_LAST
#define ST_FORMAT_LAST SND_PCM_FORMAT_LAST
#define ST_SUBFORMAT_LAST SND_PCM_SUBFORMAT_LAST
#define ST_STATE_LAST SND_PCM_STATE_LAST
#define ST_SLEEP(x) sleep(x);
#define STABILITY_COUNT 0
#define DEFAULT_PLAYBACK_DURATION 10
#define DEFAULT_CAPTURE_DURATION 10

/* Return values */
#define SUCCESS 0
#define FAILURE -1
#define DEVICE_NOT_SUPPORTED -2
#define RUN_ERROR -3

#define LEFT_SINE_FREQ 200	/* Hz */
#define RIGHT_SINE_FREQ 2000	/* Hz */
#define AMPLITUDE 32000
typedef struct _tc_dev_params {
	int device;
	int card;
	char *card_name;
	int stream;
	int loopback;
	int io_opmode;
	int access_type;
	int format;
	char *file_name;
	char *testcaseid;
	unsigned int sampling_rate;
	unsigned int channel;
	unsigned int period_size;
	unsigned int period_time;
	unsigned int buffer_time;
	unsigned int buffer_size;
	unsigned int total_size;
	unsigned int sample_size;
	unsigned int play_from_file;
	unsigned int capture_to_file;
	unsigned int throughput_flag;
	unsigned int cpuload_flag;
} tc_dev_params;
typedef enum {
	BLOCKING = 0,
	NON_BLOCKING,
	ASYNC
} io_opmodes;
typedef enum {
	PCM_ACCESS_MMAP_INTERLEAVED = 0,	/* interleaved mmap */
	PCM_ACCESS_MMAP_NONINTERLEAVED = 1,	/* noninterleaved mmap */
	PCM_ACCESS_MMAP_COMPLEX = 2,	/* complex mmap */
	PCM_ACCESS_RW_INTERLEAVED = 3,	/* readi/writei */
	PCM_ACCESS_RW_NONINTERLEAVED = 4,	/* readn/writen */
} access_types;
typedef enum {
	PCM_FORMAT_S8 = 0,
	PCM_FORMAT_U8 = 1,
	PCM_FORMAT_S16_LE = 2,
	PCM_FORMAT_S16_BE = 3,
	PCM_FORMAT_U16_LE = 4,
	PCM_FORMAT_U16_BE = 5,
	PCM_FORMAT_S24_LE = 6,	/* low three bytes */
	PCM_FORMAT_S24_BE = 7,	/* low three bytes */
	PCM_FORMAT_U24_LE = 8,	/* low three bytes */
	PCM_FORMAT_U24_BE = 9,	/* low three bytes */
	PCM_FORMAT_S32_LE = 10,
	PCM_FORMAT_S32_BE = 11,
	PCM_FORMAT_U32_LE = 12,
	PCM_FORMAT_U32_BE = 13,
	MAX_FORMAT_TYPES = PCM_FORMAT_U32_BE
} sample_formats;
typedef enum {
	MONO = 1,
	STEREO = 2
} channels;
tc_dev_params testoptions_capture;
tc_dev_params testoptions_playback;
void st_audio_capture_hw_params(void);
int st_audio_read(char *buffer, int access_type);
int st_audio_capture_prepare(void);
char *st_audio_error(int rc);
void st_audio_capture_drain(void);
void st_audio_capture_close(void);
int st_audio_open_config_hw_interface(tc_dev_params * pcm_param);
void st_audio_playback_hw_params(void);
int st_audio_write(char *buffer, int access_type);
int st_audio_playback_prepare(void);
void st_audio_playback_drain(void);
void st_audio_playback_close(void);
void st_alsa_init_capture_test_params(void);
void st_alsa_init_playback_test_params(void);
int st_audio_capture(tc_dev_params * pcm_param);
int st_audio_playback(tc_dev_params * pcm_param);
void st_audio_pcm_type_format(void);
void st_audio_pcm_volume_ctrl(void);
int st_audio_loopback(void);
int st_pcm_pthread_create(pthread_t * thread_ID,
			  void *(*start_routine) (void *));
int st_pcm_pthread_join(pthread_t thread_ID);
void st_pcm_pthread_exit(void *ptr);
void st_audio_pcm_stress_test(void);
void st_audio_pcm_stability_test(void);
int st_audio_file_playback(tc_dev_params * pcm_param);
int st_audio_file_capture(tc_dev_params * pcm_param);
short int st_scale(float data);

#endif /*  */

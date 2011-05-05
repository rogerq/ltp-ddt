/*
 * st_v4l2_display_parser.h
 *
 * This file contains command line option definions
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

#ifndef _ST_V4L2_DISP_PARSER_H_
#define _ST_V4L2_DISP_PARSER_H_

#include <st_v4l2_capture_common.h>

/*
* Define Options here, so that it would be easy to add new features/options.
*/
/* Device name on which display test is to be run */
#define OPTION_DEVICE_NAME_CAP  	1
/* Width of the image to be displayed */
#define OPTION_WIDTH        		2
/* Height of the image to be displayed */
#define OPTION_HEIGHT       		3
/* Width of the image to be displayed */
#define OPTION_TOP          		4
/* Height of the image to be displayed */
#define OPTION_LEFT         		5
/* Control Value */
#define OPTION_VALUE        		6
/* Control ID */
#define OPTION_ID           		7
/* Crop height to be displayed */
#define OPTION_CROP_HEIGHT  		8
/* Crop width to be displayed */
#define OPTION_CROP_WIDTH   		9
/* Crop top to be displayed */
#define OPTION_CROP_TOP     		10
/* Crop left to be displayed */
#define OPTION_CROP_LEFT		11
/* Scale height to be displayed */
#define OPTION_FMT_HEIGHT   		12
/* Scale width to be displayed */
#define OPTION_FMT_WIDTH    		13
/* Scale top to be displayed */
#define OPTION_FMT_TOP      		14
/* Scale left to be displayed */
#define OPTION_FMT_LEFT     		15
/* Pixel Format to be displayed */
#define OPTION_PIXELFMT     		16
/* Terstcase ID*/
#define OPTION_TESTID       		17
/* Number of buffers */
#define OPTION_BUFNOS       		18
/* Number of buffers */
#define OPTION_BUFTYPE      		19
/* Number of Frames */
#define OPTION_LOOP         		20
/* IOCTL Number */
#define OPTION_IOCTL_TEST   		21
/* IOCTL Number */
#define OPTION_CRTL_ID       		22
/* IOCTL Number */
#define OPTION_CRTL_VAL      		23
/* Display Device */
#define OPTION_DEVICE_NAME_DISP    	24
/* CPU Load */
#define OPTION_CPULOAD      		25
/* Frame Rate */
#define OPTION_FRAMERATE    		26
/* User Pointer */
#define OPTION_USERPTR      		27
/* Displays the help */
#define OPTION_HELP         		28
/* Displays the version of LFTB package */
#define OPTION_VERSION      		29

#endif  /* #ifndef _ST_V4L2_DISP_PARSER_H_ */

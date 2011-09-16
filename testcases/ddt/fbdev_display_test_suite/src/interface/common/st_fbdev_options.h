/*
 * st_fbdev_parser.h
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

#ifndef _ST_FBDEV_PARSER_H_
#define _ST_FBDEV_PARSER_H_

/*
 * Define Options here, so that it would be easy to add new features/options.
 */
/* Device name on which display test is to be run */
#define OPTION_DEVICE_NAME	1
/* Width of the image to be displayed */
#define OPTION_WIDTH		2
/* Height of the image to be displayed */
#define OPTION_HEIGHT		3
/* Virtual Width of the image to be displayed */
#define OPTION_VIRTUAL_WIDTH	4
/* Virtual Height of the image to be displayed */
#define OPTION_VIRTUAL_HEIGHT	5
/* Pixel Format */
#define OPTION_BPP		6
/* Left Margin */
#define OPTION_LEFT_MARGIN	7
/* Right Margin */
#define OPTION_RIGHT_MARGIN	8
/* Lower Margin */
#define OPTION_LOWER_MARGIN	9
/* Upper Margin */
#define OPTION_UPPER_MARGIN	10
/* Hsync len */
#define OPTION_HSYNC_LEN	11
/* Vsync len */
#define OPTION_VSYNC_LEN 	12
/* Grey scale */
#define OPTION_GRAYSCALE	13
/* Color depth and offset [< r,g,b,a>] */
#define OPTION_RGBA_OFF		14
/* Rotation angle [Note: Please check support for rotation] */
#define OPTION_ROTATE		15
/* Panning */
#define OPTION_PAN		16
/* Mirroring angle [Note: Please check support for mirroring] */
#define OPTION_MIRROR		17
/* Name of the image file to display */
#define OPTION_FILE_NAME	18
/* Output: LCD, DVI, TV etc...*/
#define OPTION_OUTPUT		19
/* Number of times to run loop for ioctl test */
#define OPTION_LOOP_CNT		20
/* IOCTL Test */
#define OPTION_IOCTL_TEST	21
/* Test case id string for logging purpose */
#define OPTION_TESTCASE_ID	22
/* Displays the frame rate in fps(frames per sec) */
#define OPTION_FRAME_RATE	23
/* Displays the help */
#define OPTION_HELP		24
/* Displays the version of LFTB package */
#define OPTION_VERSION		25
/* Displays the cpu load in percenatge */
#define OPTION_CPU_LOAD		26

#define OPTION_COLORKEY         27
#define OPTION_ALPHA            28
#define OPTION_NONSTD_PIXELFMT	29

/*
 * IOCTL Support Options
 */
/* get variable screen info */
#define OPTION_IOCTL_GETVSCREENINFO	0

/* put variable screen info */
#define OPTION_IOCTL_PUTVSCREENINFO	1

/* get fix screen info */
#define OPTION_IOCTL_GETFSCREENINFO	2

/* on/of blanking */
#define OPTION_IOCTL_BLANK		3

/* get color map table */
#define OPTION_IOCTL_GETCMAP		4

/* put color map table */
#define OPTION_IOCTL_PUTCMAP		5
#define OPTION_IOCTL_PANDISPLAY		6

#endif	/* #ifndef _ST_FBDEV_PARSER_H_ */

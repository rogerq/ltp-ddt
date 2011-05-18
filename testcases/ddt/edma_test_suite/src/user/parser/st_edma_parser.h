/*
 * st_edma_parser.h
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

#ifndef _ST_edma_PARSER_H_
#define _ST_edma_PARSER_H_

/*
 * Define Options here, so that it would be easy to add new features/options.
 */
 
/* 
 *Device name on which edma test is to be run 
 */
#define OPTION_DEVICE_NAME	1

/* 
 *execute the IOCTL test case
 */
#define OPTION_IOCTL	2

/* 
 *ACNT value 
 */
#define OPTION_ACNT	3

/* 
 * BCNT value 
 */
#define OPTION_BCNT	4

/* 
 *CCNT value 
 */
#define OPTION_CCNT	5

/* 
 * Number of Transfer controllers 
 */
#define OPTION_NUMTCS	6


/* 
 * Test case id string for logging purpose 
 */
#define OPTION_TESTCASE_ID	7

/* 
 * Enables performance calculation
 */
#define OPTION_PERFORMANCE 8

/*
 *Displays the version of LFTB package 
*/
#define OPTION_VERSION	'v'
/* 
 *Displays the help
 */
#define OPTION_HELP	'h'


#define MAX_DMA_TRANSFER_IN_BYTES   1024 * 1024 * 2

/* 
 *Table which holds the ioctl info 
 */
struct edma_ioctl_table ioctl_table[] = {
	{"edma_async", EDMA_ASYNC},
	{"edma_absync", EDMA_ABSYNC},
	{"edma_link_async", EDMA_LINK_ASYNC},
	{"edma_link_absync", EDMA_LINK_ABSYNC},
	{"edma_chain_async", EDMA_CHAIN_ASYNC},
	{"edma_chain_absync",  EDMA_CHAIN_ABSYNC},
	{NULL, 0}
};


#endif	/* #ifndef _ST_edma_PARSER_H_ */

/*
 * st_defines.h 
 * 
 * This file contains the standard definitions used in the performance test suite
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

#ifndef _ST_DEFINES_H_
#define _ST_DEFINES_H_

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
/* Version string for the test suite. Change this for every release */
#define VERSION_STRING "02.00.00.04"

/* Use these defines to improve readability */
#define IN
#define OUT
#define INOUT
#define OPT

/* Return codes */
#define SUCCESS 0
#define FAILURE -1
#define NOT_SUPPORTED -2
#define INVALID_PARAMS -3
#define INVALID_STATE -4
#define TIMEOUT -5

/* Status codes */
#define TRUE 1
#define FALSE 0

typedef void *Ptr;

typedef char Int8;		/*  8 bit value */
typedef short int Int16;	/* 16 bit value */
typedef long int Int32;		/* 32 bit value */

typedef unsigned char Uint8;	/*  8 bit value */
typedef unsigned short int Uint16;	/* 16 bit value */
typedef unsigned long int Uint32;	/* 32 bit value */

typedef short int Bool;		/* 16 bit value */

#endif /* _ST_DEFINES_H_ */

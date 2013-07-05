/*
 * Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation version 2.
 *
 * This program is distributed "as is" WITHOUT ANY WARRANTY of any
 * kind, whether express or implied; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
*/

#ifndef _ST_DEFINES_H_
#define _ST_DEFINES_H_

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
/* Version string for the test suite. Change this for every release */
#define VERSION_STRING "02.00.00.05"

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

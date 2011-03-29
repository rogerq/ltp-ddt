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

/*Generic header files */
#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <fcntl.h>

/* Generic header files */
#include <st_defines.h>

/*
 * Function             	: st_open
 * Functionality        	: To open a file
 * Input Params         	: pathname    - Name of the file to be open
 *                      		: oflag - arguments passed to open
 * Return Value         	: file descriptor 
 */
Int32 st_open (IN const Int8* pathname, IN Int32 oflag)
{

    Int32 fd =0;
    fd = open((const char*)pathname, oflag);
	if(-1 == fd)
	{
		perror("\n open ");
	}

    return fd;
}

/*
 * Function             	: st_write
 * Functionality        	: writes up to count bytes to the file referenced by the file descriptor 
 * Input Params         	: fd    - file descriptor
 *                      		: buf - buffer pointer
 					: count -  number of bytes to be written
 * Return Value         	: number of bytes written 
 */
Int32 st_write (IN  Int32 fd, IN const void *buf,IN Int32 count)
{

    Int32 ret =0;
	ret = write(fd, buf, count) ;
    if(count != ret)
    {
    	perror("\n write ") ;
        close(fd);
    }
    return ret;
}

/*
 * Function             	: st_read
 * Functionality        	: read up to count bytes to the file referenced by the file descriptor 
 * Input Params         	: fd    - file descriptor
 *                      		: buf - buffer pointer
 *					: count -  number of bytes to be read
 * Return Value         	: number of bytes read
 */
Int32 st_read (IN  Int32 fd, IN void *buf,IN Int32 count)
{

    Int32 ret =0;
	ret = read(fd, buf, count) ;
    if(count != ret)
    {
    	perror("\n read ") ;
        close(fd);
    }
    return ret;
}

/*
 * Function             	: st_perror
 * Functionality        	: write error messages to standard error
 * Input Params         	: s    - error message strings 
 * Return Value         	: None
 */

void st_perror(const char *s)
{
	perror(s);

}

/*
 * Function             	: st_close
 * Functionality        	: To close a file
 * Input Params         	: fd    - closes  a file descriptor
 * Return Value         	: zero on success, or -1 if an error occurred 
 */
Int32 st_close (IN Int32 fd)
{
	Int32 ret =0;
    ret = close(fd);
	if(-1 == ret)
    {
    	perror("\n close ");
    }

	return ret;
}


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

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mount.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <st_defines.h>


Int32 st_thread_create(pthread_t *restrict thread,
              const pthread_attr_t *restrict attr,
              void *(*start_routine)(void*), void *restrict arg);

Int32 st_thread_join(pthread_t thread, void **thread_ret);


/*
 * Function		:st_thread_create
 * Functionality	:Wrapper for pthread_create
 * Input Params	:pthread_t *, thread attributes,thread function and arguments
 * Return Value	:Int32, ret_val on success, ST_FAIL otherwise
 */

Int32 st_thread_create(pthread_t *restrict thread,
              const pthread_attr_t *restrict attr,
              void *(*start_routine)(void*), void *restrict arg)
{
	return (pthread_create(thread, attr, start_routine, arg));
|


/*
 * Function		:st_thread_create
 * Functionality	:Wrapper for pthread_join
 * Input Params	:pthread_t *, void** 
 * Return Value	:Int8, ret_val on success, ST_FAIL otherwise
 */

Int32 st_thread_join(pthread_t thread, void **thread_ret)
{
	return ((pthread_join(thread, thread_ret)));
}

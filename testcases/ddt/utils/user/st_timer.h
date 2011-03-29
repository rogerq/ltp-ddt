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

#ifndef _ST_TIMER_H_
#define _ST_TIMER_H_

/* Standard include files */
#include <sys/time.h>
#include <stdlib.h>

#include <st_defines.h>

/* Initialize Timer module */
void init_timer_module(void);

/* Get the current time, update the ptimer_handle */
void get_time(INOUT struct timeval * ptimer_handle);

/* Start the timer and update the ptimer_handle */
void start_timer(INOUT struct timeval * ptimer_handle);

/* Stop the Timer and return the elapsed usecs  */
unsigned long stop_timer(IN struct timeval * ptimer_handle);

#endif /* _ST_TIMER_H_ */

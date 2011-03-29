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

#include <st_defines.h>
#include <st_log.h>
#include <st_timer.h>

/*
 *Use this define to calibrate the delta value 
 */
#define BASE_NUM_TRIALS 10

/*
 *Delta in microseconds to be adjusted 
 */
static int delta_time = 0;


static long diff_time(IN struct timeval *ptime_start,
		      IN struct timeval *ptime_end);

static long elapsed_time(IN struct timeval *ptime_start,
			 IN struct timeval *ptime_end);

/*
 * Function 			: init_timer_module
 * Functionality		: Initialize the Timer module  
 * Input Params 		: None
 * Return Value 		: None
 */

void init_timer_module(void)
{
	struct timeval start_time;
	struct timeval end_time;
	int i = 0;

	/* Already calibrated */
	if (0 != delta_time)
		return;

	/* We need to average delta_time out */
	for (i = 0; i < BASE_NUM_TRIALS; i++) {
		start_timer(&start_time);
		start_timer(&end_time);

		delta_time += diff_time(&start_time, &end_time);
	}

	delta_time = delta_time / BASE_NUM_TRIALS;

}

/*
 * Function 			: get_time
 * Functionality		: Get the current time, update the ptimer_handle  
 * Input Params 		: ptimer_handle 
 * Return Value 		: None
 */
void get_time(INOUT struct timeval *ptimer_handle)
{
	start_timer(ptimer_handle);
}

/*
 * Function 			: start_timer
 * Functionality		: Start the timer and update the ptimer_handle 
 * Input Params 		: ptimer_handle 
 * Return Value 		: None
 */
void start_timer(INOUT struct timeval *ptimer_handle)
{
	struct timeval *pstart_timeval = ptimer_handle;

	gettimeofday(pstart_timeval, NULL);

#if LOG_TIME
	PERFLOG("timeVal.sec %ld\n", pstart_timeval->tv_sec);
	PERFLOG("timeVal.usec %ld\n", pstart_timeval->tv_usec);
#endif
}


/*
 * Function 			: stop_timer
 * Functionality		: Stop the Timer and return the elapsed usecs  
 * Input Params 		: ptimer_handle 
 * Return Value 		: elapsed time in usecs
 */
unsigned long stop_timer(IN struct timeval *ptimer_handle)
{
	struct timeval *pstart_timeval = ptimer_handle;
	struct timeval stop_time_val;
	gettimeofday(&stop_time_val, NULL);

#if LOG_TIME
	PERFLOG("timeVal.sec %ld\n", stop_time_val.tv_sec);
	PERFLOG("timeVal.usec %ld\n", stop_time_val.tv_usec);
#endif
	return ((unsigned long)elapsed_time(pstart_timeval, &stop_time_val));
}

/*
 * Function             	: diff_time
 * Functionality        	: Return the difference between 2 timeval struct 
 *					: in microseconds
 * Input Params         	: ptime_start    - start time 
 *                      		: ptime_end - end time
 * Return Value         	: difference between 2 timeval structs  in microseconds
 */
static long diff_time(IN struct timeval *ptime_start,
		      IN struct timeval *ptime_end)
{
	return ((ptime_end->tv_sec - ptime_start->tv_sec) * 1000000u
		+ ptime_end->tv_usec - ptime_start->tv_usec);
}

/*
 * Function             	: elapsed_time
 * Functionality        	: Return the time elapsed between 2 timeval structs
 *					: in microseconds
 * Input Params         	: ptime_start    - start time 
 *                      		: ptime_end - end time
 * Return Value         	: elapsed tim between 2 timeval structs  in microseconds
 */
static long elapsed_time(IN struct timeval *ptime_start,
			 IN struct timeval *ptime_end)
{
	return (diff_time(ptime_start, ptime_end) - delta_time);
}

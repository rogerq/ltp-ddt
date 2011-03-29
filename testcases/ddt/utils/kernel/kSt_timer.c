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

#include <linux/module.h>
#include <linux/init.h>
#include <linux/errno.h>
#include <linux/interrupt.h>
#include <linux/moduleparam.h>
#include <linux/sysctl.h>
#include <linux/mm.h>

#include "kSt_timer.h"
#include "kSt_log.h"


#define BASE_NUM_TRIALS 10

static u32 diff_time(struct timeval *time_start, struct timeval *time_end);

static int delta_time;
static struct timeval start_time;
static struct timeval end_time;

/* 
 * Initialize Timer module which take care of calibration
 */
static int __init perf_timer_init(void)
{
	int i = 0;
	delta_time = 0;
	
	for (i = 0; i < BASE_NUM_TRIALS; i++) {
		do_gettimeofday(&start_time);
		do_gettimeofday(&end_time);
		delta_time += diff_time(&start_time, &end_time);
	}
	delta_time = delta_time / BASE_NUM_TRIALS;
	DEBUG_PRINT("Delta : %d", delta_time);

	return 0;
}

#define LOG_TIME 0
void start_timer(void)
{
	do_gettimeofday(&start_time);
#if LOG_TIME
	printk("start_time.sec %d\n", start_time.tv_sec);
	printk("start_time.usec %d\n", start_time.tv_usec);
#endif

}

u32 stop_timer(void)
{
	u32 usecs;
	do_gettimeofday(&end_time);
#if LOG_TIME
	printk("end_time.sec %d\n", end_time.tv_sec);
	printk("end_time.usec %d\n", end_time.tv_usec);
#endif
	usecs = diff_time(&start_time, &end_time);
	usecs -= delta_time;	/* Compensation for calibrated value */
	return usecs;
}

static u32 diff_time(struct timeval *time_start, struct timeval *time_end)
{
	return ((time_end->tv_sec - time_start->tv_sec) * 1000000u
		+ time_end->tv_usec - time_start->tv_usec);
}

/*
 *Return the difference between 2 timeval structs 
 * in microseconds
 */
static void perf_timer_exit(void)
{
	DEBUG_PRINT("Perf Timer:NOTIFY	:Exiting Performance timer");
}

EXPORT_SYMBOL(start_timer);
EXPORT_SYMBOL(stop_timer);

module_init(perf_timer_init);
module_exit(perf_timer_exit);
MODULE_AUTHOR("Texas Instruments");
MODULE_LICENSE("GPL");


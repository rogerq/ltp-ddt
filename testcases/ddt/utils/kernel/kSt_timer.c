/*
 * kSt_timer.c 
 * 
 * This file implements the timer wrappers
 * 
 * 
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


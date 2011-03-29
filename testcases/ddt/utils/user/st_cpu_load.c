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
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include <st_defines.h>
#include <st_cpu_load.h>

/*
 *#define CPULOAD_DEBUG
 */

int enable_cpu_load = 1;

/*
 * Function             	: get_cpu_status
 * Functionality        	: Get the current Status from the Cpu
 * Input Params         	: cpu_status    - to update the cpu status 
 * Return Value         	: None
 */
static void get_cpu_status(OUT struct proc_stat * cpu_status)
{
	static FILE *fp = NULL;
	int num;
	char buf[256 + 64];

	if (cpu_status == NULL) {
		printf("error: Null pointer passed to CpuLoadMeasurement().");
		return;
	}

	if (!fp) {
		if (!(fp = fopen("/proc/stat", "r"))) {
			printf("Failed /proc/stat open: %s", strerror(errno));
			return;
		}
	}
	rewind(fp);
	fflush(fp);

	/* first value the last slot with the cpu summary line */
	if (!fgets(buf, sizeof(buf), fp)) {
		printf("failed /proc/stat read");
		return;
	}

	/* populate evrying to 0 as we are not sure about the support in the kernel */
	cpu_status->user = 0;
	cpu_status->nice = 0;
	cpu_status->system = 0;
	cpu_status->idle = 0;
	cpu_status->iowait = 0;
	cpu_status->irq = 0;
	cpu_status->softirq = 0;
	cpu_status->steal = 0;

	num = sscanf(buf, "cpu %Lu %Lu %Lu %Lu %Lu %Lu %Lu %Lu",
		     &(cpu_status->user),
		     &(cpu_status->nice),
		     &(cpu_status->system),
		     &(cpu_status->idle),
		     &(cpu_status->iowait),
		     &(cpu_status->irq),
		     &(cpu_status->softirq),
		     &(cpu_status->steal)
	    );
	if (num < 4) {
		printf("failed /proc/stat read");
		return;
	}

	return;
}

/*
 * Function             	: get_cpu_load
 * Functionality        	: Get the Load from the Cpu
 * Input Params         	: cpu_status_start    - struct proc_stat to get the cpu start status from 
 *					: cpu_status_end    - struct proc_stat to get the cpu start status from
 * 
 * Return Value         	: cpu load on success
 *					: -1 on failure
 */
static float get_cpu_load(IN struct proc_stat cpu_status_start,
			  IN struct proc_stat cpu_status_end)
{
	struct proc_stat cpu_statusDiff = { 0, };
	float cpu_load = 0, total_time = 0, idle_time = 0;

	cpu_statusDiff.user = cpu_status_end.user - cpu_status_start.user;
	cpu_statusDiff.nice = cpu_status_end.nice - cpu_status_start.nice;
	cpu_statusDiff.system = cpu_status_end.system - cpu_status_start.system;
	cpu_statusDiff.idle = cpu_status_end.idle - cpu_status_start.idle;
	cpu_statusDiff.iowait = cpu_status_end.iowait - cpu_status_start.iowait;
	cpu_statusDiff.irq = cpu_status_end.irq - cpu_status_start.irq;
	cpu_statusDiff.softirq =
	    cpu_status_end.softirq - cpu_status_start.softirq;
	cpu_statusDiff.steal = cpu_status_end.steal - cpu_status_start.steal;

	total_time = (float)(cpu_statusDiff.user
			    + cpu_statusDiff.nice
			    + cpu_statusDiff.system
			    + cpu_statusDiff.idle
			    + cpu_statusDiff.iowait
			    + cpu_statusDiff.irq
			    + cpu_statusDiff.softirq
			    + cpu_statusDiff.steal);
	idle_time = (float)cpu_statusDiff.idle;
	/* IOwait should not be accounted in CPU perspective, refer: http://kbase.redhat.com/faq/FAQ_80_5637.shtm */
	idle_time += (float)cpu_statusDiff.iowait;

	if (total_time) {
		cpu_load = ((total_time - idle_time) * 100) / total_time;
	} else {
		printf
		    ("\n !!! Warning: Unable to calculate cpu_load,                              !!! \n");
		printf
		    ("\n !!!          total time of exectuiton is too low (lesser than a jiffy) !!! \n");
		cpu_load = -1;
	}

#ifdef CPULOAD_DEBUG
	printf("\n");
	printf("--------------------------------------------------------\n");
	printf("          |              Time(in jiffies)               \n");
	printf("CPU state |---------------------------------------------\n");
	printf("          |       <start>      <end>     <diff>         \n");
	printf("--------------------------------------------------------\n");
	printf("user      |    %10Lu %10Lu %10Lu\n", cpu_status_start.user,
	       cpu_status_end.user, cpu_statusDiff.user);
	printf("nice      |    %10Lu %10Lu %10Lu\n", cpu_status_start.nice,
	       cpu_status_end.nice, cpu_statusDiff.nice);
	printf("system    |    %10Lu %10Lu %10Lu\n", cpu_status_start.system,
	       cpu_status_end.system, cpu_statusDiff.system);
	printf("idle      |    %10Lu %10Lu %10Lu\n", cpu_status_start.idle,
	       cpu_status_end.idle, cpu_statusDiff.idle);
	printf("iowait    |    %10Lu %10Lu %10Lu\n", cpu_status_start.iowait,
	       cpu_status_end.iowait, cpu_statusDiff.iowait);
	printf("irq       |    %10Lu %10Lu %10Lu\n", cpu_status_start.irq,
	       cpu_status_end.irq, cpu_statusDiff.irq);
	printf("softirq   |    %10Lu %10Lu %10Lu\n", cpu_status_start.softirq,
	       cpu_status_end.softirq, cpu_statusDiff.softirq);
	printf("steal     |    %10Lu %10Lu %10Lu\n", cpu_status_start.steal,
	       cpu_status_end.steal, cpu_statusDiff.steal);
	printf("user_rt   |    %10Lu %10Lu %10Lu\n", cpu_status_start.user_rt,
	       cpu_status_end.user_rt, cpu_statusDiff.user_rt);
	printf("system_rt |    %10Lu %10Lu %10Lu\n", cpu_status_start.system_rt,
	       cpu_status_end.system_rt, cpu_statusDiff.system_rt);
	printf("--------------------------------------------------------\n");
	printf("                         total_time = %10Lu*10ms\n",
	       (unsigned long long)total_time);
	printf("   idle_time == (idle+iowait+steal) = %10Lu*10ms\n",
	       (unsigned long long)idle_time);
	printf("--------------------------------------------------------\n");
	printf("cpu_load == (total_time-idle_time)*100/total_time = %3.2f%% \n",
	       cpu_load);
	printf("--------------------------------------------------------\n");
#endif
	return cpu_load;
}

/*
 * Function             	: start_cpuload_measurement
 * Functionality        	: Get the current Status from the Cpu
 * Input Params         	: cpu_status_start    - struct proc_stat to get the cpu start status from 
 * Return Value         	: None
 */
void start_cpuload_measurement(OUT struct proc_stat * cpu_status_start)
{
	if (enable_cpu_load) {
		get_cpu_status(cpu_status_start);
	}
	return;
}

/*
 * Function             	: stop_cpuload_measurement
 * Functionality        	: Stop CPU Load measurement 
 * Input Params         	: cpu_status_start    - struct proc_stat to get the cpu start status from 
 * Return Value         	: % CPU Load on success
 *					: -1 on Failure
 */
 
float stop_cpuload_measurement(IN const struct proc_stat * cpu_status_start)
{
	struct proc_stat cpu_status_end;

	if (enable_cpu_load) {
		get_cpu_status(&cpu_status_end);
		return (get_cpu_load(*cpu_status_start, cpu_status_end));
	} else {
		return (CPULOAD_NOT_ENABLED);
	}
}

/*
 * ionice.c - ionice
 * Command-line utility for setting and retrieving a task's I/O priority
 *
 * Jens Axboe	<axboe@suse.de>
 * Robert Love	<rlove@rlove.org>
 *
 * Linux kernels as of 2.6.12 provide the needed syscalls for
 * manipulating a task's I/O priority.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License, v2, as
 * published by the Free Software Foundation
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * Copyright (C) 2005 Novell, Inc.
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <getopt.h>
#include <unistd.h>
#include <sys/ptrace.h>
#include <sys/syscall.h>

static inline int ioprio_set (int which, int who, int ioprio)
{
	return syscall (__NR_ioprio_set, which, who, ioprio);
}

static inline int ioprio_get (int which, int who)
{
	return syscall (__NR_ioprio_get, which, who);
}

enum {
	IOPRIO_CLASS_NONE,
	IOPRIO_CLASS_RT,
	IOPRIO_CLASS_BE,
	IOPRIO_CLASS_IDLE,
};

enum {
	IOPRIO_WHO_PROCESS = 1,
	IOPRIO_WHO_PGRP,
	IOPRIO_WHO_USER,
};

#define IOPRIO_CLASS_SHIFT	13
#define IOPRIO_PRIO_MASK	0xff

const char *to_prio[] = { "none", "realtime", "best-effort", "idle", };

int main (int argc, char *argv[])
{
	int ioprio = 4, set = 0, ioprio_class = IOPRIO_CLASS_BE;
	int c, pid = 0;

	while ((c = getopt (argc, argv, "+n:c:p:V")) != EOF) {
		switch (c) {
		case 'n':
			ioprio = strtol (optarg, NULL, 10);
			set = 1;
			break;
		case 'c':
			ioprio_class = strtol (optarg, NULL, 10);
			set = 1;
			break;
		case 'p':
			pid = strtol (optarg, NULL, 10);
			break;
		case 'V':
			printf ("ionice version " VERSION "\n");
			return 0;
		}
	}

	switch (ioprio_class) {
		case IOPRIO_CLASS_NONE:
			ioprio_class = IOPRIO_CLASS_BE;
			break;
		case IOPRIO_CLASS_RT:
		case IOPRIO_CLASS_BE:
			break;
		case IOPRIO_CLASS_IDLE:
			ioprio = 7;
			break;
		default:
			fprintf (stderr, "invalid class %d!\n", ioprio_class);
			return 1;
	}

	if (!set) {
		if (!pid && argv[optind])
			pid = strtol (argv[optind], NULL, 10);

		ioprio = ioprio_get (IOPRIO_WHO_PROCESS, pid);
		if (ioprio == -1)
			perror ("ioprio_get");
		else {
			ioprio_class = ioprio >> IOPRIO_CLASS_SHIFT;
			ioprio = ioprio & IOPRIO_PRIO_MASK;
			printf ("%s: prio %d\n", to_prio[ioprio_class], ioprio);
		}
	} else {
		if (ioprio_set (IOPRIO_WHO_PROCESS, pid, ioprio | ioprio_class
				<< IOPRIO_CLASS_SHIFT) == -1) {
			perror ("ioprio_set");
			return 1;
		}

		if (argv[optind])
			execvp (argv[optind], &argv[optind]);
	}

	return 0;
}

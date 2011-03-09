/*

 * Copyright (c) 2002-2003, Intel Corporation. All rights reserved.
 * Created by:  rusty.lynch REMOVE-THIS AT intel DOT com
 * This file is licensed under the GPL license.  For the full content
 * of this license, see the COPYING file at the top level of this
 * source tree.

  Test case for assertion #13 of the sigaction system call that verifies
  signal-catching functions are executed on the current stack if the
  SA_ONSTACK flag is cleared in the sigaction.sa_flags parameter to the
  sigaction function call.

  NOTE: This test case does not attempt to verify the proper operation
        of sigaltstack.
*/

#define _XOPEN_SOURCE 600

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include "posixtest.h"

stack_t current;

void handler(int signo)
{
	stack_t oss;

	printf("Caught SIGTRAP\n");

	if (sigaltstack((stack_t *)0, &oss) == -1) {
		perror("Unexpected error while attempting to setup test "
		       "pre-conditions");
		exit(-1);
	}

	if (oss.ss_sp != current.ss_sp || oss.ss_size != current.ss_size) {
		printf("Test FAILED\n");
		exit(-1);
	}
}

int main()
{
	struct sigaction act;

	act.sa_handler = handler;
	act.sa_flags = 0;
	sigemptyset(&act.sa_mask);
	if (sigaction(SIGTRAP,  &act, 0) == -1) {
		perror("Unexpected error while attempting to setup test "
		       "pre-conditions");
		return PTS_UNRESOLVED;
	}

	if (sigaltstack((stack_t *)0, &current) == -1) {
		perror("Unexpected error while attempting to setup test "
		       "pre-conditions");
		return PTS_UNRESOLVED;
	}

	if (raise(SIGTRAP) == -1) {
		perror("Unexpected error while attempting to setup test "
		       "pre-conditions");
		return PTS_UNRESOLVED;
	}

	printf("Test PASSED\n");
	return PTS_PASS;
}
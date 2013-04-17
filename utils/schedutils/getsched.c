/*                               BSD License */

/*         Redistribution and use in source and binary forms, with or */
/*         without modification, are permitted provided that the following */
/*         conditions are met: */


/*         Redistributions of source code must retain the above */
/*         copyright notice, this list of conditions and the following */
/*         disclaimer. */

/*         Redistributions in binary form must reproduce the above */
/*         copyright notice, this list of conditions and the following */
/*         disclaimer in the documentation and/or other materials */
/*         provided with the distribution. */


/*       THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY */
/*       EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, */
/*       THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A */
/*       PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR */
/*       BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, */
/*       EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED */
/*       TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, */
/*       DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND */
/*       ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT */
/*       LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING */
/*       IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF */
/*       THE POSSIBILITY OF SUCH DAMAGE. */
#define _getsched_c_ 1
#include <stdlib.h>
#include <stdio.h>
#include <sched.h>
#include <errno.h>
#include <sys/types.h>
#ifdef HAVE_LIMITS_H
#include <limits.h>
#else
#define _POSIX_PATH_MAX 255
#endif
#include <string.h>
#include "util.h"
#include "config.h"
char progname[_POSIX_PATH_MAX];

void
print_usage (void)
{
  fprintf (stderr, "Usage : %s [-h] pid [pid, ...]\n\
Gets the scheduler and priority used for processes identified with the pids.\n\
Options:	-h print help (this text).\n\
		pid - which pid to reschedule.\n", progname);

}

int
main (int argc, char *argv[])
{
  pid_t thepid;
  int ret;
  struct sched_param oldpri;
  int oldsched;


  strcpy (progname, argv[0]);
  if (argc < 2)
    {
      print_usage ();
      return EXIT_FAILURE;
    }
  if (!strcmp ("-h", argv[1]))
    {
      print_usage ();
      return EXIT_FAILURE;
    }

  while (--argc > 0 && (*++argv)[0]){ 
    thepid = (pid_t) atoi (argv[0]);
    if (thepid < 0)
      {
        fprintf(stderr,"Failed: pid cannot be negative.\n");
        print_usage ();
        return EXIT_FAILURE;
      }

    ret = oldsched = sched_getscheduler (thepid);
    if (ret < 0)
      {
        perror ("Failed ");
        return EXIT_FAILURE;
      }
    ret = sched_getparam (thepid, &oldpri);
    if (ret < 0)
      {
        perror ("Failed ");
        return EXIT_FAILURE;
      }
    
    print_sched (thepid, oldsched, &oldpri);
  
  }
    return EXIT_SUCCESS;

}

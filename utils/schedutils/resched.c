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
#include "isnumber.h"
#include "config.h"
char progname[_POSIX_PATH_MAX];

void
print_usage (void)
{
  fprintf (stderr, "Usage : %s [-h] [-v] -r|-o|-f pid [priority]\n\
Sets a new scheduler and/or priority for process identified with pid.\n\
Options:	-v be verbose.\n\
		-h print help (this text).\n\
		-r|-o|-f which scheduler to set.\n\n\
		Schedulers are:\n\
		-r Round Robin scheduler.\n\
		-f FIFO scheduler.\n\
		-o Other(normal) scheduler.\n\
		\n\
		pid - which pid to reschedule.\n\
		priority - new priority to set.\n", progname);

}

int
main (int argc, char *argv[])
{
  pid_t thepid;
  int newpri;
  int ret;
  struct sched_param pri, oldpri;
  int oldsched, newsched = -1;
  int verbose = 0;

  char *s;
  strcpy (progname, argv[0]);
  if (argc < 2)
    {
      print_usage ();
      return EXIT_FAILURE;
    }
  while (--argc > 0 && (*++argv)[0] == '-')
    {
      for (s = argv[0] + 1; *s != '\0'; s++)
	{
	  switch (*s)
	    {
	    case 'h':
	      print_usage ();
	      return EXIT_FAILURE;
	      break;
	    case 'v':
	      verbose = 1;
	      break;
	    case 'r':
	      newsched = SCHED_RR;
	      break;
	    case 'f':
	      newsched = SCHED_FIFO;
	      break;
	    case 'o':
	      newsched = SCHED_OTHER;
	      break;
	    default:
	      fprintf (stderr,"Unrecognized option: %s\n", s);
	      print_usage ();
	      return EXIT_FAILURE;

	    }
	}
    }
  if (newsched < 0)
    {
      fprintf(stderr,"Which scheduler?\n");
      print_usage ();
      return EXIT_FAILURE;
    }
  if (argc--)
    {
      thepid = (pid_t) atoi (argv[0]);
    }
  else
    {
      fprintf (stderr,"Failed: pid not specified\n");
      print_usage ();
      return EXIT_FAILURE;
    }
  ret = sched_getparam (thepid, &oldpri);
  if (ret < 0)
    {
      perror ("Failed ");
      return EXIT_FAILURE;
    }
  newpri = oldpri.sched_priority;
  if (argc)
    {
      if (isnumber (*++argv))
	{
	  newpri = atoi (*argv);

	}
      else
	{
	  fprintf (stderr,"Failed: priority is not a number.\n");
	  return EXIT_FAILURE;
	}
    }
  if ((newpri > sched_get_priority_max (newsched)) || (newpri <
						       sched_get_priority_min
						       (newsched)))
    {
      fprintf (stderr,
	       "Priority %d is illegal, must be in range %d-%d for this scheduler.\n",
	       newpri, sched_get_priority_min (newsched),
	       sched_get_priority_max (newsched));
      return EXIT_FAILURE;
    }

  ret = oldsched = sched_getscheduler (thepid);
  if (ret < 0)
    {
      perror ("Failed ");
      return EXIT_FAILURE;
    }
  if (verbose)
    {
      printf ("Before: ");
      print_sched (thepid, oldsched, &oldpri);
    }
  pri.sched_priority = newpri;
  ret = sched_setscheduler (thepid, newsched, &pri);
  if (ret < 0)
    {
      perror ("Failed ");
      return EXIT_FAILURE;
    }

  if (verbose)
    {
      printf ("After : ");
      print_sched (thepid, newsched, &pri);
    }
  return EXIT_SUCCESS;


}

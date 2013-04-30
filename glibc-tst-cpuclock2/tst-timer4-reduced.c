/* Tests for POSIX timer implementation.
   Copyright (C) 2004-2013 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Jakub Jelinek <jakub@redhat.com>, 2004

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation; either version 2.1 of the
   License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; see the file COPYING.LIB.  If
   not, see <http://www.gnu.org/licenses/>.  */

#define _GNU_SOURCE

#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#if _POSIX_THREADS
# include <pthread.h>

# ifndef TEST_CLOCK
#  define TEST_CLOCK		CLOCK_REALTIME
# endif


timer_t timer_sig1;


volatile int sig1_cnt, sig1_err;
volatile union sigval sig1_sigval;
struct timespec sig1_ts;

static void
sig1_handler (int sig, siginfo_t *info, void *ctx)
{
  int err = 0;
  if (sig != SIGRTMIN) err |= 1 << 0;
  if (info->si_signo != SIGRTMIN) err |= 1 << 1;
  if (info->si_code != SI_TIMER) err |= 1 << 2;
  if (clock_gettime (TEST_CLOCK, &sig1_ts) != 0)
    err |= 1 << 3;
  if (sig1_cnt >= 5)
    {
      struct itimerspec it = { };
      if (timer_settime (timer_sig1, 0, &it, NULL))
	err |= 1 << 4;
    }
  sig1_err |= err;
  sig1_sigval = info->si_value;
  ++sig1_cnt;
}


/* Check if end is later or equal to start + nsec.  */
static int
check_ts (const char *name, const struct timespec *start,
	  const struct timespec *end, long msec)
{
  struct timespec ts = *start;

  ts.tv_sec += msec / 1000000;
  ts.tv_nsec += (msec % 1000000) * 1000;
  if (ts.tv_nsec >= 1000000000)
    {
      ++ts.tv_sec;
      ts.tv_nsec -= 1000000000;
    }
  if (end->tv_sec < ts.tv_sec
      || (end->tv_sec == ts.tv_sec && end->tv_nsec < ts.tv_nsec))
    {
      printf ("\
*** timer %s invoked too soon: %ld.%09ld instead of expected %ld.%09ld\n",
	      name, (long) end->tv_sec, end->tv_nsec,
	      (long) ts.tv_sec, ts.tv_nsec);
      return 1;
    }
  else
    return 0;
}

#define TIMEOUT 15
#define TEST_FUNCTION do_test ()

int main(void)
{
  int result = 0;

#ifdef TEST_CLOCK_MISSING
  const char *missing = TEST_CLOCK_MISSING (TEST_CLOCK);
  if (missing != NULL)
    {
      printf ("%s missing, skipping test\n", missing);
      return 0;
    }
#endif

  struct timespec ts;
  if (clock_gettime (TEST_CLOCK, &ts) != 0)
    {
      printf ("*** clock_gettime failed: %m\n");
      result = 1;
    }
  else
    printf ("clock_gettime returned timespec = { %ld, %ld }\n",
	    (long) ts.tv_sec, ts.tv_nsec);

  if (clock_getres (TEST_CLOCK, &ts) != 0)
    {
      printf ("*** clock_getres failed: %m\n");
      result = 1;
    }
  else
    printf ("clock_getres returned timespec = { %ld, %ld }\n",
	    (long) ts.tv_sec, ts.tv_nsec);

  struct sigevent ev;
  struct sigaction sa = { .sa_sigaction = sig1_handler,
			  .sa_flags = SA_SIGINFO };
  sigemptyset (&sa.sa_mask);
  sigaction (SIGRTMIN, &sa, NULL);

  memset (&ev, 0x22, sizeof (ev));
  ev.sigev_notify = SIGEV_SIGNAL;
  ev.sigev_signo = SIGRTMIN;
  ev.sigev_value.sival_ptr = &ev;
  if (timer_create (TEST_CLOCK, &ev, &timer_sig1) != 0)
    {
      printf ("*** timer_create for timer_sig1 failed: %m\n");
      return 1;
    }


  struct itimerspec it;
  it.it_value.tv_sec = 0;
  it.it_value.tv_nsec = -26;
  it.it_interval.tv_sec = 0;
  it.it_interval.tv_nsec = 0;

  struct timespec startts;
  if (clock_gettime (TEST_CLOCK, &startts) != 0)
    {
      printf ("*** clock_gettime failed: %m\n");
      result = 1;
    }

  it.it_value.tv_nsec = 400000000;
  if (timer_settime (timer_sig1, 0, &it, NULL) != 0)
    {
      printf ("*** timer_settime timer_sig1 failed: %m\n");
      result = 1;
    }

  while (sig1_cnt == 0)
    {
      ts.tv_sec = 0;
      ts.tv_nsec = 100 * 1000 * 1000;
      nanosleep (&ts, NULL);
    }

  if (sig1_cnt != 1)
    {
      printf ("*** sig1 not called exactly once, but %d times\n", sig1_cnt);
      result = 1;
    }
  else if (sig1_err)
    {
      printf ("*** errors occurred in sig1 handler %x\n", sig1_err);
      result = 1;
    }
  else if (sig1_sigval.sival_ptr != &ev)
    {
      printf ("*** sig1_sigval.sival_ptr %p != %p\n",
	      sig1_sigval.sival_ptr, &ev);
      result = 1;
    }
  else if (check_ts ("sig1", &startts, &sig1_ts, 400000))
    result = 1;


  if (timer_delete (timer_sig1) != 0)
    {
      printf ("*** timer_delete for timer_sig1 failed: %m\n");
      result = 1;
    }


  return result;
}
#else
# define TEST_FUNCTION 0
#endif



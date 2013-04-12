 #include <unistd.h>
 #include <stdio.h>
 #include <stdlib.h>
 #include <time.h>
 #include <fcntl.h>
 #include <string.h>
 #include <errno.h>
 #include <pthread.h>

static pthread_barrier_t barrier;

/* This function is intended to rack up both user and system time.  */
static void *
chew_cpu (void *arg)
{
  pthread_barrier_wait (&barrier);

  while (1)
    {
      static volatile char buf[4096];
      for (int i = 0; i < 100; ++i)
	for (size_t j = 0; j < sizeof buf; ++j)
	  buf[j] = 0xaa;
      int nullfd = open ("/dev/null", O_WRONLY);
      for (int i = 0; i < 100; ++i)
	for (size_t j = 0; j < sizeof buf; ++j)
	  buf[j] = 0xbb;
      write (nullfd, (char *) buf, sizeof buf);
      close (nullfd);
    }

  return NULL;
}

static unsigned long long int
tsdiff (const struct timespec *before, const struct timespec *after)
{
  struct timespec diff = { .tv_sec = after->tv_sec - before->tv_sec,
			   .tv_nsec = after->tv_nsec - before->tv_nsec };
  while (diff.tv_nsec < 0)
    {
      --diff.tv_sec;
      diff.tv_nsec += 1000000000;
    }
  return diff.tv_sec * 1000000000ULL + diff.tv_nsec;
}

int
main (void)
{
  int result = 0;
  clockid_t process_clock;
  int e;
  pthread_t th;

  e = clock_getcpuclockid (0, &process_clock);
  if (e != 0)
    {
      printf ("clock_getcpuclockid on self => %s\n", strerror (e));
      return 1;
    }

  pthread_barrier_init (&barrier, NULL, 2);

  e = pthread_create (&th, NULL, chew_cpu, NULL);
  if (e != 0)
    {
      printf ("pthread_create: %s\n", strerror (e));
      return 1;
    }
  pthread_barrier_wait (&barrier);

  struct timespec process_before;
  if (clock_gettime (process_clock, &process_before) < 0)
    {
      printf ("clock_gettime on process clock %lx => %s\n",
	      (unsigned long int) process_clock, strerror (errno));
      return 1;
    }

  const struct timespec sleeptime = { .tv_nsec = 100000000 };

  struct timespec sleeptimeabs = sleeptime;
  sleeptimeabs.tv_sec += process_before.tv_sec;
  sleeptimeabs.tv_nsec += process_before.tv_nsec;
  while (sleeptimeabs.tv_nsec >= 1000000000)
    {
      ++sleeptimeabs.tv_sec;
      sleeptimeabs.tv_nsec -= 1000000000;
    }
  e = clock_nanosleep (process_clock, TIMER_ABSTIME, &sleeptimeabs, NULL);
  if (e != 0)
    {
      return 1;
    }

  struct timespec process_after;
  if (clock_gettime (process_clock, &process_after) < 0)
    {
      return 1;
    }

  long long sleepdiff = (long long) tsdiff (&sleeptimeabs, &process_after);
  if (sleepdiff < 0)
    {
	    printf("absolute clock_nanosleep %llu past target (outside reasonable range)\n",
		   sleepdiff);
	    return 1;
    }

  unsigned long long int diffabs = tsdiff (&process_before, &process_after);
  if (diffabs < sleeptime.tv_nsec)
    {
      printf ("\
absolute clock_nanosleep slept %llu (outside reasonable range)\n",
	      diffabs);
      return 1;
    }

  return result;
}




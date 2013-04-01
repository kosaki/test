#include <time.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

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

int main(void)
{
	int e;
	clockid_t process_clock;
	clockid_t my_thread_clock;
	struct timespec process_before, process_after;
	struct timespec me_before, me_after;

	e = clock_getcpuclockid (0, &process_clock);
	if (e != 0)
	{
		printf ("clock_getcpuclockid on self => %s\n", strerror (e));
		return 1;
	}

	e = pthread_getcpuclockid (pthread_self (), &my_thread_clock);
	if (e != 0)
	{
		printf ("pthread_getcpuclockid on self => %s\n", strerror (e));
		return 1;
	}


	if (clock_gettime (process_clock, &process_before) < 0)
	{
		printf ("clock_gettime on process clock %lx => %s\n",
			(unsigned long int) process_clock, strerror (errno));
		return 1;
	}

	if (clock_gettime (my_thread_clock, &me_before) < 0)
	{
		printf ("clock_gettime on self thread clock %lx => %s\n",
			(unsigned long int) my_thread_clock, strerror (errno));
		return 1;
	}

	struct timespec sleeptime = { .tv_nsec = 500000000 };
	if (nanosleep (&sleeptime, NULL) != 0)
	{
		perror ("nanosleep");
		return 1;
	}

	if (clock_gettime (my_thread_clock, &me_after) < 0)
	{
		printf ("clock_gettime on self thread clock %lx => %s\n",
			(unsigned long int) my_thread_clock, strerror (errno));
		return 1;
	}

	if (clock_gettime (process_clock, &process_after) < 0)
	{
		printf ("clock_gettime on process clock %lx => %s\n",
			(unsigned long int) process_clock, strerror (errno));
		return 1;
	}

	unsigned long long int pdiff = tsdiff (&process_before, &process_after);
	unsigned long long int my_diff = tsdiff (&me_before, &me_after);

	if (my_diff > 100000000)
	{
		printf ("self thread before - after %llu outside reasonable range\n",
			my_diff);
		return 1;
	}

	if (pdiff < my_diff)
	{
		printf ("process before - after %llu outside reasonable range (%llu)\n",
			pdiff, my_diff);
		return 1;
	}

	return 0;
}

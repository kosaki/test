#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdint.h>
#include <limits.h>
#include <getopt.h>

#define min_t(type, x, y) ({			\
	type __min1 = (x);			\
	type __min2 = (y);			\
	__min1 < __min2 ? __min1 : __min2; })

static int		pagemap_fd;
static pid_t		opt_pid;	/* process to walk */
static int		page_size;

static unsigned long	start_addr = 0x700000000000; /* x86_64 default */
static unsigned long	end_addr   = 0x800000000000; /* x86_64 default */

static void fatal(const char *x, ...)
{
	va_list ap;

	va_start(ap, x);
	vfprintf(stderr, x, ap);
	va_end(ap);
	exit(EXIT_FAILURE);
}

static unsigned long long parse_number(const char *str)
{
	unsigned long long n;

	n = strtoll(str, NULL, 0);

	if (n == 0 && str[0] != '0')
		fatal("invalid name or number: %s\n", str);

	return n;
}

static int checked_open(const char *pathname, int flags)
{
	int fd = open(pathname, flags);

	if (fd < 0) {
		perror(pathname);
		exit(EXIT_FAILURE);
	}

	return fd;
}

static void parse_pid(const char *str)
{
	char buf[5000];

	opt_pid = parse_number(str);

	sprintf(buf, "/proc/%d/pagemap", opt_pid);
	pagemap_fd = checked_open(buf, O_RDONLY);
}


static unsigned long do_u64_read(int fd, char *name,
				 uint64_t *buf,
				 unsigned long index,
				 unsigned long count)
{
	long bytes;

	if (index > ULONG_MAX / 8)
		fatal("index overflow: %lu\n", index);

	if (lseek(fd, index * 8, SEEK_SET) < 0) {
		perror(name);
		exit(EXIT_FAILURE);
	}

	bytes = read(fd, buf, count * 8);
	if (bytes < 0) {
		perror(name);
		exit(EXIT_FAILURE);
	}
#if 1 /* RHEL6 hack */
	if (bytes == 0) {
		bytes = count * 8;
	}
#endif

	if (bytes % 8)
		fatal("partial read: %lu bytes\n", bytes);

	return bytes / 8;
}

static unsigned long pagemap_read(uint64_t *buf,
				  unsigned long index,
				  unsigned long count)
{
	return do_u64_read(pagemap_fd, "/proc/pid/pagemap", buf, index, count);
}

#define PAGEMAP_BATCH	(64 << 10)
static void walk_pagemap_range(unsigned long index, unsigned long count)
{
	uint64_t buf[PAGEMAP_BATCH];
	unsigned long batch;
	unsigned long pages;

	while (count) {
		batch = min_t(unsigned long, count, PAGEMAP_BATCH);
		pages = pagemap_read(buf, index, batch);
		if (pages == 0)
			break;

		index += pages;
		count -= pages;
	}
}


static void usage(void)
{
	/* TBD */
	fprintf(stderr, "read_pagemap -p <pid>\n");
	exit(1);
}

int main(int argc, char* argv[])
{
	int c;
	unsigned long index;
	unsigned long end_index;
	unsigned long count;

	page_size = getpagesize();

	while ((c = getopt_long(argc, argv,
				"p:s:e:", NULL, NULL)) != -1) {
		switch (c) {
		case 'p':
			parse_pid(optarg);
			break;
		case 's':
			start_addr = parse_number(optarg);
			break;
		case 'e':
			end_addr = parse_number(optarg);
			break;

		default:
			usage();
			exit(1);
		}
	}

	if (opt_pid == 0) {
		usage();
		exit(1);
	}

	index = start_addr / page_size;
	end_index = end_addr / page_size;
	count = end_index - index;
	walk_pagemap_range(index, count);

	return 0;
}

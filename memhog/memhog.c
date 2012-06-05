#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static unsigned long long memparse(const char *ptr, char **retptr)
{
	char *endptr;	/* local pointer to end of parsed string */

	unsigned long long ret = strtoull(ptr, &endptr, 0);

	switch (*endptr) {
	case 'G':
	case 'g':
		ret <<= 10;
	case 'M':
	case 'm':
		ret <<= 10;
	case 'K':
	case 'k':
		ret <<= 10;
		endptr++;
	default:
		break;
	}

	if (retptr)
		*retptr = endptr;

	return ret;
}

void usage(void)
{
	fprintf(stderr, "usage: memhog [options] size[KMG]\n");
	fprintf(stderr, " -h: help\n");
	fprintf(stderr, " -r: Repeated touch\n");
	exit(1);
}

int main(int argc, char **argv)
{
	unsigned long long size;
	void *ptr;
	int ch;
	extern char	*optarg;
	extern int	optind, opterr;
	int repeat = 0;

	while ((ch = getopt(argc, argv, "rh")) != -1){
		switch (ch){
		case 'r': /* repeat touch */
			repeat = 1;
			break;
		case 'h':
		default:
			usage();
		}
	}
	argc -= optind;
	argv += optind;


	if (argc != 1)
		usage();

	size = memparse(argv[0], NULL);
	ptr = malloc(size);
	if (!ptr) {
		perror("malloc");
		exit(1);
	}

	if (repeat)
		for(;;)
			memset(ptr, 0, size);
	else {
		memset(ptr, 0, size);
		pause();
	}

	return 0;
}

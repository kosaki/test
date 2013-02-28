#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

main()
{
	int err;

	printf("getpid()=%d\n", getpid());
	printf("getpgid()=%d\n", getpgid(0));
	err = setsid();
	printf("%d, %d\n", err, errno);
	return 0;
}

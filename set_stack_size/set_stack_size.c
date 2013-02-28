#include <sys/time.h>
#include <sys/resource.h>
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
	struct rlimit rlim;
	int err;
	
	err = getrlimit(RLIMIT_STACK, &rlim);
	if (err < 0) {
		perror("getrlimit ");
		exit(1);
	}
	printf("%ld %ld\n", rlim.rlim_cur, rlim.rlim_max);

	rlim.rlim_cur -= 1;
	setrlimit(RLIMIT_STACK, &rlim);
	if (err < 0) {
		perror("setrlimit ");
		exit(1);
	}

	err = getrlimit(RLIMIT_STACK, &rlim);
	if (err < 0) {
		perror("getrlimit ");
		exit(1);
	}
	printf("%ld %ld\n", rlim.rlim_cur, rlim.rlim_max);

	return 0;
}

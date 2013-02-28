#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <numaif.h>
#include <errno.h>

/* gcc mbind_test.c -lnuma -o mbind_test -Wall */
#define MAXNODE 4096

int main()
{
	int ret;
	int len;
	int policy = -1;
	unsigned char *p;
	unsigned long mask[MAXNODE] = { 0 };
	unsigned long retmask[MAXNODE] = { 0 };

	len = getpagesize();
	p = mmap(NULL, len, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS,
		 0, 0);
	if (p == MAP_FAILED)
		printf("mbind err: %d\n", errno);

	mask[0] = 1;
	ret = mbind(p, len, MPOL_BIND, mask, MAXNODE, 0);
	if (ret < 0)
		printf("mbind err: %d %d\n", ret, errno);
	ret = get_mempolicy(&policy, retmask, MAXNODE, p, MPOL_F_ADDR);
	if (ret < 0)
		printf("get_mempolicy err: %d %d\n", ret, errno);

	if (policy == MPOL_BIND)
		printf("OK\n");
	else
		printf("ERROR: policy is %d\n", policy);

	return 0;
}


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/mman.h>

int pagesize;

#define NR_MAPS 65536
void *thr_func(void* arg)
{
	void* addr;
	int ret;
	int i;
	void* maps[NR_MAPS];

again:
	for(i = 0; i < NR_MAPS; i++) {
		addr = maps[i] = mmap(NULL, pagesize*2,
				      PROT_READ|PROT_WRITE,
				      MAP_PRIVATE|MAP_ANON,
				      -1, 0);
		if (addr == MAP_FAILED)
			perror("mmap"), exit(1);

		/* changes prot for preventing vma merge. */
		mprotect(addr, pagesize, PROT_NONE);
	}
	for(i = 0; i< NR_MAPS; i++) {
		ret = munmap(maps[i], pagesize);
		if (ret < 0)
			perror("munmap"), exit(1);
	}

	goto again;

	return NULL; /* never reached here */
}

/* make a lot of unused maps. we need tons vma
 * for increasing race window.
 */
void make_long_vma_list(void)
{
	int i;
	void *addr;

	for(i = 0; i < 100000; i++) {
		addr = mmap(NULL, pagesize*2, PROT_READ|PROT_WRITE,
			    MAP_PRIVATE|MAP_ANON,
			    -1, 0);
		if (addr == MAP_FAILED)
			perror("mmap"), exit(1);

		/* changes prot for preventing vma merge. */
		mprotect(addr, pagesize, PROT_NONE);
	}


}

int main(void)
{
	pthread_t thr;
	int i;

	printf("pid=%d\n", getpid());
	pagesize = getpagesize();
	make_long_vma_list();

	for (i=0; i < 32; i++) {
		pthread_create(&thr, NULL, thr_func, NULL);
	}

	pause();

	return 0;
}

#include <sys/mman.h>
#include <stdlib.h>

#ifndef MAP_HUGETLB
#define MAP_HUGETLB 0x0040000
#endif

int main() {
	int i;

	for (i=0; i!=10000000; ++i) {
		void* ptr=mmap(NULL, 2*1024*1024, PROT_READ|PROT_WRITE,
			       MAP_PRIVATE|MAP_ANONYMOUS|MAP_HUGETLB, 0, 0);
		if (ptr!=MAP_FAILED)
			abort();
	}
	return 0;
}


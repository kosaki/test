#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <sys/time.h>
#include <sys/mman.h>

#define MM_SZ1 24
#define MM_SZ2 56
#define MM_SZ3 4168

void mlock_test()
{
	char ptr1[MM_SZ1];
	char ptr2[MM_SZ2];
	char ptr3[MM_SZ3];

	if(0 != mlock(ptr1, MM_SZ1) )
		perror("mlock MM_SZ1\n");
	if(0 != mlock(ptr2, MM_SZ2) )
		perror("mlock MM_SZ2\n");
	if(0 != mlock(ptr3, MM_SZ3) )
		perror("mlock MM_SZ3\n");

	if(0 != munlock(ptr1, MM_SZ1) )
		perror("munlock MM_SZ1\n");
	if(0 != munlock(ptr2, MM_SZ2) )
		perror("munlock MM_SZ2\n");
	if(0 != munlock(ptr3, MM_SZ3) )
		perror("munlock MM_SZ3\n");
}

int main(int argc, char *argv[])
{
	int ret, opt;
	int i,cnt;

	while((opt = getopt(argc, argv, "c:")) != -1 )
	{
		switch(opt){
		case 'c':
			cnt = atoi(optarg);
			break;
		default:
			printf("Usage: %s [-c count] arg...\n", argv[0]);
			exit(EXIT_FAILURE);
		}
	}

	for(i = 0; i < cnt; i++)
		mlock_test();

	return 0;
}

#include <unistd.h>
#include <stdio.h>
#include <errno.h>

int main(void)
{
	if ((void*)-1 == sbrk(2048))
		perror("sbrk");

	return 0;
}




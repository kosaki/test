#include <sys/select.h>
#include <stdlib.h>

int main(void)
{
	fd_set *set;

	set = malloc(sizeof(fd_set)*2);
	FD_SET(1025, set);

#if 0
	__fdelt_check_warn(1025);
#endif

	return 0;
}

 #include <sys/mman.h>
 #include <sys/stat.h>
 #include <fcntl.h>
 #include <unistd.h>
 #include <sys/types.h>
 #include <sys/mman.h>
 #include <stdio.h>
 #include <errno.h>
 #include <stdlib.h>
 #include <numaif.h>
 #include <numa.h>


void* shmem_open(void)
{
	void* ptr;
	int fd;
	char* filename = "/shm_mempolicy";
	int err;
	int file_len = 4096 * 3;

	fd = shm_open(filename, O_RDWR|O_CREAT|O_TRUNC, 0622);
	if (fd < 0)
		perror("shm_open"), exit(1);
	(void)shm_unlink(filename);

	err = ftruncate(fd, file_len);
	if (err < 0)
		perror("ftruncate"), exit(1);

	ptr = mmap(NULL, file_len, PROT_READ|PROT_WRITE, MAP_SHARED,
		   fd, 0);
	if (ptr == MAP_FAILED)
		perror("mmap"), exit(1);

	return ptr;
}

int main(void)
{
	void* ptr;
	struct bitmask *nmask;
	int err;

	nmask = numa_allocate_nodemask();
	numa_bitmask_setbit(nmask, 0);

	ptr = shmem_open();

	err = mbind(ptr, 4096 * 3, MPOL_INTERLEAVE,
		    nmask->maskp, nmask->size, 0);
	if (err < 0)
		perror("mbind1"), exit(1);

	err = mbind(ptr + 4096, 4096, MPOL_BIND,
		    nmask->maskp, nmask->size, 0);
	if (err < 0)
		perror("mbind1"), exit(1);

	return 0;
}

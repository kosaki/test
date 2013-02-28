 #define _XOPEN_SOURCE 600
 #include <sys/types.h>
 #include <sys/stat.h>
 #include <fcntl.h>
 #include <unistd.h>
 #include <fcntl.h>
 #include <stdlib.h>
 #include <errno.h>

main()
{
	int err;
	int fd;
	char buf[4096] = {0};

	fd = open("/dev/shm/foo.txt", O_CREAT|O_RDWR, 0644);
	if (fd < 0) {
		perror("open ");
		exit(1);
	}

	err = posix_fadvise(fd, 0, 4096, POSIX_FADV_WILLNEED);
	if (err) {
		errno = err;
		perror("fadvise(WILLNEEED) ");
		exit(1);
	}

	return 0;
}

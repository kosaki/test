#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

#define SP_VALUE(m,n,p)	(((double) ((n) - (m))) / (p) * 100)

int nr_online_cpus(void)
{
	return sysconf(_SC_NPROCESSORS_ONLN);
}

double ll_sp_value(unsigned long long value1, unsigned long long value2,
		   unsigned long long itv)
{
	if ((value2 < value1) && (value1 <= 0xffffffff))
		/* Counter's type was unsigned long and has overflown */
		return ((double) ((value2 - value1) & 0xffffffff)) / itv * 100;
	else
		return SP_VALUE(value1, value2, itv);
}

double ll_sp_value2(unsigned long long value1, unsigned long long value2,
		   unsigned long long itv)
{
	if (value2 < value1)
		return 0;
	else
		return SP_VALUE(value1, value2, itv);
}

int main(int argc, char *argv[])
{
	int fd;
	unsigned long long *prev;
	unsigned long long cur[10];
	static const char * const statname[] = {
		"user", "nice", "system", "idle", "iowait", "irq", "softirq",
		"steal", "guest", "guest_nice"
	};
	int cpu;
	int i;

	fd = open("/proc/stat", O_RDONLY);
	if (fd < 0)
		perror("open "), exit(1);

	prev = calloc(nr_online_cpus()*10, sizeof(unsigned long long));
	if (!prev)
		perror("allocation failure "), exit(1);

	while (1) {
		char buffer[100*1024];
		char *buf = buffer;
		ssize_t ret = pread(fd, buffer, sizeof(buffer) - 1, 0);
		if (ret <= 0)
			break;
		buffer[ret] = '\0';
		for (cpu = 0; cpu < nr_online_cpus(); cpu++) {
			char *eol = strchr(buf, '\n');
			if (!eol)
				break;
			*eol++ = '\0';
			if (strncmp(buf, "cpu", 3))
				break;
			while (*buf && *buf != ' ')
				buf++;
			memset(cur, 0, sizeof(cur));
			sscanf(buf, "%llu %llu %llu %llu %llu %llu %llu %llu "
			       "%llu %llu", &cur[0], &cur[1], &cur[2], &cur[3],
			       &cur[4], &cur[5], &cur[6], &cur[7], &cur[8],
			       &cur[9]);
			for (i = 0; i < 10; i++) {
				if (prev[cpu*10 + i] > cur[i])
					printf("cpu[%d].%-10s : "
					       "%llu -> %llu : %f %f\n",
					       cpu,
					       statname[i], prev[cpu*10 + i], cur[i],
					       ll_sp_value(prev[cpu*10 + i], cur[i], 1),
					       ll_sp_value2(prev[cpu*10 + i], cur[i], 1)
						);
				prev[cpu*10 + i] = cur[i];
			}
			buf = eol;
		}
	}
	return 0;
}

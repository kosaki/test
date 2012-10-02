#include <stdio.h>
#include <pthread.h>

extern void* __libc_stack_end;

struct pthread_attr
{
  /* Scheduler parameters and priority.  */
  struct sched_param schedparam;
  int schedpolicy;
  /* Various flags like detachstate, scope, etc.  */
  int flags;
  /* Size of guard area.  */
  size_t guardsize;
  /* Stack handling.  */
  void *stackaddr;
  size_t stacksize;
  /* Affinity map.  */
  cpu_set_t *cpuset;
  size_t cpusetsize;
};

int main()
{
	pthread_attr_t attr;
	void *addr;
	size_t size;
	struct pthread_attr *iattr = (struct pthread_attr *) &attr;

	pthread_getattr_np(pthread_self(), &attr);
	pthread_attr_getstack(&attr, &addr, &size);


	printf("%p, %ld, %p\n", iattr->stackaddr, iattr->stacksize, __libc_stack_end);
	printf("%p, %ld\n", addr, size);
	return 0;
}

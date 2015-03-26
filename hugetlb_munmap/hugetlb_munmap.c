/*
  Picked from LKML thread. (Subject: [patch][resend] MAP_HUGETLB munmap fails with size not 2MB aligned)
  Author: Davide Libenzi <davidel@xmailserver.org>
*/


#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

static void test(int flags, size_t size)
{
    void* addr = mmap(NULL, size, PROT_READ | PROT_WRITE,
                      flags | MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    if (addr == MAP_FAILED)
    {
        perror("mmap");
        exit(1);
    }
    *(char*) addr = 17;

    if (munmap(addr, size) != 0)
    {
        perror("munmap");
        exit(1);
    }
}

int main(int ac, const char** av)
{
    static const size_t hugepage_size = 2 * 1024 * 1024;

    printf("Testing normal pages with 2MB size ...\n");
    test(0, hugepage_size);
    printf("OK\n");

    printf("Testing huge pages with 2MB size ...\n");
    test(MAP_HUGETLB, hugepage_size);
    printf("OK\n");


    printf("Testing normal pages with 4KB byte size ...\n");
    test(0, 4096);
    printf("OK\n");

    printf("Testing huge pages with 4KB byte size ...\n");
    test(MAP_HUGETLB, 4096);
    printf("OK\n");


    printf("Testing normal pages with 1 byte size ...\n");
    test(0, 1);
    printf("OK\n");

    printf("Testing huge pages with 1 byte size ...\n");
    test(MAP_HUGETLB, 1);
    printf("OK\n");

    return 0;
}

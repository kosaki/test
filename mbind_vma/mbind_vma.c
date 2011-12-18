#include <numaif.h>
#include <numa.h>
#include <sys/mman.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

static unsigned long pagesize;
void* mmap_addr;
struct bitmask *nmask;
char buf[1024];
FILE *file;
char retbuf[10240] = "";

char *rubysrc = "ruby -e '\
  pid = %d; \
  vstart = 0x%llx; \
  vend = 0x%llx; \
  s = `pmap -q #{pid}`; \
  rary = []; \
  s.each_line {|line|; \
    ary=line.split(\" \"); \
    addr = ary[0].to_i(16); \
    if(vstart <= addr && addr < vend) then \
      rary.push(ary[1].to_i()/4); \
    end; \
  }; \
  print rary.join(\",\"); \
'";

void init(void)
{
	void* addr;

	nmask = numa_allocate_nodemask();
	numa_bitmask_setbit(nmask, 0);

	pagesize = getpagesize();

	addr = mmap(NULL, pagesize*8, PROT_NONE,
		    MAP_ANON|MAP_PRIVATE, 0, 0);
	if (addr == MAP_FAILED)
		perror("mmap "), exit(1);

	if (mmap(addr+pagesize, pagesize*6, PROT_READ|PROT_WRITE,
		 MAP_ANON|MAP_PRIVATE|MAP_FIXED, 0, 0) == MAP_FAILED)
		perror("mmap "), exit(1);

	mmap_addr = addr + pagesize;

	/* make page populate */
	memset(mmap_addr, 0, pagesize*6);
}

void fin(void)
{
	void* addr = mmap_addr - pagesize;
	munmap(addr, pagesize*8);

	memset(buf, 0, sizeof(buf));
	memset(retbuf, 0, sizeof(retbuf));
}

void mem_bind(int index, int len)
{
	int err;

	err = mbind(mmap_addr+pagesize*index, pagesize*len,
		    MPOL_BIND, nmask->maskp, nmask->size, 0);
	if (err)
		perror("mbind "), exit(err);
}

void mem_interleave(int index, int len)
{
	int err;

	err = mbind(mmap_addr+pagesize*index, pagesize*len,
		    MPOL_INTERLEAVE, nmask->maskp, nmask->size, 0);
	if (err)
		perror("mbind "), exit(err);
}

void mem_unbind(int index, int len)
{
	int err;

	err = mbind(mmap_addr+pagesize*index, pagesize*len,
		    MPOL_DEFAULT, NULL, 0, 0);
	if (err)
		perror("mbind "), exit(err);
}

void Assert(char *expected, char *value, char *name, int line)
{
	if (strcmp(expected, value) == 0) {
		fprintf(stderr, "%s: passed\n", name);
		return;
	}
	else {
		fprintf(stderr, "%s: %d: test failed. expect '%s', actual '%s'\n",
			name, line,
			expected, value);
//		exit(1);
	}
}

/*
      AAAA
    PPPPPPNNNNNN
    might become
    PPNNNNNNNNNN
    case 4 below
*/
void case4(void)
{
	init();
	sprintf(buf, rubysrc, getpid(), mmap_addr, mmap_addr+pagesize*6);

	mem_bind(0, 4);
	mem_unbind(2, 2);

	file = popen(buf, "r");
	fread(retbuf, sizeof(retbuf), 1, file);
	Assert("2,4", retbuf, "case4", __LINE__);

	fin();
}

/*
       AAAA
 PPPPPPNNNNNN
 might become
 PPPPPPPPPPNN
 case 5 below
*/
void case5(void)
{
	init();
	sprintf(buf, rubysrc, getpid(), mmap_addr, mmap_addr+pagesize*6);

	mem_bind(0, 2);
	mem_bind(2, 2);

	file = popen(buf, "r");
	fread(retbuf, sizeof(retbuf), 1, file);
	Assert("4,2", retbuf, "case5", __LINE__);

	fin();
}

/*
	    AAAA
	PPPPNNNNXXXX
	might become
	PPPPPPPPPPPP 6
*/
void case6(void)
{
	init();
	sprintf(buf, rubysrc, getpid(), mmap_addr, mmap_addr+pagesize*6);

	mem_bind(0, 2);
	mem_bind(4, 2);
	mem_bind(2, 2);

	file = popen(buf, "r");
	fread(retbuf, sizeof(retbuf), 1, file);
	Assert("6", retbuf, "case6", __LINE__);

	fin();
}

/*
    AAAA
PPPPNNNNXXXX
might become
PPPPPPPPXXXX 7
*/
void case7(void)
{
	init();
	sprintf(buf, rubysrc, getpid(), mmap_addr, mmap_addr+pagesize*6);

	mem_bind(0, 2);
	mem_interleave(4, 2);
	mem_bind(2, 2);

	file = popen(buf, "r");
	fread(retbuf, sizeof(retbuf), 1, file);
	Assert("4,2", retbuf, "case7", __LINE__);

	fin();
}

/*
    AAAA
PPPPNNNNXXXX
might become
PPPPNNNNNNNN 8
*/
void case8(void)
{
	init();
	sprintf(buf, rubysrc, getpid(), mmap_addr, mmap_addr+pagesize*6);

	mem_bind(0, 2);
	mem_interleave(4, 2);
	mem_interleave(2, 2);

	file = popen(buf, "r");
	fread(retbuf, sizeof(retbuf), 1, file);
	Assert("2,4", retbuf, "case8", __LINE__);


	fin();
}

int main(int argc, char** argv)
{
	case4();
	case5();
	case6();
	case7();
	case8();

	return 0;
}

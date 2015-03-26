#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <errno.h>
#include <stdio.h>
main(int argc,char *argv[])
{
         unsigned long siz, procs, itterations;
         char    *ptr1;
         char    *i;
         int     pid, j, k, status;

         if ((argc <= 1)||(argc >4)) {
                 printf("bad args, usage: forkoff <memsize GB#children #itterations\n");
                 exit(-1);
         }
         siz = ((long)atol(argv[1])*1024*1024*1024);
         procs = atol(argv[2]);
         itterations = atol(argv[3]);
         printf("mmaping %ld anonymous bytes\n", siz);
         ptr1 = (char *)mmap((void *)0,siz,PROT_READ|PROT_WRITE,MAP_ANONYMOUS|MAP_PRIVATE,-1,0);
         if (ptr1 == (char *)-1) {
                 printf("ptr1 = %lx\n", ptr1);
                 perror("");
         }
         printf("forking %ld processes\n", procs);
         k = procs;
         do{
                 pid = fork();
                 if (pid == -1) {
                         printf("fork failure\n");
                         exit(-1);
                 } else if (!pid) {
                         //printf("PID %d touching %d pages\n", getpid(), siz/4096);

                         for (j=0; j<itterations; j++) {
                                 for (i=ptr1; i<ptr1+siz-1; i+=4096) {
                                                 *i=(char)'i';
                                 }
                         }
                         //printf("All done, exiting\n");
                         exit(0);
                 }

         } while(--k);

         while (procs-- && wait(&status));
}


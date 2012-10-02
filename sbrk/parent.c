#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

int main(int argc, char **argv){
	pid_t pid;
	int status;
	pid = fork();
	if (pid == 0){
		execl("./sbrk_test", "sbrk_test", NULL);
		printf("exec error\n");
		exit(-1);
	}else{
		waitpid(pid, &status, 0);
		if (WIFEXITED(status)){
			printf("exit, status=%d\n", WEXITSTATUS(status));
		} else if (WIFSIGNALED(status)){
			printf("signal, sig=%d\n", WTERMSIG(status));
		} else {
			printf("abnormal exit\n");
		}
		exit(0);
	}
	return (0);
}

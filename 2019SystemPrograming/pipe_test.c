#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>






int main() {
	int status;
	// 0: stdin, 1: stdout
	// fd1 : stdin, fd2 : stdout
	int fd1, fd2;
	int pipeA[2];
	char* args[100];
	args[0] = "ls";
	

	if(pipe(pipeA) == -1){
		perror("error");	
	};

	if(fork() == 0) {
		printf("hi\n");
		dup2(pipeA[0],1);
		close(1);
		execv("/bin/ls",args);
	}
	else{
		wait(&status);
		dup2(0,pipeA[1]);
		
	
	}

	return 0;
}

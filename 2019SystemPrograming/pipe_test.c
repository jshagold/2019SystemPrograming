#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <sys/types.h>





int main() {
	int status;
	// 0: stdin, 1: stdout
	// in pipe, 1:input, 0:output
	int pipeC[2];
	char* args[2];
	args[0] = "ls";
	char* grep_args[3];
	grep_args[0] = "grep";
	grep_args[1] = "test";
	//grep_args[1] = "^d";
	//grep_args[2] = "test";


	pid_t pid[2];
	pipe(pipeC);
/*

	
	for(int i=0; i<2; i++) {
		if((pid[i] = fork()) == 0){
			if(i==0) {
				printf("child%d\n",i);
				close(pipeC[0]);
				dup2(pipeC[1], 1);
				close(pipeC[1]);
				execv("/bin/ls", args);
			}
			else {
				waitpid(pid[0],NULL,0);
				printf("child%d\n",i);
				close(pipeC[1]);
				dup2(pipeC[0],0);
				close(pipeC[0]);
				execv("/bin/grep", grep_args);
			}
		}
		else{
			waitpid(pid[1],NULL,0);
			printf("parent\n");

		}
	}
*/	

	if((pid[0] = fork()) == 0){
		printf("child%d\n",0);
		close(pipeC[0]);
		dup2(pipeC[1], 1);
		close(pipeC[1]);
		execl("/bin/ls", "ls", NULL);
	}
	
	if((pid[1] = fork()) == 0){
		waitpid(pid[0],NULL,0);
		printf("child%d\n",1);
		close(pipeC[1]);
		dup2(pipeC[0], 0);
		close(pipeC[0]);
		execl("/bin/grep", "grep", "test", NULL);
	}

	close(pipeC[0]);
	close(pipeC[1]);
	waitpid(pid[1],NULL,0);
	printf("parent\n");
	return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_LEN 100

int main(void) {
	char *args[MAX_LEN / 2];
	char *input, *command;
	char cd_buf[255];
	
	chdir("/");

	while(1) {
		int i = 0;
		int check_int = 0;
		getcwd(cd_buf,255);
		printf("my_cd:%s> ",cd_buf);
		fflush(stdout);

		input = (char*)malloc(sizeof(char)*MAX_LEN);
		fgets(input, MAX_LEN, stdin);

		command = strtok(input, "\n");
		args[i] = strtok(command," ");
		while(args[i] != NULL){
			i++;
			args[i] = strtok(NULL, " ");
			printf("args[%d]=> %s\n",i,args[i]);
			
		}


		if(args[0] == NULL){
			goto no_input;
		}
		if(strcmp(args[0], "exit") == 0){
			break;
		}
		else if(strcmp(args[0], "cd") == 0){
			if(chdir(args[1])){
				perror("error");
				exit(1);
			}
		}
		else if(strcmp(args[0], "ls") == 0){
			if(fork() == 0) {
				execv("/bin/ls",args);
			}
		}
		no_input:;
	}
	free(input);
	return 0;
}

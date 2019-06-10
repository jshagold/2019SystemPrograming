#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
 
int run_command(char* command);
void redirect(char* f_command, char* b_command, char* delimiter);
void pipe_command(char* command);
char* filename(char* command);

int main() {
	// && & ; > >> < >| |
	char* command[100];
	command[0] = "cat";
	command[1] = ";";
	command[2] = "hi.txt";
	command[3] = " hello.txt ";
	command[4] = NULL;
	int i = 0;
	char* test;

	test = filename(" hello.txt");
	printf("filename : !!!%s!!!\n", test);

	while(command[i] != NULL) {
		if(command[i+1] == NULL) {
			run_command(command[i]);
			break;
		}
		else if(strcmp(command[i+1], ";") == 0) {
			run_command(command[i]);
			i = i+2;
		}
		else if(strcmp(command[i+1], "&") == 0) {
			if(fork() == 0) {
				run_command(command[i]);
			}
			i = i+2;
		}
		else if(strcmp(command[i+1], "&&") == 0) {
			if(run_command(command[i]) == 1) {
				i = i+4;
			}
			else {
				run_command(command[i]);
				i = i+2;
			}
		}
		else if((strcmp(command[i+1],">")==0) || (strcmp(command[i+1],">>")==0) || (strcmp(command[i+1],"<")==0) || (strcmp(command[i+1],">|")==0)) {
			redirect(command[i],command[i+2],command[i+1]);
			i = i+4;
		}
		else if(strcmp(command[i+1], "|") == 0) {
			pipe_command(command[i]);
		}

	}


	return 0;
}
 

int run_command(char* command) {
	printf("1111111111\n");
	printf("running : !!%s!!\n",command);
	return 0;
}

void redirect(char* f_command, char* b_command, char* delimiter) {
	if(b_command == NULL) {
		perror("redirection error");
	}
	else {
		if(strcmp(delimiter, ">") == 0) {
			int fd;
			fd = open(b_command,O_WRONLY|O_CREAT,0666);
			dup2(fd,1);
			run_command(f_command);
			close(fd);
		}
		else if(strcmp(delimiter, ">>") == 0) {
			int fd;
			fd = open(b_command,O_RDWR|O_CREAT|O_APPEND,0666);
			dup2(fd,1);
			run_command(f_command);
			close(fd);
		}
		else if(strcmp(delimiter, "<") == 0) {
			int fd;
			fd = open(b_command,O_RDONLY,0666);
			dup2(fd,0);
			run_command(f_command);
			close(fd);
		}
		else if(strcmp(delimiter, ">|") == 0) {
			int fd;
		}

	}
}
char* filename(char* command) {
        char* check;
        char* result;

        if(command == NULL) {
                result = NULL;
        }
        else{
                check = strtok(command," ");
                if(check != NULL) {     
                        result = check;
                        return result;
                }
                else {
                        check = strtok(NULL," ");
                        result = check;
                }
        }
        return result;
}


void pipe_command(char* command) {

}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>

#define MAX_LEN 100
#define BUF_SIZE 256


// seperate command function
int run_command(char* args[MAX_LEN], char* host_dir);
// seperate string by &&
char** byand(char* origin);
// return command array using by byand function
char** auto_seper(char* command);



int main(void) {

	// origin command(= input)
	char *input, *command;
	// set of seperated command by &&
	char* D_and[MAX_LEN];
	// set of seperated command by &&,;
	char* semi[MAX_LEN];
	// set of seperated command by &&,;,&
	char* parsed[MAX_LEN];
	char* string;
	char* string2;
	// current working directory
	char cd_buf[BUF_SIZE];
	
	// initialize directory to user directory
	char* username = getenv("USER");
	char* host_dir = (char *)malloc(128);
	sprintf(host_dir, "/home/%s", username);
	chdir(host_dir);


	while(1) {
		int i = 0, j = 0, k = 0;
		int check_int = 0;
		getcwd(cd_buf,255);
		printf("my_cd:%s> ",cd_buf);
		fflush(stdout);

		input = (char*)malloc(sizeof(char)*MAX_LEN);
		fgets(input, MAX_LEN, stdin);

		command = strtok(input, "\n");

		// array seperated by &&
		char* result_semi[100];
		char* result_final[100];

		if(command != NULL){
			D_and[i] = command;
			do{
				string2 = D_and[i];

				string = strstr(D_and[i],"&&");

				if(string != NULL) {
					memset(string, '\0', 2);
					D_and[i] = string2;
					D_and[i+1] = "&&";
					D_and[i+2] = string+2;
				}
				else {
					D_and[i] = string2;
					D_and[i+1] = NULL;
					break;
				}
				i=i+2;
			}
			while(D_and[i] != NULL);

			// command seperated by &&, ;
			i = 0;
			while(D_and[j] != NULL) {
				char* semi_test;
				semi_test = malloc(sizeof(char)*100);
				char* semi_check;
				semi_check = malloc(sizeof(char)*100);
				strcpy(semi_test,D_and[j]);

				semi[i] = strtok(semi_test,";");
				while(semi[i] != NULL) {
					i++;
					semi_check = strtok(NULL, ";");
					if(semi_check != NULL){
						semi[i] = ";";
						i++;
						semi[i] = semi_check;
					}
					else {
						semi[i] = NULL;
					}
				}
				j++;
			}

			// command seperated by &&, ;, &
			i=0;
			j=0;
			while(semi[j] != NULL) {
				char* and_test;
				and_test = malloc(sizeof(char)*100);
				char* and_check;
				and_check = malloc(sizeof(char)*100);
				if(strcmp(semi[j], "&&") == 0){
					parsed[i] = "&&";
					i++;
					j++;
				}
				strcpy(and_test,semi[j]);

				parsed[i] = strtok(and_test,"&");
				while(parsed[i] != NULL) {
					i++;
					and_check = strtok(NULL, "&");
					if(and_check != NULL) {
						parsed[i] = "&";
						i++;
						parsed[i] = and_check;
					}
					else {
						parsed[i] = NULL;
					}
				}
				j++;
			}

			i=0;
			j=0;
			while(parsed[i] != NULL){
				// parsing seperated command by blank " "
				char *args[MAX_LEN];
				// distinguish command by &&, ;, &
				if(strcmp(parsed[i], "&&") == 0) {
					i++;
					args[j] = strtok(parsed[i]," ");
					while(args[j] != NULL){
						j++;
						args[j] = strtok(NULL, " ");
					}
					j=0;

					run_command(args, host_dir);
					i++;

				}
				else if(strcmp(parsed[i], ";") == 0) {
					i++;
					args[j] = strtok(parsed[i]," ");
					while(args[j] != NULL){
						j++;
						args[j] = strtok(NULL, " ");
					}
					j=0;

					run_command(args, host_dir);
					i++;

				}
				else if(strcmp(parsed[i], "&") == 0) {
					i++;
					args[j] = strtok(parsed[i]," ");
					while(args[j] != NULL){
						j++;
						args[j] = strtok(NULL, " ");
					}
					j=0;

					run_command(args, host_dir);
					i++;

				}
				else{
					args[j] = strtok(parsed[i]," ");
					while(args[j] != NULL){
						j++;
						args[j] = strtok(NULL, " ");
					}
					j=0;

					run_command(args, host_dir);
					i++;
				}
			}
		}
	}
	free(input);
	return 0;
}



int run_command(char* args[MAX_LEN], char* host_dir){

	int status;
	int h_n = 0;
	char history_buf[BUF_SIZE];

	// when input exit
	if(strcmp(args[0], "exit") == 0){
		exit(1);
	}
	// when input cd
	else if(strcmp(args[0], "cd") == 0){
		if(chdir(args[1])){
			perror("error");
			return 1;
		}
	}
	//when input history
	else if(strcmp(args[0], "history") == 0){
		if(fork() == 0) {
			strcat(host_dir, "/.bash_history");
			if(execl("/bin/cat", "cat", host_dir, 0) == -1){
				return 1;
			}
		}
		else
			wait(&status);
	}	
	// when input ls
	else if(strcmp(args[0], "ls") == 0){
		if(fork() == 0) {
			if(execv("/bin/ls",args) < 0){
				return 1;
			}
		}
		else
			wait(&status);
	}
	else {
		printf("command not found\n");
	}
no_input:;

	return 0;
}


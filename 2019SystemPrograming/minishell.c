#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#define MAX_LEN 100
#define HIS_SIZE 100
#define BUF_SIZE 256

// seperate command function
int run_command(char* args[MAX_LEN], int output_fd, int input_fd, char* hist[HIS_SIZE], int current);
// parsing
char** parsing_command(char* command);
// distinguish by delimiters
void distinguish(char* command[MAX_LEN], char* hist[HIS_SIZE], int current);
// redirect
void redirect(char* f_command, char* b_command, char* delimiter, char* hist[HIS_SIZE], int current);
// pipe
void pipe_command(char* command_arr[MAX_LEN], int pipe_num, char* hist[HIS_SIZE], int current);
void pipe_run(char* args[MAX_LEN], int output_fd, int input_fd, int* f_pipe, int* s_pipe, char* hist[HIS_SIZE], int current);
// make filename
char* filename(char* command);
// parsing by space
char** space_p(char* command);

int main(void) {
	// origin command(= input)
	char *input, *command;
	// set of seperated command by &&,;,&
	char* parsed[MAX_LEN];
	char* string;
	char* string2;
	// current working directory
	char cd_buf[BUF_SIZE];
	// history buffer
	char* hist[HIS_SIZE];
	int current = 0;


	// initialize directory to user directory
	char* username = getenv("USER");
	char* host_dir = (char *)malloc(128);
	sprintf(host_dir, "/home/%s", username);
	chdir(host_dir);

	while (1)
	{
		int i = 0, j = 0, k = 0;
		int check_int = 0;
		char *args[MAX_LEN];
		getcwd(cd_buf, 255);
		printf("my_cd:%s> ", cd_buf);
		fflush(stdout);

		// get input
		input = (char *)malloc(sizeof(char) * MAX_LEN);
		fgets(input, MAX_LEN, stdin);
		command = strtok(input, "\n");
		
		// input history buffer
		if(command == NULL) {
			hist[current] = " ";
		}
		else
			hist[current] = strdup(command);
		
		current = (current + 1) % HIS_SIZE;

		// input == NULL
		if (command == NULL)
		{
			goto no_input;
		}

		// parsing by delimiters
		memcpy(parsed, parsing_command(command), sizeof(parsed));
		distinguish(parsed, hist, current);	
		i++;
	no_input:;
	}

	free(input);
	free(host_dir);
	return 0;
}

char** parsing_command(char* command) {
	char* parsing_A[100];
	char* parsing_B[100];
	char* parsing_C[100];
	char** result;
	char* check;
	char* string;
	char* string2;
	int i = 0;
	int j = 0;

	if (command == NULL)
		return NULL;

	parsing_A[i] = command;
	// parsing by &&
	do
	{
		string2 = parsing_A[i];
		string = strstr(parsing_A[i], "&&");

		if (string != NULL)
		{
			memset(string, '\0', 2);
			parsing_A[i] = string2;
			parsing_A[i + 1] = "&&";
			parsing_A[i + 2] = string + 2;
		}
		else
		{
			parsing_A[i] = string2;
			parsing_A[i + 1] = NULL;
			break;
		}
		i = i + 2;
	} while (parsing_A[i] != NULL);
	// parsing by >>
	i = 0;
	j = 0;
	while (parsing_A[j] != NULL)
	{
		parsing_B[i] = parsing_A[j];
		do
		{
			string2 = parsing_B[i];
			string = strstr(parsing_B[i], ">>");

			if (string != NULL)
			{
				memset(string, '\0', 2);
				parsing_B[i] = string2;
				parsing_B[i + 1] = ">>";
				parsing_B[i + 2] = string + 2;
			}
			else
			{
				parsing_B[i] = string2;
				parsing_B[i + 1] = NULL;
				break;
			}
			i = i + 2;
		} while (parsing_B[i] != NULL);
		i += 1;

		if (parsing_A[j + 1] == NULL)
		{
			break;
		}
		j = j + 1;
	}
	// parsing by >|
	i = 0;
	j = 0;
	while (parsing_B[j] != NULL)
	{
		parsing_C[i] = parsing_B[j];
		do
		{
			string2 = parsing_C[i];
			string = strstr(parsing_C[i], ">|");

			if (string != NULL)
			{
				memset(string, '\0', 2);
				parsing_C[i] = string2;
				parsing_C[i + 1] = ">|";
				parsing_C[i + 2] = string + 2;
			}
			else
			{
				parsing_C[i] = string2;
				parsing_C[i + 1] = NULL;
				break;
			}
			i = i + 2;
		} while (parsing_C[i] != NULL);
		i += 1;

		if (parsing_B[j + 1] == NULL)
		{
			break;
		}
		j = j + 1;
	}
	// memset string array;
	i = 0;
	while (parsing_A[i] != NULL)
	{
		memset(parsing_A, 0, sizeof(char) * 100);
		i++;
	}
	// parsing by <
	i = 0;
	j = 0;
	while (parsing_C[j] != NULL)
	{
		parsing_A[i] = strtok(parsing_C[j], "<");
		while (parsing_A[i] != NULL)
		{
			i++;
			check = strtok(NULL, "<");
			if (check != NULL)
			{
				parsing_A[i] = "<";
				i++;
			}
			parsing_A[i] = check;
		}
		j++;
	}
	// memset string array;
	i = 0;
	while (parsing_B[i] != NULL)
	{
		memset(parsing_B, 0, sizeof(char) * 100);
		i++;
	}
	// parsing by &
	i = 0;
	j = 0;
	while (parsing_A[j] != NULL)
	{
		parsing_B[i] = strtok(parsing_A[j], "&");
		while (parsing_B[i] != NULL)
		{
			i++;
			check = strtok(NULL, "&");
			if (check != NULL)
			{
				parsing_B[i] = "&";
				i++;
			}
			parsing_B[i] = check;
		}
		if (parsing_A[j + 1] == NULL)
		{
			break;
		}
		parsing_B[i] = parsing_A[j + 1];
		i++;
		j = j + 2;
	}
	// memset string array;
	i = 0;
	while (parsing_C[i] != NULL)
	{
		memset(parsing_C, 0, sizeof(char) * 100);
		i++;
	}
	// parsing by ;
	i = 0;
	j = 0;
	while (parsing_B[j] != NULL)
	{
		parsing_C[i] = strtok(parsing_B[j], ";");
		while (parsing_C[i] != NULL)
		{
			i++;
			check = strtok(NULL, ";");
			if (check != NULL)
			{
				parsing_C[i] = ";";
				i++;
			}
			parsing_C[i] = check;
		}
		if (parsing_B[j + 1] == NULL)
		{
			break;
		}
		parsing_C[i] = parsing_B[j + 1];
		i++;
		j = j + 2;
	}
	// memset string array;
	i = 0;
	while (parsing_A[i] != NULL)
	{
		memset(parsing_A, 0, sizeof(char) * 100);
		i++;
	}
	// parsing by |
	i = 0;
	j = 0;
	while (parsing_C[j] != NULL)
	{
		parsing_A[i] = strtok(parsing_C[j], "|");
		while (parsing_A[i] != NULL)
		{
			i++;
			check = strtok(NULL, "|");
			if (check != NULL)
			{
				parsing_A[i] = "|";
				i++;
			}
			parsing_A[i] = check;
		}
		if (parsing_C[j + 1] == NULL)
		{
			break;
		}
		parsing_A[i] = parsing_C[j + 1];
		i++;
		j = j + 2;
	}
	// memset string array;
	i = 0;
	while (parsing_B[i] != NULL)
	{
		memset(parsing_B, 0, sizeof(char) * 100);
		i++;
	}
	// parsing by >
	i = 0;
	j = 0;
	while (parsing_A[j] != NULL)
	{
		parsing_B[i] = strtok(parsing_A[j], ">");
		while (parsing_B[i] != NULL)
		{
			i++;
			check = strtok(NULL, ">");
			if (check != NULL)
			{
				parsing_B[i] = ">";
				i++;
			}
			parsing_B[i] = check;
		}
		if (parsing_A[j + 1] == NULL)
		{
			break;
		}
		parsing_B[i] = parsing_A[j + 1];
		i++;
		j = j + 2;
	}

	result = parsing_B;
	return result;
}

int run_command(char* args[MAX_LEN], int output_fd, int input_fd, char* hist[HIS_SIZE], int current) {
	int status;
	int h_n = 0;

	// when input exit
	if (strcmp(args[0], "exit") == 0)
	{
		exit(1);
	}
	// when input cd
	else if (strcmp(args[0], "cd") == 0)
	{
		if(output_fd != 1) {	
			dup2(output_fd, 1);
			close(output_fd);
		}
		if(input_fd != 0) {	
			dup2(input_fd, 0);
			close(input_fd);
		}

		if (chdir(args[1]))
		{
			perror("error");
			return 1;
		}
	}
	//when input bg
	else if (strcmp(args[0], "bg") == 0)
	{
		if (fork() == 0)
		{
			if(output_fd != 1) {	
				dup2(output_fd, 1);
				close(output_fd);
			}
			if(input_fd != 0) {	
				dup2(input_fd, 0);
				close(input_fd);
			}

			if (system("bg") == -1)
			{
				exit(0);
				return 1;
			}
			exit(0);
		}
		else
			wait(&status);
	}
	//when input jobs
	else if (strcmp(args[0], "jobs") == 0)
	{
		if (fork() == 0)
		{
			if(output_fd != 1) {	
				dup2(output_fd, 1);
				close(output_fd);
			}
			if(input_fd != 0) {	
				dup2(input_fd, 0);
				close(input_fd);
			}

			if (system("jobs") == -1)
			{
				exit(0);
				return 1;
			}
			exit(0);
		}
		else
			wait(&status);
	}
	//when input history
	else if (strcmp(args[0], "history") == 0)
	{
		if (fork() == 0)
		{
			if(output_fd != 1) {	
				dup2(output_fd, 1);
				close(output_fd);
			}
			if(input_fd != 0) {	
				dup2(input_fd, 0);
				close(input_fd);
			}
			int i = 0;
			int hist_num = 1;
			do {
				printf("%d  %s\n", hist_num, hist[i]);
				hist_num++;

				i = (i + 1) % HIS_SIZE;

			} while (i != current);
			exit(0);
		}
		else
			wait(&status);
	}
	else
	{
		if (fork() == 0)
		{
			if(output_fd != 1) {	
				dup2(output_fd, 1);
				close(output_fd);
			}
			if(input_fd != 0) {	
				dup2(input_fd, 0);
				close(input_fd);
			}

			if (execvp(args[0], args) < 0)
			{
				printf("command not found\n");
				exit(1);
			}
		}
		else
			wait(&status);
	}
	return 0;
}

void distinguish(char* command[MAX_LEN], char* hist[HIS_SIZE], int current) {
	char* args[MAX_LEN];
	int i = 0;
	int j = 0;
	int output_fd = 1;
	int input_fd = 0;

	while (command[i] != NULL)
	{
		if (command[i + 1] == NULL)
		{
			memcpy(args, space_p(command[i]), sizeof(args));
			run_command(args, output_fd, input_fd, hist, current);
			break;
		}
		else if (strcmp(command[i + 1], ";") == 0)
		{
			memcpy(args, space_p(command[i]), sizeof(args));
			run_command(args, output_fd, input_fd, hist, current);
			i = i + 2;
		}
		else if (strcmp(command[i + 1], "&") == 0)
		{
			memcpy(args, space_p(command[i]), sizeof(args));
			if (fork() == 0)
			{
				run_command(args, output_fd, input_fd, hist, current);
			}
			i = i + 2;
		}
		else if (strcmp(command[i + 1], "&&") == 0)
		{
			memcpy(args, space_p(command[i]), sizeof(args));
			if (run_command(args, output_fd, input_fd, hist, current) == 1)
			{
				i = i + 4;
			}
			else
			{
				run_command(args, output_fd, input_fd, hist, current);
				i = i + 2;
			}
		}
		else if ((strcmp(command[i + 1], ">") == 0) || (strcmp(command[i + 1], ">>") == 0) || (strcmp(command[i + 1], "<") == 0) || (strcmp(command[i + 1], ">|") == 0))
		{

			redirect(command[i], command[i + 2], command[i + 1], hist, current);
			i = i + 4;
		}
		else if (strcmp(command[i + 1], "|") == 0)
		{
			int j = 0;
			int check_pipen = 1;
			int cmp_i = i + 1;
			char* command_arr[MAX_LEN];
			
			command_arr[j] = command[i];
			j++;
			command_arr[j] = command[i+2];
			j++;
			while(strcmp(command[cmp_i + 2], "|") == 0) {
				command_arr[j] = command[cmp_i + 3];
				j++;		
				cmp_i = cmp_i + 2;
				check_pipen++;
			}
			pipe_command(command_arr, check_pipen, hist, current);
		}
	}
}

void redirect(char* f_command, char* b_command, char* delimiter, char* hist[HIS_SIZE], int current) {
	char* args[MAX_LEN];
	char* final_b_command;
	int redirect_check = 1;
	int input_fd = 0;
	int output_fd = 1;
	int i = 0;
	int j = 0;	

	if (b_command == NULL)
	{
		perror("redirection error");
	}
	else
	{
		if (strcmp(delimiter, ">") == 0)
		{
			memcpy(args, space_p(f_command), sizeof(args));
			final_b_command = filename(b_command);
			redirect_check = open(final_b_command, O_WRONLY | O_CREAT, 0666);
			run_command(args, redirect_check, input_fd, hist, current);
		}
		else if (strcmp(delimiter, ">>") == 0)
		{
			memcpy(args, space_p(f_command), sizeof(args));
			final_b_command = filename(b_command);
			redirect_check = open(final_b_command, O_RDWR | O_CREAT | O_APPEND, 0666);
			run_command(args, redirect_check, input_fd, hist, current);
		}
		else if (strcmp(delimiter, "<") == 0)
		{
			memcpy(args, space_p(f_command), sizeof(args));
			final_b_command = filename(b_command);
			redirect_check = open(final_b_command, O_RDONLY, 0666);
			run_command(args, output_fd, redirect_check, hist, current);
		}
		else if (strcmp(delimiter, ">|") == 0)
		{
			memcpy(args, space_p(f_command), sizeof(args));
			final_b_command = filename(b_command);
			redirect_check = open(final_b_command, O_RDWR | O_CREAT | O_TRUNC, 0666);
			run_command(args, redirect_check, input_fd, hist, current);
		}
	}
}

void pipe_command(char* command_arr[MAX_LEN], int pipe_num, char* hist[HIS_SIZE], int current) {
	int output_fd = 1;
	int input_fd = 0;
	int j = 0;
	char* args[MAX_LEN];
	int pipeM[pipe_num][2];
	pid_t pid[pipe_num + 1];

	for(int i = 0; i < pipe_num; i++) {
		pipe(pipeM[i]);
	}
	for(int i = 0; i < pipe_num + 1; i++) {	
		memcpy(args, space_p(command_arr[i]), sizeof(args));
		if((pid[i] = fork()) == 0) {
			if(i > 0) {
				waitpid(pid[i-1],NULL,0);
			}
			
			pipe_run(args, output_fd, input_fd, pipeM[i], hist, current);
		}
		else {
			waitpid(pid[i],NULL,0);
		}	
	}
	for(int i = 0; i < pipe_num; i++) {
		close(pipeM[i][0]);
		close(pipeM[i][1]);
	}
}

void pipe_run(char* args[MAX_LEN], int output_fd, int input_fd, int* f_pipe, int* s_pipe, char* hist[HIS_SIZE], int current) {
	int status;
	int h_n = 0;

	// when input exit
	if (strcmp(args[0], "exit") == 0)
	{
		exit(0);
	}
	// when input cd
	else if (strcmp(args[0], "cd") == 0)
	{
		if(fork() == 0) {
			if(output_fd != 1) {	
				dup2(output_fd, 1);
				close(output_fd);
			}
			if (chdir(args[1]))
			{
				perror("error");
				exit(0);
			}
			exit(0);
		}
		else
			wait(NULL);
	}
	//when input bg
	else if (strcmp(args[0], "bg") == 0)
	{
		if (fork() == 0)
		{
			if(output_fd != 1) {	
				dup2(output_fd, 1);
				close(output_fd);
			}
			if (system("bg") == -1)
			{
				exit(0);
			}
			exit(0);
		}
		else
			wait(&status);
	}
	//when input jobs
	else if (strcmp(args[0], "jobs") == 0)
	{
		if (fork() == 0)
		{
			if(output_fd != 1) {	
				dup2(output_fd, 1);
				close(output_fd);
			}
			if (system("jobs") == -1)
			{
				exit(0);
			}
			exit(0);
		}
		else
			wait(&status);
	}
	//when input history
	else if (strcmp(args[0], "history") == 0)
	{
		if (fork() == 0)
		{
			if(output_fd != 1) {	
				dup2(output_fd, 1);
				close(output_fd);
			}
			int i = 0;
			int hist_num = 1;
			do {
				printf("%d  %s\n", hist_num, hist[i]);
				hist_num++;
				i = (i + 1) % HIS_SIZE;
			} while (i != current);
			exit(0);
		}
		else
			wait(&status);
	}
	// when input normal command
	else
	{
		if (fork() == 0)
		{
			if(output_fd != 1) {	
				dup2(output_fd, 1);
				close(output_fd);
			}
			if (execvp(args[0], args) < 0)
			{
				printf("command not found\n");
				exit(1);
			}
		}
		else
			wait(&status);
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

char** space_p(char* command) {
	char* args[MAX_LEN];
	char** result;
	int j = 0;

	args[j] = strtok(command, " ");
	while (args[j] != NULL)
	{
		j++;
		args[j] = strtok(NULL, " ");
	}
	result = args;
	return result;
}

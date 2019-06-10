#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#define MAX_LEN 100
#define BUF_SIZE 256

// seperate command function
int run_command(char* args[MAX_LEN], char* host_dir, int redirect_check);
// parsing
char **parsing_command(char* command);
// distinguish by delimiters
void distinguish(char* command[MAX_LEN], char* host_dir);
// redirect
void redirect(char* f_command, char* b_command, char* delimiter, char* host_dir);
// pipe
void pipe_command(char* command, char* host_dir);
// make filename
char* filename(char* command);

int main(void)
{
	// origin command(= input)
	char *input, *command;
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

		// input == NULL
		if (command == NULL)
		{
			goto no_input;
		}

		// parsing by delimiters
		memcpy(parsed, parsing_command(command), sizeof(parsed));
		distinguish(parsed, host_dir);	
		i++;
	no_input:;
	}
	free(input);
	free(host_dir);
	return 0;
}

char** parsing_command(char* command)
{
	char *parsing_A[100];
	char *parsing_B[100];
	char *parsing_C[100];
	char **result;
	char *check;
	char *string;
	char *string2;
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

int run_command(char* args[MAX_LEN], char* host_dir, int redirect_check)
{

	int status;
	int h_n = 0;
	char history_buf[BUF_SIZE];

	// when input exit
	if (strcmp(args[0], "exit") == 0)
	{
		exit(1);
	}
	// when input cd
	else if (strcmp(args[0], "cd") == 0)
	{
		if(redirect_check != 1) {	
			dup2(redirect_check, 1);
			close(redirect_check);
		}

		if (chdir(args[1]))
		{
			perror("error");
			return 1;
		}
	}
	//when input history
	else if (strcmp(args[0], "history") == 0)
	{
		if (fork() == 0)
		{
			strcat(host_dir, "/.bash_history");
			if(redirect_check != 1) {	
				dup2(redirect_check, 1);
				close(redirect_check);
			}

			if (execlp("cat", "cat", host_dir, 0) == -1)
			{
				return 1;
			}
		}
		else
			wait(&status);
	}
	else
	{
		if (fork() == 0)
		{
			if(redirect_check != 1) {	
				dup2(redirect_check, 1);
				close(redirect_check);
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

void distinguish(char* command[MAX_LEN], char* host_dir)
{
	char* args[MAX_LEN];
	int i = 0;
	int j = 0;
	int redirect_check = 1;

	while (command[i] != NULL)
	{
		if (command[i + 1] == NULL)
		{
			args[j] = strtok(command[i], " ");
			while (args[j] != NULL)
			{
				j++;
				args[j] = strtok(NULL, " ");
			}
			j = 0;

			run_command(args, host_dir, redirect_check);
			break;
		}
		else if (strcmp(command[i + 1], ";") == 0)
		{
			args[j] = strtok(command[i], " ");
			while (args[j] != NULL)
			{
				j++;
				args[j] = strtok(NULL, " ");
			}
			j = 0;

			run_command(args, host_dir, redirect_check);
			i = i + 2;
		}
		else if (strcmp(command[i + 1], "&") == 0)
		{
			args[j] = strtok(command[i], " ");
			while (args[j] != NULL)
			{
				j++;
				args[j] = strtok(NULL, " ");
			}
			j = 0;

			if (fork() == 0)
			{
				run_command(args, host_dir, redirect_check);
			}
			i = i + 2;
		}
		else if (strcmp(command[i + 1], "&&") == 0)
		{
			args[j] = strtok(command[i], " ");
			while (args[j] != NULL)
			{
				j++;
				args[j] = strtok(NULL, " ");
			}
			j = 0;

			if (run_command(args, host_dir, redirect_check) == 1)
			{
				i = i + 4;
			}
			else
			{
				run_command(args, host_dir, redirect_check);
				i = i + 2;
			}
		}
		else if ((strcmp(command[i + 1], ">") == 0) || (strcmp(command[i + 1], ">>") == 0) || (strcmp(command[i + 1], "<") == 0) || (strcmp(command[i + 1], ">|") == 0))
		{

			redirect(command[i], command[i + 2], command[i + 1], host_dir);
			i = i + 4;
		}
		else if (strcmp(command[i + 1], "|") == 0)
		{
			pipe_command(command[i], host_dir);
		}
	}
}

void redirect(char* f_command, char* b_command, char* delimiter, char* host_dir)
{
	char* args[MAX_LEN];
	char* final_b_command;
	int redirect_check = 1;
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
			args[j] = strtok(f_command, " ");
			while (args[j] != NULL)
			{
				j++;
				args[j] = strtok(NULL, " ");
			}
			j = 0;
			final_b_command = filename(b_command);
			if(fork() == 0) {		
				redirect_check = open(final_b_command, O_WRONLY | O_CREAT, 0666);
				run_command(args, host_dir, redirect_check);
			
			}
			else
				wait(NULL);
		}
		else if (strcmp(delimiter, ">>") == 0)
		{
			args[j] = strtok(f_command, " ");
			while (args[j] != NULL)
			{
				j++;
				args[j] = strtok(NULL, " ");
			}
			j = 0;

			redirect_check = open(b_command, O_RDWR | O_CREAT | O_APPEND, 0666);
			run_command(args, host_dir, redirect_check);
		}
		else if (strcmp(delimiter, "<") == 0)
		{
			args[j] = strtok(f_command, " ");
			while (args[j] != NULL)
			{
				j++;
				args[j] = strtok(NULL, " ");
			}
			j = 0;
			
			redirect_check = open(b_command, O_RDONLY, 0666);
			run_command(args, host_dir, redirect_check);
		}
		else if (strcmp(delimiter, ">!") == 0)
		{
			int fd;
		}
	}
}

void pipe_command(char* command, char* host_dir) {


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


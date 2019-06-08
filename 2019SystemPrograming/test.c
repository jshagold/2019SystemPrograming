#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>


int main(){
	char command[100] = "";
	char* string;
	char* string2;
	char* result[100];
	char* result_semi[100];
	char* result_final[100];
	int i = 0;
	int j = 0;

	

	result[i] = command;
	do{
		string2 = result[i];

		string = strstr(result[i],"&&");

		if(string != NULL) {
			memset(string, '\0', 2);
			result[i] = string2;
			result[i+1] = "&&";
			result[i+2] = string+2;
		}
		else {
			result[i] = string2;
			result[i+1] = NULL;
			break;
		}
		i=i+2;
	}
	while(result[i] != NULL);

	i = 0;
	while(result[j] != NULL) {
		char* semi_test;
		semi_test = malloc(sizeof(char)*100);
		char* semi_check;
		semi_check = malloc(sizeof(char)*100);
		strcpy(semi_test,result[j]);

		result_semi[i] = strtok(semi_test,";");
		while(result_semi[i] != NULL) {
			i++;
			semi_check = strtok(NULL, ";");
			if(semi_check != NULL){
				result_semi[i] = ";";
				i++;
				result_semi[i] = semi_check;
			}
			else {
				result_semi[i] = NULL;
			}
		}
		j++;
	}

	i=0;
	j=0;
	while(result_semi[j] != NULL) {
		char* and_test;
		and_test = malloc(sizeof(char)*100);
		char* and_check;
		and_check = malloc(sizeof(char)*100);
		if(strcmp(result_semi[j], "&&") == 0){
			result_final[i] = "&&";
			i++;
			j++;
		}
		strcpy(and_test,result_semi[j]);
		
		result_final[i] = strtok(and_test,"&");
		while(result_final[i] != NULL) {
			i++;
			and_check = strtok(NULL, "&");
			if(and_check != NULL) {
				result_final[i] = "&";
				i++;
				result_final[i] = and_check;
			}
			else {
				result_final[i] = NULL;
			}
		}
		j++;
	}

	return 0;
}




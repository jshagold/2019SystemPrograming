#include <stdio.h>
#include <string.h>

int main() {

	char test[100] = "aaa&&bbb&&ccc&dd;e&&f";

	char* string;
	char* string2;
	char* result[100];
	int i = 0;

	result[i] = test;
	do{
		string2 = result[i];

		string = strstr(result[i],"&&");

		if(string != NULL) {
			memset(string, '\0', 2);
			result[i] = string2;
			result[i+1] = string+2;           
		}
		else {
			result[i] = string2;
			result[i+1] = NULL;
		}
		i++;
	}
	while(result[i] != NULL);

	i = 0;
	while(result[i]!=NULL) {
        printf("in byand, result[0] : %s\n", result[i]);
        printf("in byand, result[1] : %s\n", result[i+1]);
	i++;
	}
	
	return 0;
}
	

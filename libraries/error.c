#include"error.h"
#include<stdio.h>

char* filename;
int error_testing = 0;
int errorState = 0;

void error(char* message, ErrorType type,int lineno) {
	if(type==ERROR) {
		printf("%s: %d: error: %s", filename, lineno, message);
		printf("\n");
		errorState = 1;
		return;
	}
	printf("%s: %d: warning: %s", filename, lineno, message);
	printf("\n");
}

void initializeError(char* file, int testing) {
	filename = file;
	error_testing = testing;
}

int checkErrorState() {
	return errorState;
}
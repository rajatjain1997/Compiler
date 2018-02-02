#include"error.h"
#include<stdio.h>

char* filename;

void error(char* message, ErrorType type,int lineno) {
	if(type==ERROR) {
		printf("%s: %d: error: %s", filename, lineno, message);
		printf("\n");
		return;
	}
	printf("%s: %d: warning: %s", filename, lineno, message);
	printf("\n");
}

void initializeError(char* file) {
	filename = file;
}
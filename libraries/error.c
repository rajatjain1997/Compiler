#include"error.h"
#include<stdio.h>

char filename[20] = "abc.r";

void error(char* message, ErrorType type,int lineno) {
	if(type==ERROR) {
		printf("%s: %d: error: %s", filename, lineno, message);
		return;
	}
	printf("%s: %d: warning: %s", filename, lineno, message);
}
/**
 *	AUTHOR: Rajat Jain
 *  ID No. 2015A7PS0549P
 */


#include"error.h"
#include"list.h"
#include<stdio.h>
#include<string.h>
#include<stdlib.h>

char* filename;
int error_testing = 0;
int errorState = 0;
List errorBuffer;

void printErrors() {
	ErrorMessage* errmsg;
	char* message;
	ErrorType type;
	int lineno;
	Element temp = errorBuffer->first;
	while(temp!=NULL) {
		errmsg = temp->data.value.errorMessage;
		message = errmsg->message;
		type = errmsg->errorType;
		lineno = errmsg->lineno;
		if(type==ERROR) {
			printf("%s: %d: error: %s", filename, lineno, message);
			printf("\n");
			errorState = 1;
		} else {
			printf("%s: %d: warning: %s", filename, lineno, message);
			printf("\n");
		}
		temp = temp->next;
	}
}

void error(char* message, ErrorType type, int lineno) {
	ErrorMessage* errmsg = (ErrorMessage*) malloc(sizeof(ErrorMessage));
	errmsg->message = malloc(strlen(message));
	strcpy(errmsg->message, message);
	errmsg->errorType = type;
	errmsg->lineno = lineno;
	Data d;
	d.value.errorMessage = errmsg;
	Element temp = errorBuffer->first;
	int index = 0;
	while(temp!=NULL && temp->data.value.errorMessage->lineno<=lineno) {
		index++;
		temp = temp->next;
	}
	insertAtIndex(errorBuffer, d, index);
	errorState = 1;
}

void initializeError(char* file, int testing) {
	filename = file;
	error_testing = testing;
	errorBuffer = createList();
}

int checkErrorState() {
	return errorState;
}

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

/*
 * error_testing: A variable that maintains whether compiler is in test state or not.
 */
int error_testing = 0;

/*
 * errorState: A variable that maintains whether the compilation process resulted in an error or not.
 */
int errorState = 0;


/*
 * errorBuffer: Buffers all errors to display them in ascending order of line nos. in the end.
 */
List errorBuffer;

/*
 * void printErrors(): Used to print all errors in error buffer.
 */

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

/*
 * void error(char* message, ErrorType type, int lineno): Adds new errors to error buffer.
 */

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

/*
 * void initializeError(char* file, int testing): Initializes error module with filename and testing mode.
 */


void initializeError(char* file, int testing) {
	filename = file;
	error_testing = testing;
	errorBuffer = createList();
}

/*
 * int checkErrorState(): Determines if the program is in an erraneous state.
 */

int checkErrorState() {
	return errorState;
}

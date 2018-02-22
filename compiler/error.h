typedef enum {
	WARNING,
	ERROR
} ErrorType;

typedef struct errorMessage{
	ErrorType errorType;
	char* message;
	int lineno;
} ErrorMessage;

extern int error_testing;

void printErrors();
void error(char* message, ErrorType type, int lineno);
void initializeError(char* file, int testing);
int checkErrorState();

typedef enum {
	WARNING,
	ERROR
} ErrorType;

extern int error_testing;

void error(char* message, ErrorType type, int lineno);
void initializeError(char* file, int testing);
int checkErrorState();

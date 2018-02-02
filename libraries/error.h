typedef enum {
	WARNING,
	ERROR
} ErrorType;

void error(char* message, ErrorType type, int lineno);
void initializeError(char* file);

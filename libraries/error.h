char filename[20];

typedef enum {
	WARNING,
	ERROR
} ErrorType;

void error(char* message, ErrorType type, int lineno);

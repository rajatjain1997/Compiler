struct Token;

typedef enum {
	S_ASSIGNOP,
	S_COMMENT,
	S_FUNID,
	S_ID,
	S_NUM,
	S_RNUM,
	S_STR,
	S_END,
	S_INT,
	S_REAL,
	S_STRING,
	S_MATRIX,
	S_MAIN,
	S_SQO,
	S_SQC,
	S_OP,
	S_CL,
	S_SEMICOLON,
	S_COMMA,
	S_IF,
	S_ELSE,
	S_ENDIF,
	S_READ,
	S_PRINT,
	S_FUNCTION,
	S_PLUS,
	S_MINUS,
	S_MUL,
	S_DIV,
	S_SIZE,
	S_AND,
	S_OR,
	S_NOT,
	S_LT,
	S_LE,
	S_EQ,
	S_GT,
	S_GE,
	S_NE
} SymbolType;

typedef struct symbol {
	SymbolType symbolType;
	struct Token* token;
} Symbol;

Symbol* generateSymbol(SymbolType symbolType);
int attachTokenToSymbol(Symbol* symbol, struct Token* token);
int isTerminal(Symbol* symbol);
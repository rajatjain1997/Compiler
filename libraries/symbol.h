struct Token;

typedef enum {
	ASSIGNOP,
	COMMENT,
	FUNID,
	ID,
	NUM,
	RNUM,
	STR,
	END,
	INT,
	REAL,
	STRING,
	MATRIX,
	MAIN,
	SQO,
	SQC,
	OP,
	CL,
	SEMICOLON,
	COMMA,
	IF,
	ELSE,
	ENDIF,
	READ,
	PRINT,
	FUNCTION,
	PLUS,
	MINUS,
	MUL,
	DIV,
	SIZE,
	AND,
	OR,
	NOT,
	LT,
	LE,
	EQ,
	GT,
	GE,
	NE
} SymbolType;

typedef struct symbol {
	SymbolType symbolType;
	struct Token* token;
} Symbol;

Symbol* generateSymbol(SymbolType symbolType);
void attachTokenToSymbol(Symbol* symbol, Token* token);
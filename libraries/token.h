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
} TokenType;

typedef struct {
	int size;
	char* value;
} String;

typedef union {
	String* string;
	float real;
	int integer;
	char* lexeme;
} Value;

typedef struct Token {
	TokenType type;
	Value value;
	int lineno;
} Token;

struct trie;

struct trie* keywords;

Token* tokenize(TokenType type, char* buf,int lineno);
void initializeTokenizer();
struct trie* getTokenMapping();
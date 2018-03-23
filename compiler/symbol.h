/**
 *	AUTHOR: Rajat Jain
 *  ID No. 2015A7PS0549P
 */

struct Token;
struct head;

typedef int SymbolType;

typedef union {
	struct Token* token;
	struct head* rule;
} SymbolDataItems;

typedef struct symbol {
	SymbolType symbolType;
	int isTerminal;
	SymbolDataItems value;
} Symbol;

Symbol* generateSymbol(SymbolType symbolType, int isTerminal);
int attachTokenToSymbol(Symbol* symbol, struct Token* token);
int isTerminal(Symbol* symbol);
struct Token* getToken(Symbol* symbol);

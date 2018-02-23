struct Token;

typedef int SymbolType;

typedef struct symbol {
	SymbolType symbolType;
	int isTerminal;
	struct Token* token;
} Symbol;

Symbol* generateSymbol(SymbolType symbolType, int isTerminal);
int attachTokenToSymbol(Symbol* symbol, struct Token* token);
int isTerminal(Symbol* symbol);
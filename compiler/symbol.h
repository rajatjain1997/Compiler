/**
 *	AUTHOR: Rajat Jain
 *  ID No. 2015A7PS0549P
 */

struct Token;
struct head;
struct rule;

typedef int SymbolType;

typedef struct symbol {
	SymbolType symbolType;
	int isTerminal;
	struct Token* token;
	struct rule* rule;
} Symbol;

Symbol* generateSymbol(SymbolType symbolType, int isTerminal);
int attachTokenToSymbol(Symbol* symbol, struct Token* token);
int isTerminal(Symbol* symbol);
struct Token* getToken(Symbol* symbol);
int attachRuleToSymbol(Symbol* symbol, struct rule* rule);
struct head* getRule(Symbol* symbol);

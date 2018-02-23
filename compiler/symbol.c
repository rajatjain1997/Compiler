#include <stdlib.h>
#include "symbol.h"
#include "token.h"

Symbol* generateSymbol(SymbolType symbolType, int isTerminal) {
	Symbol* symbol = (Symbol*) malloc(sizeof(Symbol));
	symbol->symbolType = symbolType;
	symbol->isTerminal = isTerminal;
	symbol->token = NULL;
	return symbol;
}

int attachTokenToSymbol(Symbol* symbol, Token* token) {
	if(token->type==symbol->symbolType && symbol->isTerminal) {
		symbol->token = token;
		return 0;
	}
	return -1;
}

int isTerminal(Symbol* symbol) {
	return symbol->isTerminal;
}
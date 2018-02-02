#include <stdlib.h>
#include "symbol.h"
#include "token.h"

Symbol* generateSymbol(SymbolType symbolType) {
	Symbol* symbol = (Symbol*) malloc(sizeof(Symbol));
	symbol->symbolType = symbolType;
	symbol->token = NULL;
	return symbol;
}

int attachTokenToSymbol(Symbol* symbol, Token* token) {
	if(symbol->token->type==symbol->symbolType) {
		symbol->token = token;
		return 0;
	}
	return -1;
}

int isTerminal(Symbol* symbol) {
	return symbol->symbolType <= S_NE;
}
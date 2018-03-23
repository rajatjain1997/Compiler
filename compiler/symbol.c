/**
 *	AUTHOR: Rajat Jain
 *  ID No. 2015A7PS0549P
 */

#include <stdlib.h>
#include "symbol.h"
#include "token.h"

/*
 * Symbol* generateSymbol(SymbolType symbolType, int isTerminal): Uses an integer symboltype and isTerminal to generate
 * a symbol on heap and return its pointer.
 */

Symbol* generateSymbol(SymbolType symbolType, int isTerminal) {
	Symbol* symbol = (Symbol*) malloc(sizeof(Symbol));
	symbol->symbolType = symbolType;
	symbol->isTerminal = isTerminal;
	symbol->value.token = NULL;
	return symbol;
}

/*
 * int attachTokenToSymbol(Symbol* symbol, Token* token): Used to attach tokens to symbols during the parsing process.
 * Returns -1 if symbol and token cannot be attached, else returns 0.
 */

int attachTokenToSymbol(Symbol* symbol, Token* token) {
	if(token->type==symbol->symbolType && symbol->isTerminal) {
		symbol->value.token = token;
		return 0;
	}
	return -1;
}

/*
 * int isTerminal(Symbol* symbol): Checks whether the passed symbol is a non terminal or terminal.
 */

int isTerminal(Symbol* symbol) {
	return symbol->isTerminal;
}

struct Token* getToken(Symbol* symbol) {
	if(isTerminal(symbol)) {
		return symbol->value.token;
	}
	return NULL;
}

#include "symbol.h"

Symbol* generateSymbol(SymbolType symbolType) {
	Symbol* symbol = (Symbol*) malloc(sizeof(Symbol));
	symbol->symbolType = symbolType;
	symbol->token = NULL;
	return symbol;
}

void attachTokenToSymbol(Symbol* symbol, Token* token) {
	symbol->token = token;
}
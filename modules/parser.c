#include "../libraries/list.h"
#include "../libraries/token.h"
#include "../libraries/error.h"
#include "../libraries/tree.h"
#include "../libraries/symbol.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void raiseUnexpectedSymbolException(Symbol* expected, Token* received) {
	char msg[100];
	ErrorType e = ERROR;
	strcpy(msg, "Expected A recieved B");
	error(msg, e, received->lineno);
}

void PDAPush(Stack PDAStack, StackSymbol lastPopped, SymbolType symbolType) {
	Symbol* symbol = generateSymbol(symbolType);
	Tree symbolTree = add_child(lastPopped.symbolTree, symbol);
	StackSymbol toPush;
	toPush.symbol = symbol;
	toPush.symbolTree = symbolTree;
	StackData data;
	data.value.stackSymbol = toPush;
	push(PDAStack, data);
}

StackSymbol PDAPop(Stack PDAStack, Token* currentToken) {
	StackData data;
	data = pop(PDAStack);
	StackSymbol popped = data.value.stackSymbol;
	if(isTerminal(popped.symbol)) {
		if(attachTokenToSymbol(extractSymbol(popped.symbolTree), currentToken)==-1) {
			raiseUnexpectedSymbolException(popped.symbol, currentToken);
		}
	}
	return popped;
}
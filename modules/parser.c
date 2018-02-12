#include "../libraries/list.h"
#include "../libraries/token.h"
#include "../libraries/error.h"
#include "../libraries/tree.h"
#include "../libraries/symbol.h"
#include "../libraries/grammar.h"
#include "../libraries/set.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void raiseUnexpectedSymbolException(Symbol* expected, Token* received) {
	char msg[100];
	ErrorType e = ERROR;
	strcpy(msg, "Expected A recieved B");
	error(msg, e, received->lineno);
}

void raiseUnexpectedTerminationException() {
	char msg[100];
	ErrorType e = ERROR;
	strcpy(msg, "Unexpected token stream termination. You have skipped some symbols.");
	error(msg, e, -1);
}

void PDAPush(Stack PDAStack, StackSymbol lastPopped, Symbol* symbol) {
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

Stack createPDAStack() {
	Stack stack = createStack();
	StackData data;
	Symbol* symbol = generateSymbol(NT_MainFxn);
	Tree symbolTree = createTree(symbol);
	StackSymbol toPush;
	toPush.symbol = symbol;
	toPush.symbolTree = symbolTree;
	data.value.stackSymbol = toPush;
	push(stack, data);
	return stack;
}

Tree parse(Queue tokenStream) {
	Stack stack = createPDAStack();
	Tree parseTree = top(stack).value.stackSymbol.symbolTree;
	Token* currentToken;
	QueueData data;
	StackSymbol stackSymbol;
	// while(tokenStream->size>0) {
	// 	data = dequeue(tokenStream);
	// 	currentToken = data.value;
	// 	switch(currentToken->type) {
	// 		case ASSIGNOP: 
	// 			stackSymbol = PDAPop(stack, currentToken);
	// 			if(!isTerminal(stackSymbol.symbol)) {
	// 				raiseUnexpectedSymbolException(stackSymbol.symbol, currentToken);
	// 			}
	// 			break;
	// 		case FUNID:
	// 			while(1) {
	// 				stackSymbol = PDAPop(stack, currentToken);
	// 				if(isTerminal(stackSymbol.symbol)) {
	// 					break;
	// 				}
	// 				switch(stackSymbol.symbol->symbolType) {
	// 					case NT_Prog: rule2(); break;
	// 					case NT_StmtsAndFxnDefs: rule3(); break;
	// 					case NT_StmtOrFxnDef: rule5(); break;
	// 					case NT_Stmt: rule11(); break;
	// 					case NT_FxnCallStmt: rule60(); break;
	// 					case NT_FxnCall: rule61(); break;
	// 					case NT_RightHandSide: rule25(); break;
	// 					case NT_StmtProg: rule33(); break;
	// 					default: raiseUnexpectedSymbolException(stackSymbol.symbol, currentToken);
	// 				}
	// 			}
	// 			break;
	// 		case ID:
	// 			while(1) {
	// 				stackSymbol = PDAPop(stack, currentToken);
	// 				if(isTerminal(stackSymbol.symbol)) {
	// 					break;
	// 				}
	// 				switch(stackSymbol.symbol->symbolType) {
	// 					case NT_Prog: rule2(); break;
	// 					case NT_StmtsAndFxnDefs: rule3(); break;
	// 					case NT_StmtOrFxnDef: rule5(); break;
	// 					case NT_Stmt: rule8(); break;
	// 					case NT_AssignStmt: rule21(); break;
	// 					case NT_LeftHandSide: rule22(); break;
	// 					case NT_RightHandSide: rule24(); break;
	// 					case NT_StmtProg: rule33(); break;
	// 					case NT_ArithmeticExpr: break; //Need to handle
	// 					case NT_Var: rule41(); break;
	// 					case NT_MatrixElem: rule38(); break;
	// 					case NT_VarList: rule52(); break;
	// 					case NT_ArgList: rule62(); break;
	// 					default: raiseUnexpectedSymbolException(stackSymbol.symbol, currentToken);
	// 				}
	// 			}
	// 			break;
	// 		case NUM:
	// 			while(1) {
	// 				stackSymbol = PDAPop(stack, currentToken);
	// 				if(isTerminal(stackSymbol.symbol)) {
	// 					break;
	// 				}
	// 				switch(stackSymbol.symbol->symbolType) {
	// 					case NT_RightHandSide: rule24(); break;
	// 					case NT_ArithmeticExpr: break; //Need to Handle
	// 					case NT_Var: rule39(); break;
	// 					case NT_IntegerList: rule56(); break;
	// 					case NT_ArgList: rule62(); break;
	// 					default: raiseUnexpectedSymbolException(stackSymbol.symbol, currentToken);
	// 				}
	// 			}
	// 			break;
	// 	}
	// }
	// if(stack->size!=0) {
	// 	raiseUnexpectedTerminationException();
	// }
	return parseTree;
}

Set first(Grammar g, Symbol* symbol) {
	Set set = createSet();
	if(isTerminal(symbol)) {
		putInSet(set, symbol->symbolType);
		return set;
	}
	Set empty = createSet();
	putInSet(empty, 63);
	Element rule = g->NonTerminals[symbol->symbolType].rules->first;
	Element s; Set ruleset, temp;
	while(rule!=NULL) {
		s = rule->first;
		do {
			ruleset = first(g, s->data.value.symbol);
			temp = set;
			set = union(set, difference(ruleset, empty));
			free(temp);
		} while(getFromSet(ruleset, 63) && (s=s->next)!=NULL);
		if(getFromSet(ruleset, 63) && s==NULL) {
			temp = set;
			set = union(set, 63);
			free(temp);
		}
		rule = rule->next;
	}
	g->NonTerminals[symbol->symbolType].first = set;
	free(empty);
	return set;
}

Set follow(Grammar g, SymbolType symboltype) {
	if(g->NonTerminals[i].follow!=NULL) {
		return g->NonTerminals[i].follow;
	}
	Set set = createSet();
	Set empty = createSet();
	putInSet(empty, 63);
	Element occurance = g->NonTerminals[i].occrances->first; 
	Element s; Set temp; int owner;
	while(occurance!=NULL) {
		s = occurance->data.value.occurance->node->next;
		owner = occurance->data.value.occurance->owner;
		do {
			ruleset = g->NonTerminals[s->data.value.symbol->symbolType].first;
			temp = set;
			union(set, difference(ruleset, empty));
			free(temp);
		} while(getFromSet(ruleset, 63) && (s=s->next)!=NULL);
		if(getFromSet(ruleset, 63) && s==NULL) {
			temp = set;
			set = union(set, follow(g, owner));
			free(temp);
		}
		occurance = occurance->next;
	}
	g->NonTerminals[i].follow = set;
	free(empty);
	return set;
}

void createFirstSets(Grammar g) {
	int i = 0; Symbol temp;
	for(;i<g->size;i++) {
		if(g->NonTerminals[i].first==NULL) {
			temp = generateSymbol(g->NonTerminals[i].symbolType, 0);
			first(g, temp);
			free(temp);
		}
	} 
}

void createFollowSets(Grammar g) {
	int i = 0;
	for(;i<g->size;i++) {
		if(getFromSet(g->NonTerminals[i].first, 63) && g->NonTerminals[i].follow==NULL) {
			follow(g, i);
		}
	}
}

void initializeParser(char* grammarfile) {
	Grammar g = readGrammar(grammarfile);
	createFirstSets(g);
	createFollowSets(g);
}
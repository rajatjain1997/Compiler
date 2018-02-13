#include "../libraries/list.h"
#include "../libraries/token.h"
#include "../libraries/error.h"
#include "../libraries/tree.h"
#include "../libraries/symbol.h"
#include "../libraries/grammar.h"
#include "../libraries/set.h"
#include "parser.h"
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

void PDAPush(Stack PDAStack, StackSymbol lastPopped, List rule) {
	Symbol* symbol;
	Element itr = rule->last;
	while(itr!=NULL) {
		symbol = generateSymbol(itr->data.value.symbol->symbolType, itr->data.value.symbol->isTerminal);
		Tree symbolTree = add_child(lastPopped.symbolTree, symbol);
		StackSymbol toPush;
		toPush.symbol = symbol;
		toPush.symbolTree = symbolTree;
		StackData data;
		data.value.stackSymbol = toPush;
		push(PDAStack, data);
		itr = itr->prev;
	}
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
	Symbol* symbol = generateSymbol(0, 0);
	Tree symbolTree = createTree(symbol);
	StackSymbol toPush;
	toPush.symbol = symbol;
	toPush.symbolTree = symbolTree;
	data.value.stackSymbol = toPush;
	push(stack, data);
	return stack;
}

Set first(Grammar g, Symbol* symbol) {
	Set set = createSet();
	if(isTerminal(symbol)) {
		putInSet(set, symbol->symbolType);
		return set;
	}
	Set empty = createSet();
	putInSet(empty, EPSILON);
	Element rule = g->NonTerminals[symbol->symbolType].rules->first;
	Element s; Set ruleset, temp;
	while(rule!=NULL) {
		if(rule->data.value.list->size==0) {
			set = setUnion(set, empty);
			rule = rule->next;
			continue;
		}
		s = rule->data.value.list->first;
		do {
			ruleset = first(g, s->data.value.symbol);
			temp = set;
			set = setUnion(set, difference(ruleset, empty));
			//free(temp);
		} while(getFromSet(ruleset, EPSILON) && (s=s->next)!=NULL);
		if(getFromSet(ruleset, EPSILON) && s==NULL) {
			temp = set;
			set = setUnion(set, empty);
			//free(temp);
		}
		rule = rule->next;
	}
	g->NonTerminals[symbol->symbolType].first = set;
	//free(empty);
	return set;
}

Set follow(Grammar g, SymbolType symbolType) {
	if(g->NonTerminals[symbolType].follow!=NULL) {
		return g->NonTerminals[symbolType].follow;
	}
	Set set = createSet();
	Set empty = createSet();
	putInSet(empty, EPSILON);
	Element occurance = g->NonTerminals[symbolType].occurances->first; 
	Element s; Set temp, ruleset; int owner;
	while(occurance!=NULL) {
		s = occurance->data.value.occurance->node->next;
		owner = occurance->data.value.occurance->owner;
		do {
			ruleset = g->NonTerminals[s->data.value.symbol->symbolType].first;
			temp = set;
			set = setUnion(set, difference(ruleset, empty));
			//free(temp);
		} while(getFromSet(ruleset, EPSILON) && (s=s->next)!=NULL);
		if(getFromSet(ruleset, EPSILON) && s==NULL) {
			temp = set;
			set = setUnion(set, follow(g, owner));
			//free(temp);
		}
		occurance = occurance->next;
	}
	g->NonTerminals[symbolType].follow = set;
	//free(empty);
	return set;
}

void createFirstSets(Grammar g) {
	int i = 0; Symbol* temp;
	for(;i<g->size;i++) {
		if(g->NonTerminals[i].first==NULL) {
			temp = generateSymbol(g->NonTerminals[i].symbolType, 0);
			first(g, temp);
			//free(temp);
		}
	} 
}

void createFollowSets(Grammar g) {
	int i = 0;
	for(;i<g->size;i++) {
		if(getFromSet(g->NonTerminals[i].first, EPSILON) && g->NonTerminals[i].follow==NULL) {
			follow(g, i);
		}
	}
}

List** createParseTable(Grammar g) {
	List **parsetable;
	parsetable = (List**) malloc(sizeof(List*)*g->size);
	int i = 0;
	for(;i<g->size;i++) {
		parsetable[i] = (List*) malloc(sizeof(List)*(NE+2));
	}
	TokenType terminal; int nonTerminal; Element temp, temp2; List rule; int firstSymbol;
	for(nonTerminal = 0; nonTerminal < g->size; nonTerminal++) {
		for(terminal = 0; terminal < NE+1; terminal++) {
			parsetable[nonTerminal][terminal] = NULL;

			//Checking if the non-terminal has a valid rule
			if(!getFromSet(g->NonTerminals[nonTerminal].first, terminal) && 
				(
					g->NonTerminals[nonTerminal].follow==NULL ||
					(g->NonTerminals[nonTerminal].follow!=NULL && !getFromSet(g->NonTerminals[nonTerminal].follow, terminal))
				)
			) {
				continue;
			}

			temp = g->NonTerminals[nonTerminal].rules->first;

			//Iterating through all rules of the non-terminal in order to find the correct rule

			while(temp!=NULL && parsetable[nonTerminal][terminal]==NULL) {
				rule = temp->data.value.list;

				//For A->EPSILON. In such a case the rule needs to be applied if terminal is present in the follow set of A.
				if(rule->size==0) {
					if(getFromSet(g->NonTerminals[nonTerminal].follow, terminal)) {
						parsetable[nonTerminal][terminal] = rule;
					}
					break;
				}

				temp2 = rule->first;

				//Iterating though each symbol in the rule to calculate a cumulative first set for the rule. Rule is nullable if temp2 reaches NULL.
				do {
					firstSymbol = temp2->data.value.symbol->symbolType;
					if(isTerminal(temp2->data.value.symbol)) {
						if(firstSymbol==terminal) {
							parsetable[nonTerminal][terminal] = rule;
						}
						break;
					} else if(getFromSet(g->NonTerminals[firstSymbol].first, terminal)) {
						parsetable[nonTerminal][terminal] = rule;
						break;
					} else if (getFromSet(g->NonTerminals[firstSymbol].first, EPSILON)) {
						temp2 = temp2->next;
					} else {
						break;
					}
				} while (temp2!=NULL);

				//Checking if the rule is nullable. Such a case has the same behavior as A->EPSILON. If a terminal could not be generated by the rule,
				//nullify it.

				if(temp2==NULL && getFromSet(g->NonTerminals[firstSymbol].first, EPSILON)) {
					if(getFromSet(g->NonTerminals[nonTerminal].follow, terminal)) {
						parsetable[nonTerminal][terminal] = rule;
						break;
					}
				}
				temp = temp->next;
			}
		}
	}
	return parsetable;
}

List** initializeParser(char* grammarfile) {
	Grammar g = readGrammar(grammarfile);
	createFirstSets(g);
	createFollowSets(g);
	return createParseTable(g);
}

Tree parse(Queue tokenStream, char* grammarfile) {
	List** parsetable = initializeParser(grammarfile);
	Stack stack = createPDAStack();
	Tree parseTree = top(stack).value.stackSymbol.symbolTree;
	Token* currentToken;
	QueueData data;
	StackSymbol stackSymbol;
	while(tokenStream->size>0 && stack->size>0) {
		data = dequeue(tokenStream);
		currentToken = data.value;
		stackSymbol = PDAPop(stack, currentToken);
		if(isTerminal(stackSymbol.symbol)) {
			continue;
		}
		if(parsetable[stackSymbol.symbol->symbolType][currentToken->type]!=NULL) {
			PDAPush(stack, stackSymbol, parsetable[stackSymbol.symbol->symbolType][currentToken->type]);
		} else {
			raiseUnexpectedSymbolException(stackSymbol.symbol, currentToken);
		}
	}
	if(stack->size!=0) {
		raiseUnexpectedTerminationException();
	}
	return parseTree;
}

// void main() {
// 	List** parsetable = initializeParser("Test Grammar");
// 	int i, j;
// 	for(i = 0; i<3; i++) {
// 		for(j = 0; j<NE; j++) {
// 			if(parsetable[i][j]!=NULL) {
// 				printf("%d %d\n", i, j);
// 			}
// 		}
// 	}
// }
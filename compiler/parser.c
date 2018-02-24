/**
 *	AUTHOR: Rajat Jain
 *  ID No. 2015A7PS0549P
 */

#include "list.h"
#include "token.h"
#include "error.h"
#include "tree.h"
#include "symbol.h"
#include "grammar.h"
#include "set.h"
#include "parser.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int panic(Queue tokenStream, Stack stack, List** parsetable,Token* currentToken, StackSymbol currentSymbol) {
	Token* temp = currentToken; StackData d;
	d.value.stackSymbol = currentSymbol;
	while(tokenStream->size>0) {
		if(isTerminal(currentSymbol.symbol)) {
			return 0;
		} else if(parsetable[currentSymbol.symbol->symbolType][temp->type]!=NULL) {
			push(stack, d);
			return 0;
		} else if(stack->size!=0 && isTerminal(top(stack).value.stackSymbol.symbol)) {
			if(top(stack).value.stackSymbol.symbol->symbolType==temp->type) {
				return 0;
			} else {
				temp = dequeue(tokenStream).value;
				*currentToken = *temp;
			}
		} else if(stack->size!=0 && !isTerminal(top(stack).value.stackSymbol.symbol) &&
			parsetable[top(stack).value.stackSymbol.symbol->symbolType][temp->type]!=NULL
		) {
			return 0;
		} else if(temp->type==SEMICOLON) {
			temp = dequeue(tokenStream).value;
			*currentToken = *temp;
			return 0;
		} else {
			temp = dequeue(tokenStream).value;
			*currentToken = *temp;
		}
	}
	return -1;
}

int raiseUnexpectedSymbolException(Queue tokenStream, Stack stack, List** parsetable, StackSymbol expected, Token* received) {
	char msg[256]; int i = 0; int j = 0; char buf[20];
	ErrorType e = ERROR;
	strcpy(msg, "SYNTAX ERROR: Unexpected token - \"");
	getLexeme(received, buf);
	strcat(msg, buf);
	strcat(msg, "\". The expected token is: ");
	if(isTerminal(expected.symbol)) {
		sprintf(msg, "%s %s", msg, tokenStrings[i]);
	} else {
		for(;i<NE+1 && j<5;i++) {
			if(parsetable[expected.symbol->symbolType][i]!=NULL) {
				sprintf(msg, "%s %s, ", msg, tokenStrings[i]);
				j++;
			}
		}
		sprintf(msg, "%s %s", msg, "etc.");
	}
	error(msg, e, received->lineno);
	return panic(tokenStream, stack, parsetable, received, expected);
}

void raiseUnexpectedTerminationException() {
	char msg[256];
	ErrorType e = ERROR;
	strcpy(msg, "SYNTAX ERROR: Unexpected token stream termination. You have skipped some symbols.");
	error(msg, e, -1);
}

void raiseLongerStreamException() {
	char msg[256];
	ErrorType e = ERROR;
	strcpy(msg, "SYNTAX ERROR: Program is longer than expected.");
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

StackSymbol PDAPop(Queue tokenStream, Stack PDAStack, List** parsetable, Token* currentToken) {
	StackData data;
	data = pop(PDAStack);
	StackSymbol popped = data.value.stackSymbol;
	while(isTerminal(popped.symbol)) {
		if(attachTokenToSymbol(extractSymbol(popped.symbolTree), currentToken)==-1) {
			if(raiseUnexpectedSymbolException(tokenStream, PDAStack, parsetable, popped, currentToken)==-1 && PDAStack->size>0) {
				popped = pop(PDAStack).value.stackSymbol;
				continue;
			}
		}
		break;
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
	Set set;
	if(isTerminal(symbol)) {
		set = createSet();
		putInSet(set, symbol->symbolType);
		return set;
	}
	if(g->NonTerminals[symbol->symbolType].first!=NULL) {
		return g->NonTerminals[symbol->symbolType].first;
	}
	set = createSet();
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
			//freeSet(temp);
		} while(getFromSet(ruleset, EPSILON) && (s=s->next)!=NULL);
		if(getFromSet(ruleset, EPSILON) && s==NULL) {
			temp = set;
			set = setUnion(set, empty);
			//freeSet(temp);
		}
		rule = rule->next;
	}
	g->NonTerminals[symbol->symbolType].first = set;
	//freeSet(empty);
	return set;
}

Set follow(Grammar g, SymbolType symbolType, int recursive) {
	if(g->NonTerminals[symbolType].follow!=NULL && recursive) {
		return g->NonTerminals[symbolType].follow;
	} else if(g->NonTerminals[symbolType].follow==NULL) {
		g->NonTerminals[symbolType].follow = createSet();
	}
	Set empty = createSet();
	putInSet(empty, EPSILON);
	Element occurance = g->NonTerminals[symbolType].occurances->first;
	Element s; Set temp, ruleset; int owner, freeRuleSet = 0;
	while(occurance!=NULL) {
		ruleset = empty;
		s = occurance->data.value.occurance->node->next;
		owner = occurance->data.value.occurance->owner;
		while(getFromSet(ruleset, EPSILON) && s!=NULL) {
			if(isTerminal(s->data.value.symbol)) {
				ruleset = createSet();
				putInSet(ruleset, s->data.value.symbol->symbolType);
				freeRuleSet = 1;
			} else {
				ruleset = g->NonTerminals[s->data.value.symbol->symbolType].first;
			}
			temp = g->NonTerminals[symbolType].follow;
			g->NonTerminals[symbolType].follow = setUnion(g->NonTerminals[symbolType].follow, difference(ruleset, empty));
			s=s->next;
			//freeSet(temp);
			if(freeRuleSet) {
				//freeSet(ruleset);
				freeRuleSet = 0;
			}
		}
		if(getFromSet(ruleset, EPSILON) && s==NULL) {
			temp = g->NonTerminals[symbolType].follow;
			g->NonTerminals[symbolType].follow = setUnion(g->NonTerminals[symbolType].follow, follow(g, owner, 1));
			//freeSet(temp);
		}
		occurance = occurance->next;
	}
	//freeSet(empty);
	return g->NonTerminals[symbolType].follow;
}

void createFirstSets(Grammar g) {
	int i = 0; Symbol* temp;
	for(;i<g->size;i++) {
		if(g->NonTerminals[i].first==NULL) {
			temp = generateSymbol(g->NonTerminals[i].symbolType, 0);
			first(g, temp);
			// free(temp);
		}
	}
}

void createFollowSets(Grammar g) {
	int i = 0;
	for(;i<g->size;i++) {
		if(getFromSet(g->NonTerminals[i].first, EPSILON)) {
			follow(g, i, 0);
		}
	}
	for(i=0;i<g->size;i++) {
		if(!getFromSet(g->NonTerminals[i].first, EPSILON)) {
			//freeSet(g->NonTerminals[i].follow);
			g->NonTerminals[i].follow=NULL;
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
		for(terminal = 0; terminal < NE; terminal++) {
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

List** initializeParser(Grammar g) {
	createFirstSets(g);
	createFollowSets(g);
	return createParseTable(g);
}

void collectGarbage(Grammar g, List** parsetable, Stack stack, Queue tokenStream) {
	// free(parsetable);
	// freeStack(stack);
	// freeQueue(tokenStream);
	// freeGrammar(g);
}

Tree parse(Queue tokenStream, char* grammarfile) {
	Grammar g = readGrammar(grammarfile);
	List** parsetable = initializeParser(g);
	Stack stack = createPDAStack();
	Tree parseTree = top(stack).value.stackSymbol.symbolTree;
	Token* currentToken;
	QueueData data;
	StackSymbol stackSymbol;
	data = dequeue(tokenStream);
	currentToken = data.value;
	while(stack->size>0) {
		stackSymbol = PDAPop(tokenStream, stack, parsetable, currentToken);
		if(isTerminal(stackSymbol.symbol)) {
			if(tokenStream->size==0 || stack->size==0) {
				break;
			}
			data = dequeue(tokenStream);
			currentToken = data.value;
			continue;
		}
		if(parsetable[stackSymbol.symbol->symbolType][currentToken->type]!=NULL) {
			PDAPush(stack, stackSymbol, parsetable[stackSymbol.symbol->symbolType][currentToken->type]);
		} else {
			raiseUnexpectedSymbolException(tokenStream, stack, parsetable, stackSymbol, currentToken);
		}
	}
	if(stack->size!=0) {
		raiseUnexpectedTerminationException();
	}
	if(tokenStream->size!=0) {
		raiseLongerStreamException();
	}
	collectGarbage(g, parsetable, stack, tokenStream);
	return parseTree;
}

//Functions for GDB

void printStack(Stack s) {
	StackElement temp = s->top;
	TokenType t;
	while(temp!=NULL) {
		if(temp->data.value.stackSymbol.symbol->isTerminal) {
			t = temp->data.value.stackSymbol.symbol->symbolType;
			printf("%d -> ", t);
		} else {
			printf("%d -> ", temp->data.value.stackSymbol.symbol->symbolType);
		}
		temp = temp->next;
	}
	printf("\n");
}

// void main() {
// 	List** parsetable = initializeParser("grammar.txt");
// 	int i, j;
// 	for(i = 0; i<39; i++) {
// 		for(j = 0; j<NE; j++) {
// 			if(parsetable[i][j]!=NULL) {
// 				printf("%d %d\n", i, j);
// 			}
// 		}
// 	}
// }

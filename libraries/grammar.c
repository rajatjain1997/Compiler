#include"grammar.h"
#include"token.h"
#include"symbol.h"
#include"error.h"
#include"trie.h"
#include"list.h"
#include<stdlib.h>
#include<string.h>
#include<stdio.h>

void raiseGrammarNotPresentExeption() {
	char msg[100];
	ErrorType e = ERROR;
	strcpy(msg, "Grammar file could not be opened. It might be corrupted.");;
	error(msg, e, -1);
}

Grammar createGrammar() {
	Grammar g = malloc(sizeof(struct grammar));
	g->size = 0;
	return g;
}

int addNonTerminal(Grammar g, char* symbol, Trie nonterminalmapping) {
	insertInTrie(nonterminalmapping, symbol, g->size);
	g->NonTerminals[g->size].symbolType = g->size;
	g->NonTerminals[g->size].rules = createList();	
	g->NonTerminals[g->size].occurances = createList();
	g->size++;
	return g->size-1;
}

List createRule(Grammar g, int nonterminalindex) {
	Data d;
	List rule = createList();
	d.value.list = rule;
	insertInFront(g->NonTerminals[nonterminalindex].rules, d);
	return rule;
}

int addRule(Grammar g, FILE* fp, Trie terminalmapping, Trie nonterminalmapping) {
	char buffer[256], *symbol;
	int grammarindex, mapping;
	Data d;
	if(!fgets(buffer, 256, fp)) {
		return 0;
	}
	symbol = strtok(buffer, " \n");
	grammarindex = findInTrie(nonterminalmapping, symbol);
	if(grammarindex == -1) {
		grammarindex = addNonTerminal(g, symbol, nonterminalmapping);
	}
	List rule = createRule(g, grammarindex);
	while(symbol = strtok(NULL, " \n")) {
		if(strcmp(symbol, "$")==0) {
			break;
		}
		mapping = findInTrie(terminalmapping, symbol);
		if(mapping != -1) {
			d.value.symbol = generateSymbol(mapping, 1);
			insertInFront(rule, d);
		} else {
			mapping = findInTrie(nonterminalmapping, symbol);
			if(mapping != -1) { 
				d.value.symbol = generateSymbol(mapping, 0);
				insertInFront(rule, d);
				d.value.node = rule->first;
				insertInFront(g->NonTerminals[mapping].occurances, d);
			} else {
				mapping = addNonTerminal(g, symbol, nonterminalmapping);
				d.value.symbol = generateSymbol(mapping, 0);
				insertInFront(rule, d);
				d.value.node = rule->first;
				insertInFront(g->NonTerminals[mapping].occurances, d);
			}
		}
	};
	return 1;
}

Grammar readGrammar(char* filename) {
	FILE* fp = fopen(filename, "r");
	if(fp==NULL) {
		raiseGrammarNotPresentExeption();
		return NULL;
	}
	Trie terminalmapping = getTokenMapping();
	Trie nonterminalmapping = makeTrie();
	Grammar g = createGrammar();
	while(addRule(g, fp, terminalmapping, nonterminalmapping));
	return g;
}

// void main() {
// 	Grammar g = readGrammar("abc");
// 	Node* temp = g->NonTerminals[0].rules->first;
// 	Node* temp2;
// 	while(temp!=NULL) {
// 		temp2 = temp->data.value.list->first;
// 		while(temp2!=NULL) {
// 			printf("%d\t",temp2->data.value.symbol->symbolType);
// 			temp2=temp2->next;
// 		}
// 		printf("Rule Ends\n");
// 		temp=temp->next;
// 	}
// 	printf("\n");
// 	printf("%d", g->size);
// }
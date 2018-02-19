#include"grammar.h"
#include"token.h"
#include"symbol.h"
#include"error.h"
#include"trie.h"
#include"list.h"
#include<stdlib.h>
#include<string.h>
#include<stdio.h>

char nonTerminalStrings[50][30];

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
	if(error_testing) {
		printf("Found %s as %d\n", symbol, g->size);
	}
	strcpy(nonTerminalStrings[g->size],symbol);
	insertInTrie(nonterminalmapping, symbol, g->size);
	g->NonTerminals[g->size].symbolType = g->size;
	g->NonTerminals[g->size].rules = createList();	
	g->NonTerminals[g->size].occurances = createList();
	g->NonTerminals[g->size].follow = NULL;
	g->NonTerminals[g->size].first = NULL;
	g->size++;
	return g->size-1;
}

Occurance createOccurance(Element node, int owner) {
	Occurance occ = malloc(sizeof(struct occurance));
	occ->node = node;
	occ->owner = owner;
	return occ;
}

List createRule(Grammar g, int nonterminalindex) {
	Data d;
	List rule = createList();
	d.value.list = rule;
	insertAtEnd(g->NonTerminals[nonterminalindex].rules, d);
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
			insertAtEnd(rule, d);
		} else {
			mapping = findInTrie(nonterminalmapping, symbol);
			if(mapping != -1) { 
				d.value.symbol = generateSymbol(mapping, 0);
				insertAtEnd(rule, d);
				d.value.occurance = createOccurance(rule->last, grammarindex);
				insertAtEnd(g->NonTerminals[mapping].occurances, d);
			} else {
				mapping = addNonTerminal(g, symbol, nonterminalmapping);
				d.value.symbol = generateSymbol(mapping, 0);
				insertAtEnd(rule, d);
				d.value.occurance = createOccurance(rule->last, grammarindex);
				insertAtEnd(g->NonTerminals[mapping].occurances, d);
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
	Trie nonterminalmapping = makeTrie(TRIE_CASE_INSENSITIVE);
	Grammar g = createGrammar();
	while(addRule(g, fp, terminalmapping, nonterminalmapping));
	freeTrie(terminalmapping);
	freeTrie(nonterminalmapping);
	return g;
}

void freeGrammar(Grammar g) {
	int i = 0; Element temp; NonTerminal nt;
	for(;i<g->size;i++) {
		nt = g->NonTerminals[i];
		freeSet(nt.first);
		if(nt.follow!=NULL) {
			freeSet(nt.follow);
		}
		temp = nt.rules->first;
		while(temp!=NULL) {
			freeList(temp->data.value.list);
			temp = temp->next;
		}
		freeList(nt.rules);
		freeList(nt.occurances);
	}
}

// void main() {
// 	Grammar g = readGrammar("Test Grammar");
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

// void main() {
// 	Grammar g = readGrammar("Test Grammar");
// 	Node* temp = g->NonTerminals[0].occurances->first;
// 	while(temp!=NULL) {
// 		printf("%d\t",temp->data.value.occurance->owner);
// 		temp=temp->next;
// 	}
// 	printf("\n");
// 	printf("%d", g->size);
// }
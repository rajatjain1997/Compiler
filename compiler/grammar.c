/**
 *	AUTHOR: Rajat Jain
 *  ID No. 2015A7PS0549P
 */

#include"grammar.h"
#include"token.h"
#include"symbol.h"
#include"error.h"
#include"trie.h"
#include"list.h"
#include"set.h"
#include<stdlib.h>
#include<string.h>
#include<stdio.h>

char nonTerminalStrings[50][30];
struct trie* nonTerminalMapping;
/*
 * void raiseGrammarNotPresentExeption(): Raises an error if the grammar file cannot be opened.
 */

void raiseGrammarNotPresentExeption() {
	char msg[100];
	ErrorType e = ERROR;
	strcpy(msg, "Grammar file could not be opened. It might be corrupted.");;
	error(msg, e, -1);
}

/*
 * Grammar createGrammar(): Allocates memory for the Grammar type.
 */

Grammar createGrammar() {
	Grammar g = malloc(sizeof(struct grammar));
	g->size = 0;
	return g;
}

/*
 * int addNonTerminal(): Adds a new non terminal to the grammar and inserts it into a temporary Trie.
 * Returns: The index of the nonTerminal.
 */

int addNonTerminal(Grammar g, char* symbol, Trie nonterminalmapping) {
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

/*
 * Occurance createOccurance(Element node, int owner): Appends a new occurance to the occurance list of
 * the owner non-terminal. All insertion operations are O(1) as the list is doubly linked.
 */

Occurance createOccurance(Element node, int owner) {
	Occurance occ = malloc(sizeof(struct occurance));
	occ->node = node;
	occ->owner = owner;
	return occ;
}

/*
 * List createRule(Grammar g, int nonterminalindex): Creates a new rule instance in the provided nonterminal index and returns a
 * a reference to the rulelist..
 */

List createRule(Grammar g, int nonterminalindex, int ruleindex) {
	Data d;
	Rule* rule = (Rule*) malloc(sizeof(Rule));
	rule->rule = createList();
	rule->index = ruleindex;
	d.value.rule = rule;
	insertAtEnd(g->NonTerminals[nonterminalindex].rules, d);
	return rule->rule;
}

/*
 * int addRule(Grammar g, FILE* fp. Trie terminalmapping, Trie nonterminalmapping): Reads exactly one line from the file and adds
 * rules to the grammar appropriately by adding any new nonterminals as needed. Here $ is considered as epsilon. And rules of type
 * A -> $ are represented as empty lists.
 */

int addRule(Grammar g, FILE* fp, Trie terminalmapping, Trie nonterminalmapping, int rulenos) {
	char buffer[256], *symbol;
	int grammarindex, mapping;
	Data d;
	if(!fgets(buffer, 256, fp)) {
		return 0;
	}
	symbol = strtok(buffer, " \n\r");
	grammarindex = findInTrie(nonterminalmapping, symbol);
	if(grammarindex == -1) {
		grammarindex = addNonTerminal(g, symbol, nonterminalmapping);
	}
	List rule = createRule(g, grammarindex, rulenos);
	while(symbol = strtok(NULL, " \n\r")) {
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

/**
 * Grammar readGrammar(char* filename): Opens the grammar file and constructs and returns the grammar structure.
 * Grammar is an array of nonterminals which in turn contain their rules and occurances.
 */

Grammar readGrammar(char* filename) {
	FILE* fp = fopen(filename, "r");
	if(fp==NULL) {
		raiseGrammarNotPresentExeption();
		return NULL;
	}
	Trie terminalmapping = getTokenMapping();
	Trie nonterminalmapping = makeTrie(TRIE_CASE_INSENSITIVE);
	Grammar g = createGrammar();
	int rulenos = 0;
	while(addRule(g, fp, terminalmapping, nonterminalmapping, rulenos) && ++rulenos);
	// freeTrie(terminalmapping);
	nonTerminalMapping = nonterminalmapping;
	return g;
}

/**
 * void freeGrammar(Grammar g): Used to free the grammar once compilation is over.
 */

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

struct head* extractRule(Rule* rule) {
	return rule->rule;
}
int getRuleIndex(Rule* rule) {
	return rule->index;
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

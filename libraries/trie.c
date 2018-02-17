#include "trie.h"
#include <stdio.h>
#include <stdlib.h>

const int TRIE_CASE_SENSITIVE = 0;
const int TRIE_CASE_INSENSITIVE = 1;

Trie makeTrie(int insensitive) {
	Trie trie = (Trie) malloc(sizeof(struct trie));
	int i = 0;
	for(;i<27;i++) {
		trie->level[i] = NULL;
	}
	trie->insensitive = insensitive;
	return trie;
}

int getIndex(char ch, int insensitive) {
	if(ch>='a'&&ch<='z') {
		return ch-97;
	} else if (ch>='A'&&ch<='Z'&&insensitive) {
		return ch-'A';
	} else {
		return 26;
	}
}

void insertInTrie(Trie trie, char* str, int info) {
	int i = 0, index;
	for(;str[i] != '\0' && trie->level[getIndex(str[i], trie->insensitive)]!=NULL;i++) {
		trie = trie->level[getIndex(str[i], trie->insensitive)];
	}
	for(;str[i] != '\0'; i++) {
		index = getIndex(str[i], trie->insensitive);
		trie->level[index] = makeTrie(trie->insensitive);
		trie = trie->level[index];
	}
	int* store = (int*) malloc(sizeof(int));
	*store = info;
	trie->level[getIndex('$', trie->insensitive)] = (Trie) store;
}

int findInTrie(Trie trie, char* str) {
	int i = 0;
	for(;str[i]!='\0';i++) {
		if(trie->level[getIndex(str[i], trie->insensitive)]!=NULL) {
			trie = trie->level[getIndex(str[i], trie->insensitive)];
		} else {
			return -1;
		}
	}
	if(trie->level[getIndex('$', trie->insensitive)]!=NULL) {
		return *((int*) trie->level[getIndex('$', trie->insensitive)]);
	}
	return -1;
}

void deleteFromTrie(Trie trie, char* str) {
	int i = 0;
	for(;str[i]!='\0';i++) {
		if(trie->level[getIndex(str[i], trie->insensitive)]!=NULL) {
			trie = trie->level[getIndex(str[i], trie->insensitive)];
		} else {
			return;
		}
	}
	trie->level[getIndex('$', trie->insensitive)] = NULL;
}
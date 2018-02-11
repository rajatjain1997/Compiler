#include "trie.h"
#include <stdio.h>
#include <stdlib.h>

// int 27 = 27;

Trie makeTrie() {
	Trie trie = (Trie) malloc(sizeof(struct trie));
	int i = 0;
	for(;i<27;i++) {
		trie->level[i] = NULL;
	}
	return trie;
}

int getIndex(char ch) {
	if(ch>='a'&&ch<='z') {
		return ch-97;
	} else if (ch>='A'&&ch<='Z') {
		return ch-'A';
	} else {
		return 26;
	}
}

void insertInTrie(Trie trie, char* str, int info) {
	int i = 0, index;
	for(;str[i] != '\0' && trie->level[getIndex(str[i])]!=NULL;i++) {
		trie = trie->level[getIndex(str[i])];
	}
	for(;str[i] != '\0'; i++) {
		index = getIndex(str[i]);
		trie->level[index] = makeTrie();
		trie = trie->level[index];
	}
	int* store = (int*) malloc(sizeof(int));
	*store = info;
	trie->level[getIndex('$')] = (Trie) store;
}

int findInTrie(Trie trie, char* str) {
	int i = 0;
	for(;str[i]!='\0';i++) {
		if(trie->level[getIndex(str[i])]!=NULL) {
			trie = trie->level[getIndex(str[i])];
		} else {
			return -1;
		}
	}
	if(trie->level[getIndex('$')]!=NULL) {
		return *((int*) trie->level[getIndex('$')]);
	}
	return -1;
}

void deleteFromTrie(Trie trie, char* str) {
	int i = 0;
	for(;str[i]!='\0';i++) {
		if(trie->level[getIndex(str[i])]!=NULL) {
			trie = trie->level[getIndex(str[i])];
		} else {
			return;
		}
	}
	trie->level[getIndex('$')] = NULL;
}
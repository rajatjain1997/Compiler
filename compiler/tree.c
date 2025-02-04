/**
 *	AUTHOR: Rajat Jain
 *  ID No. 2015A7PS0549P
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"
#include "tree.h"

Tree createTree(struct symbol* root) {
	Tree tree = (Tree) malloc(sizeof(struct tree));
	tree->symbol = root;
	tree->children = createList();
	tree->parent = NULL;
	int i = 0;
	for(;i<ATTR_NOS;i++) {
		tree->attr[i] = NULL;
	}
	return tree;
}

Tree add_child(Tree tree, struct symbol* symbol) {
	Tree child = createTree(symbol);
	Data d;
	d.value.tree = child;
	insertInFront(tree->children, d);
	child->parent = tree;
	return child;
}

struct symbol* extractSymbol(Tree tree) {
	return tree->symbol;
}

void freeTree(Tree tree, int freeAttr) {
	free(tree->symbol);
	if(freeAttr) {
		int i = 0;
		for(;i<ATTR_NOS;i++) {
			free(tree->attr[i]);
		}
	}
	freeList(tree->children);
}

void nullifyAttributes(Tree tree) {
	int i = 0;
	for(;i<ATTR_NOS;i++) {
		tree->attr[i] = NULL;
	}
}

// void visitBFT(Tree tree) {
// 	// printf("%s\n", tree->dir_name);
// }

// void DFT(Tree tree) {
// 	StackData d;
// 	Element temp, x;
// 	Stack st = createStack();
// 	visitDFT(tree);
// 	d.value.element = tree->children->first;
// 	push(st,d);
// 	while(st->size!=0) {
// 		temp = pop(st).value.element;
// 		if(temp!=NULL) {
// 			visitDFT(temp->data.value.tree);
// 			x = temp->data.value.tree->children->first;
// 			temp = temp->next;
// 			d.value.element = temp;
// 			push(st, d);
// 			d.value.element = x;
// 			push(st, d);
// 		}
// 	}
// }

// void BFT(Tree tree) {
// 	DataQueue d;
// 	Element temp;
// 	Queue q = createQueue();
// 	d.value = tree;
// 	enqueue(q, d);
// 	while(q->size!=0) {
// 		tree = dequeue(q).value;
// 		if(tree!=NULL) {
// 			visitBFT(tree);
// 			temp = tree->children->first;
// 			while(temp!=NULL) {
// 				d.value = temp->data.value;
// 				enqueue(q,d);
// 				temp = temp->next;
// 			}
// 		}
// 	}
// }

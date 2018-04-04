#include <stdio.h>
#include <stdlib.h>
#include "tree.h"
#include "symbol.h"
#include "token.h"
#include "list.h"
#include "semantic.h"
#include "symboltable.h"

void visitInh(Tree tree) {
  Symbol* symbol = extractSymbol(tree);
  int flag = 1;
  if(isTerminal(symbol)) {
    switch(symbol->symbolType) {
      case MAIN:
        tree->attr[0] = createSymbolTable();
        break;
      default:
        tree->attr[0] = tree->parent->attr[0];
    }
    switch(symbol->symbolType) {
      case INT: case REAL: case STRING: case MATRIX:
        Element temp = tree->children->first;
        while(temp!=NULL) {
          flag = createidEntry((SymbolTable) tree->attr[0], temp->data.value.tree, symbol->symbolType);
          if(!flag) {
            //Redefined symbol
          }
          temp = temp->next;
        }
        break;
    }
  } else {
    if(symbol->symbolType == lookupSymbolDictionary("<functionDefn>", 0)->symbolType) {
      tree->attr[0] = createfunEntry((SymbolTable) tree->parent->attr[0], tree->children->first->data.value.tree);
      if(tree->attr[0]==NULL) {
        //Redefined funid error
      }
    } else {
      tree->attr[0] = tree->parent->attr[0];
    }
  }
}

void visitSyn(Tree tree) {
  switch(getRuleIndex(getRule(extractSymbol(tree)))) {
  }
}

Tree typeCheck(Tree tree) {
  Element temp;
	if(!isTerminal(tree->symbol)) {
    visitInh(tree);
		temp = tree->children->first;
		while(temp!=NULL) {
			typeCheck(temp->data.value.tree);
			temp = temp->next;
		}
    visitSyn(tree);
  }
  return tree;
}

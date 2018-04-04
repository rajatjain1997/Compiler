#include <stdio.h>
#include <stdlib.h>
#include "tree.h"
#include "token.h"
#include "symbol.h"
#include "list.h"
#include "trie.h"
#include "grammar.h"
#include "semantic.h"

Symbol*** generateSymbolDictionary() {
  Symbol*** dictionary = (Symbol***) malloc(sizeof(Symbol***) * 2);
  dictionary[0] = (Symbol**) malloc(sizeof(Symbol*) * MAX_GRAMMAR_SIZE);
  dictionary[1] = (Symbol**) malloc(sizeof(Symbol*) * (NE+1));
  int i = 0;
  for(;i<MAX_GRAMMAR_SIZE;i++) {
    dictionary[0][i] = generateSymbol(i, 0);
    if(i<=NE) {
      dictionary[1][i] = generateSymbol(i, 1);
    }
  }
  return dictionary;
}

Symbol* lookupSymbolDictionary(char nonterminal[], int terminal) {
  static Symbol*** dictionary = NULL;
  if(dictionary==NULL) {
    dictionary = generateSymbolDictionary();
  }
  Symbol* symbol;
  if(nonterminal[0] != '\0') {
    symbol = dictionary[0][findInTrie(nonTerminalMapping, nonterminal)];
  } else {
    symbol = dictionary[1][terminal];
  }
  return symbol;
}

Tree extractChild(Tree tree ,char nonterminal[], TokenType terminal, int childno) {
  Symbol* symbol = lookupSymbolDictionary(nonterminal, terminal);
  List children = tree->children;
  Element temp = children->first;
  int index = 0;
  while(temp!=NULL) {
    if(
      (
        (
          temp->data.value.tree->attr[ATTR_NOS]==NULL &&
          temp->data.value.tree->symbol->symbolType == symbol->symbolType &&
          isTerminal(temp->data.value.tree->symbol) == isTerminal(symbol)
        ) ||
        (
          temp->data.value.tree->attr[ATTR_NOS]!=NULL &&
          ((Symbol*)(temp->data.value.tree->attr[ATTR_NOS]))->symbolType == symbol->symbolType &&
          isTerminal(((Symbol*)(temp->data.value.tree->attr[ATTR_NOS]))) == isTerminal(symbol)
        )
    ) &&
      ++index == childno
    ) {
      return temp->data.value.tree;
    }
    temp = temp->next;
  }
  return NULL;
}

#include <stdio.h>
#include "tree.h"
#include "grammar.h"
#include "token.h"
#include "symbol.h"
#include "list.h"
#include "set.h"

Symbol*** generateSymbolDictionary() {
  Symbol*** dictionary = (Symbol***) malloc(sizeof(Symbol***) * 2);
  dictionary[0] = (Symbol**) malloc(sizeof(Symbol*) * MAX_GRAMMAR_SIZE);
  dictionary[1] = (Symbol**) malloc(sizeof(Symbol*) * NE);
  int i = 0;
  for(;i<MAX_GRAMMAR_SIZE;i++) {
    dictionary[0][i] = generateSymbol(i, 0);
    if(i<NE) {
      dictionary[1][i] = generateSymbol(i, 1);
    }
  }
  return dictionary;
}

Symbol* lookupSymbolDictionary(char nonterminal[], int terminal) {
  static Symbol*** dictionary = generateSymbolDictionary();
  if(nonterminal[0] == '\0') {
    symbol = dictionary[0][findInTrie(nonTerminalMapping, nonterminal)];
  } else {
    symbol = dictionary[1][terminal]
  }
}

void pruneChildren(List children, List prunelist) {
  if(children->size==0 || prunelist->size==0) {
    return;
  }

  Element temp = children->first->next, prev = children->first;
  Element todelete = prunelist->first, lastdeleted;
  int index = 0, deletefirst = 0, deletelast = 0, initialsize = children->size;

  prunelist->first = NULL;
  prunelist->last = NULL;
  prunelist->size = 0;

  while(temp!=NULL && todelete!=NULL) {
    if(temp->data.value.tree->symbol->symbolType == todelete->data.value.symbol->symbolType &&
      isTerminal(temp->data.value.tree->symbol) == isTerminal(todelete->data.value.symbol)
    ) {
      temp=temp->next;
      lastdeleted = todelete;
      todelete = todelete->next;
      if(index==0) {
        deletefirst = 1;
      } else if(index==initialsize-1) {
        deletelast = 1;
      } else {
        free(temp->prev->data.value.tree->symbol)
        free(temp->prev->data.value.tree)
        free(temp->prev);
      }
      free(lastdeleted);
    } else {
      prev->next = temp;
      temp->prev = prev;
      prev = temp;
      temp = temp->next;
    }
    index++;
  }
  if(deletefirst) {
    deleteFromFront(children);
  }
  if(deletelast) {
    deleteAtEnd(children);
  }
}

void insertInList(List list, void* element) {
  Data d;
  d.value.tree = (Tree) element;
  insertAtEnd(list, d);
}

Tree extractChild(Tree tree ,char nonterminal[], TokenType terminal, int childno) {
  Symbol* symbol = lookupSymbolDictionary(nonterminal, terminal);
  List children = tree->children;
  Element temp = children->first;
  int index = 0;
  while(temp!=NULL) {
    if(temp->data.value.tree->symbol->symbolType == symbol->symbolType &&
      isTerminal(temp->data.value.tree->symbol) == isTerminal(symbol) &&
      ++index == childno
    ) {
      return data.value.tree;
    }
    temp = temp->next;
  }
  return NULL;
}

List transfromTree(Tree tree, Symbol* head, List children) {
  tree->symbol = head;
  List oldchildren = tree->children;
  tree->children = children;
  return oldchildren;
}

void visitInh(Tree tree, Symbol*** dictionary) {
  switch(getRuleIndex(getRule(tree->symbol))) {
    case 0:
  }
}

void visitSyn(Tree tree, Symbol*** dictionary) {
  int i;
  List childList;
  List prunelist = createList();
  Data d;
  Symbol* symbol;
  Tree tempchild;
  switch(getRuleIndex(getRule(tree->symbol))) {
    case 0:
      childList = transfromTree(tree,
        extractChild(tree, "", MAIN, 1)->symbol,
        extractChild(tree, "<prog>", 0, 1)->attr[0]
      );
      insertInList(prunelist, lookupSymbolDictionary("", SQO));
      insertInList(prunelist, lookupSymbolDictionary("", SQC));
      insertInList(prunelist, lookupSymbolDictionary("<prog>", 0));
      insertInList(prunelist, lookupSymbolDictionary("", END));
      break;
    case 1:
      tree->attr[0] = extractChild(tree, "<stmtAndFunctionDefn>", 0, 1)->attr[0];
      d.value.tree = extractChild(tree,"<stmtOrFunctionDefn>", 0, 1)->attr[0];
      insertInFront(tree->attr[0], d);
      childList = tree->children;
      insertInList(prunelist, lookupSymbolDictionary("<stmtAndFunctionDefn>", 0));
      insertInList(prunelist, lookupSymbolDictionary("<stmtOrFunctionDefn>", 0));
      break;
    case 2:
      tree->attr[0] = extractChild(tree, "<stmtAndFunctionDefn>", 0, 1)->attr[0];
      d.value.tree = extractChild(tree,"<stmtOrFunctionDefn>", 1)->attr[0];
      insertInFront(tree->attr[0], d);
      childList = tree->children;
      insertInList(prunelist, lookupSymbolDictionary("<stmtAndFunctionDefn>", 0));
      insertInList(prunelist, lookupSymbolDictionary("<stmtOrFunctionDefn>", 0));
      break;
    case 3:
      tree->attr[0] = createList();
      childList = tree->children;
      break;
    case 4:
      tree->attr[0] = extractChild(tree, "<stmt>", 0, 1)->attr[0];
      childList = tree->children;
      insertInList(prunelist, lookupSymbolDictionary("<stmt>", 0));
      break;
    case 5:
      tree->attr[0] = extractChild(tree, "<functionDefn>", 0, 1)->attr[0];
      childList = tree->children;
      insertInList(prunelist, lookupSymbolDictionary("<functionDefn>", 0));
      break;
    case 6:
      tree->attr[0] = extractChild(tree, "<declarationStmt>", 0, 1)->attr[0];
      childList = tree->children;
      insertInList(prunelist, lookupSymbolDictionary("<declarationStmt>", 0));
      break;
    case 7:
      tree->attr[0] = extractChild(tree, "<assignmentStmt>", 0, 1)->attr[0];
      childList = tree->children;
      insertInList(prunelist, lookupSymbolDictionary("<assignmentStmt>", 0));
      break;
    case 8:
      tree->attr[0] = extractChild(tree, "<ifStmt>", 0, 1)->attr[0];
      childList = tree->children;
      insertInList(prunelist, lookupSymbolDictionary("<ifStmt>", 0));
      break;
    case 9:
      tree->attr[0] = extractChild(tree, "<ioStmt>", 0, 1)->attr[0];
      childList = tree->children;
      insertInList(prunelist, lookupSymbolDictionary("<ioStmt>", 0));
      break;
    case 10:
      tree->attr[0] = extractChild(tree, "<funCallStmt>", 0, 1)->attr[0];
      childList = tree->children;
      insertInList(prunelist, lookupSymbolDictionary("<funCallStmt>", 0));
      break;
    case 11:
      childList = createList();
      insertInList(childList, extractChild(tree, "", FUNID, 1));
      insertInList(childList, extractChild(tree, "<parameterList>", 1));
      insertInList(childList, extractChild(tree, "<parameterList>", 2));
      appendLists(childList, extractChild(tree, "<prog>", 1)->attr[0]);
      childList = transfromTree(tree, tree->symbol, childList);
      insertInList(prunelist, lookupSymbolDictionary("", FUNCTION));
      insertInList(prunelist, lookupSymbolDictionary("", SQO));
      insertInList(prunelist, lookupSymbolDictionary("", SQC));
      insertInList(prunelist, lookupSymbolDictionary("", ASSIGNOP));
      insertInList(prunelist, lookupSymbolDictionary("", SQO));
      insertInList(prunelist, lookupSymbolDictionary("", SQC));
      insertInList(prunelist, lookupSymbolDictionary("<prog>", 0));
      insertInList(prunelist, lookupSymbolDictionary("", END));
      insertInList(prunelist, lookupSymbolDictionary("", SEMICOLON));
      break;
    case 12:
      childList = createList();
      insertInList(childList, extractChild(tree, "", ID, 1));
      appendLists(childList, extractChild(tree, "<remainingList>", 0, 1)->attr[0]);
      childList = transfromTree(tree, tree->symbol, childList);
      insertInList(prunelist, lookupSymbolDictionary("<type>", 0));
      insertInList(prunelist, lookupSymbolDictionary("<remainingList>", 0));
      break;
    case 13:
      tree->attr[0] = extractChild(tree, "", INT, 1);
      break;
    case 14:
      tree->attr[0] = extractChild(tree, "", REAL, 1);
      break;
  }
  pruneChildren(childList, prunelist);
}

Tree createAST(Tree parsetree) {
  Element temp;
	if(tree->children->size!=0) {
    visitInh(tree);
		temp = tree->children->first;
		while(temp!=NULL) {
			createAST(temp->data.value.tree);
			temp = temp->next;
		}
    visitSyn(tree);
	}
  return parsetree;
}

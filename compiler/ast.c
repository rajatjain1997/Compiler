#include <stdio.h>
#include <stdlib.h>
#include "tree.h"
#include "grammar.h"
#include "token.h"
#include "symbol.h"
#include "list.h"
#include "set.h"
#include "ast.h"
#include "trie.h"

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
  static Symbol*** dictionary = NULL;
  if(dictionary==NULL) {
    dictionary = generateSymbolDictionary();
  }
  Symbol* symbol;
  if(nonterminal[0] == '\0') {
    symbol = dictionary[0][findInTrie(nonTerminalMapping, nonterminal)];
  } else {
    symbol = dictionary[1][terminal];
  }
  return symbol;
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
        free(temp->prev->data.value.tree->symbol);
        free(temp->prev->data.value.tree);
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
      return temp->data.value.tree;
    }
    temp = temp->next;
  }
  return NULL;
}

List transformTree(Tree tree, Tree head, List children) {
  if(head!=NULL) {
    tree->symbol = head->symbol;
    free(head);
  }
  List oldchildren = tree->children;
  tree->children = children;
  return oldchildren;
}

void visitInh(Tree tree) {
  switch(getRuleIndex(getRule(tree->symbol))) {
    case 0: break;
  }
}

void visitSyn(Tree tree) {
  int i;
  List childList;
  List prunelist = createList();
  Data d;
  Symbol* symbol;
  Tree tempchild;
  switch(getRuleIndex(getRule(tree->symbol))) {
    case 0: //<mainFunction> MAIN SQO SQC <prog> END
      childList = transformTree(tree, extractChild(tree, "", MAIN, 1), extractChild(tree, "<prog>", 0, 1)->attr[0]);
      insertInList(prunelist, lookupSymbolDictionary("", SQO));
      insertInList(prunelist, lookupSymbolDictionary("", SQC));
      insertInList(prunelist, lookupSymbolDictionary("<prog>", 0));
      insertInList(prunelist, lookupSymbolDictionary("", END));
      break;
    case 1: //<prog> <stmtOrFunctionDefn> <stmtAndFunctionDefn>
      tree->attr[0] = extractChild(tree, "<stmtAndFunctionDefn>", 0, 1)->attr[0];
      d.value.tree = extractChild(tree,"<stmtOrFunctionDefn>", 0, 1)->attr[0];
      insertInFront(tree->attr[0], d);
      childList = tree->children;
      insertInList(prunelist, lookupSymbolDictionary("<stmtAndFunctionDefn>", 0));
      insertInList(prunelist, lookupSymbolDictionary("<stmtOrFunctionDefn>", 0));
      break;
    case 2: //<stmtAndFunctionDefn> <stmtOrFunctionDefn> <stmtAndFunctionDefn1>
      tree->attr[0] = extractChild(tree, "<stmtAndFunctionDefn>", 0, 1)->attr[0];
      d.value.tree = extractChild(tree,"<stmtOrFunctionDefn>", 0, 1)->attr[0];
      insertInFront(tree->attr[0], d);
      childList = tree->children;
      insertInList(prunelist, lookupSymbolDictionary("<stmtAndFunctionDefn>", 0));
      insertInList(prunelist, lookupSymbolDictionary("<stmtOrFunctionDefn>", 0));
      break;
    case 3: //<stmtAndFunctionDefn> $
      tree->attr[0] = createList();
      childList = tree->children;
      break;
    case 4: //<stmtOrFunctionDefn> <stmt>
      tree->attr[0] = extractChild(tree, "<stmt>", 0, 1)->attr[0];
      childList = tree->children;
      insertInList(prunelist, lookupSymbolDictionary("<stmt>", 0));
      break;
    case 5: //<stmtOrFunctionDefn> <functionDefn>
      tree->attr[0] = extractChild(tree, "<functionDefn>", 0, 1)->attr[0];
      childList = tree->children;
      insertInList(prunelist, lookupSymbolDictionary("<functionDefn>", 0));
      break;
    case 6: //<stmt> <declarationStmt>
      tree->attr[0] = extractChild(tree, "<declarationStmt>", 0, 1)->attr[0];
      childList = tree->children;
      insertInList(prunelist, lookupSymbolDictionary("<declarationStmt>", 0));
      break;
    case 7: //<stmt> <assignmentStmt>
      tree->attr[0] = extractChild(tree, "<assignmentStmt>", 0, 1)->attr[0];
      childList = tree->children;
      insertInList(prunelist, lookupSymbolDictionary("<assignmentStmt>", 0));
      break;
    case 8: //<stmt> <ifStmt>
      tree->attr[0] = extractChild(tree, "<ifStmt>", 0, 1)->attr[0];
      childList = tree->children;
      insertInList(prunelist, lookupSymbolDictionary("<ifStmt>", 0));
      break;
    case 9: //<stmt> <ioStmt>
      tree->attr[0] = extractChild(tree, "<ioStmt>", 0, 1)->attr[0];
      childList = tree->children;
      insertInList(prunelist, lookupSymbolDictionary("<ioStmt>", 0));
      break;
    case 10://<stmt> <funCallStmt>
      tree->attr[0] = extractChild(tree, "<funCallStmt>", 0, 1)->attr[0];
      childList = tree->children;
      insertInList(prunelist, lookupSymbolDictionary("<funCallStmt>", 0));
      break;
    case 11://<functionDefn> FUNCTION SQO <parameterList1> SQC ASSIGNOP FUNID SQO <parameterList2> SQC <prog> END SEMICOLON
      childList = createList();
      insertInList(childList, extractChild(tree, "", FUNID, 1));
      insertInList(childList, extractChild(tree, "<parameterList>", 0, 1));
      insertInList(childList, extractChild(tree, "<parameterList>", 0, 2));
      appendLists(childList, extractChild(tree, "<prog>", 0, 1)->attr[0]);
      childList = transformTree(tree, NULL, childList);
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
    case 12://<parameterList> <type> ID <remainingList>
      childList = createList();
      insertInList(childList, extractChild(tree, "", ID, 1));
      appendLists(childList, extractChild(tree, "<remainingList>", 0, 1)->attr[0]);
      tree->attr[0] = childList;
      childList = transformTree(tree, NULL, childList);
      insertInList(prunelist, lookupSymbolDictionary("<type>", 0));
      insertInList(prunelist, lookupSymbolDictionary("<remainingList>", 0));
      break;
    case 13://<type> INT
      tree->attr[0] = extractChild(tree, "", INT, 1);
      childList = tree->children;
      break;
    case 14://<type> REAL
      tree->attr[0] = extractChild(tree, "", REAL, 1);
      childList = tree->children;
      break;
    case 15://<type> STRING
      tree->attr[0] = extractChild(tree, "", STRING, 1);
      childList = tree->children;
      break;
    case 16://<type> MATRIX
      tree->attr[0] = extractChild(tree, "", MATRIX, 1);
      childList = tree->children;
      break;
    case 17://<remainingList> COMMA <parameterList>
      tree->attr[0] = extractChild(tree, "<parameterList>", 0, 1);
      childList = tree->children;
      insertInList(prunelist, lookupSymbolDictionary("", COMMA));
      insertInList(prunelist, lookupSymbolDictionary("<parameterList>", 0));
      break;
    case 18://<remainingList> $
      tree->attr[0] = createList();
      childList = tree->children;
      break;
    case 19://<declarationStmt> <type> <varList> SEMICOLON
      childList = transformTree(tree, extractChild(tree, "<type>", 0, 1)->attr[0], extractChild(tree, "<varList>", 0, 1)->attr[0]);
      insertInList(prunelist, lookupSymbolDictionary("<type>", 0));
      insertInList(prunelist, lookupSymbolDictionary("<varList>", 0));
      break;
    case 20://<assignmentStmt> <variableAssign>
      tree->attr[0] = extractChild(tree, "<variableAssign>", 0, 1)->attr[0];
      childList = tree->children;
      insertInList(prunelist, lookupSymbolDictionary("<variableAssign>", 0));
      break;
    case 21://<assignmentStmt> <functionAssign>
      tree->attr[0] = extractChild(tree, "<functionAssign>", 0, 1)->attr[0];
      childList = tree->children;
      insertInList(prunelist, lookupSymbolDictionary("<functionAssign>", 0));
      break;
    case 22://<variableAssign> ID ASSIGNOP <arithmeticExpression> SEMICOLON
      childList = createList();
      insertInList(childList, extractChild(tree, "", ID, 1));
      insertInList(childList, extractChild(tree, "<arithmeticExpression>", 0, 1));
      childList = transformTree(tree, extractChild(tree, "", ASSIGNOP, 1), childList);
      insertInList(prunelist, lookupSymbolDictionary("", SEMICOLON));
      break;
    case 23://<functionAssign> SQO <varList> SQC ASSIGNOP <rightHandSide> SEMICOLON
      childList = appendLists(extractChild(tree, "<varList>", 0, 1)->attr[0], extractChild(tree, "<rightHandSide>", 0, 1)->attr[0]);
      childList = transformTree(tree, extractChild(tree, "", ASSIGNOP, 1), childList);
      insertInList(prunelist, lookupSymbolDictionary("", SQO));
      insertInList(prunelist, lookupSymbolDictionary("<varList>", 0));
      insertInList(prunelist, lookupSymbolDictionary("", SQC));
      insertInList(prunelist, lookupSymbolDictionary("", SEMICOLON));
    case 24://<rightHandSide> <funCall>
      tree->attr[1] = extractChild(tree, "<funCall>", 0, 1)->attr[0];
      childList = tree->children;
      insertInList(prunelist, lookupSymbolDictionary("<funCall>", 0));
      break;

  }
  pruneChildren(childList, prunelist);
}

Tree createAST(Tree tree) {
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
  return tree;
}

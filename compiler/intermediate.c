#include<stdio.h>
#include<stdlib.h>
#include"tree.h"
#include"token.h"
#include"symbol.h"
#include"quadruple.h"
#include"symboltable.h"
#include"semantic.h"

void generateTypeAttribute(Tree tree) {
  nullifyAttributes(tree);
  Symbol* symbol = extractSymbol(tree);
  SymbolTable scope = (SymbolTable) tree->parent->attr[0];
  Tree temptree; Type type1;
  if(isTerminal(symbol)) {
    switch(symbol->symbolType) {
        case ID:
          tree->attr[1] = makeAddress(retrieveSymbol(scope, getToken(extractSymbol(tree))), 0);
          break;
        default:
          tree->attr[1] = makeAddress(tree, 1);
    }
  }
}
void visitSynCode(Tree tree) {
  Symbol* symbol = extractSymbol(tree);
  SymbolTable scope = (SymbolTable) tree->attr[0];
  Quadruple newCode; List templist; Address* tempAddress1, temoAddress2;
  static Type* type = NULL;
  templist = createList();
  tree->attr[2] = templist;
  if(type==NULL) {
    type = createType(INT, 0, 0);
  }
  int flag = 1;
  if(isTerminal(symbol)) {
    switch(symbol->symbolType) {
      case PLUS:
        tree->attr[1] = generateTemporary((SymbolTable*) tree->attr[0], tree->attr[1]);
        insertAtEndFast(templist,
          makeCode(OP_PLUS,
            extractChildNumber(tree, 1)->attr[1],
            extractChildNumber(tree, 2)->attr[1],
            tree->attr[1]
          ));
        tree->attr[2] = appendLists(extractChildNumber(tree, 2)->attr[2], templist);
        tree->attr[2] = appendLists(extractChildNumber(tree, 1)->attr[2], tree->attr[2]);
      break;
      case MINUS:
        tree->attr[1] = generateTemporary((SymbolTable*) tree->attr[0], tree->attr[1]);
        insertAtEndFast(templist,
          makeCode(OP_MINUS,
            extractChildNumber(tree, 1)->attr[1],
            extractChildNumber(tree, 2)->attr[1],
            tree->attr[1]
          ));
        tree->attr[2] = appendLists(extractChildNumber(tree, 2)->attr[2], templist);
        tree->attr[2] = appendLists(extractChildNumber(tree, 1)->attr[2], tree->attr[2]);
      break;
      case MUL:
        tree->attr[1] = generateTemporary((SymbolTable*) tree->attr[0], tree->attr[1]);
        insertAtEndFast(templist,
          makeCode(OP_MUL,
            extractChildNumber(tree, 1)->attr[1],
            extractChildNumber(tree, 2)->attr[1],
            tree->attr[1]
          ));
        tree->attr[2] = appendLists(extractChildNumber(tree, 2)->attr[2], templist);
        tree->attr[2] = appendLists(extractChildNumber(tree, 1)->attr[2], tree->attr[2]);
      break;
      case DIV:
        tree->attr[1] = generateTemporary((SymbolTable*) tree->attr[0], tree->attr[1]);
        insertAtEndFast(templist,
          makeCode(OP_DIV,
            extractChildNumber(tree, 1)->attr[1],
            extractChildNumber(tree, 2)->attr[1],
            tree->attr[1]
          ));
        tree->attr[2] = appendLists(extractChildNumber(tree, 2)->attr[2], templist);
        tree->attr[2] = appendLists(extractChildNumber(tree, 1)->attr[2], tree->attr[2]);
      break;
      case ID:
        tree->attr[1] = generateTemporary((SymbolTable*) tree->attr[0], type);
        tempAddress1 = generateTemporary(scope, type);
        insertAtEndFast(templist,
          makeCode(OP_MUL,
            makeAddress(sizeLookup(INT), 1),
            makeAddress(extractChildNumber(tree, 2), 1),
            tempAddress1));
        tempAddress2 = generateTemporary(scope, type);
        insertAtEndFast(templist,
          makeCode(OP_PLUS,
            tempAddress1,
            makeAddress(extractChildNumber(tree, 1), 1),
            tempAddress2));
          //Need to insert a code for adding base address of matrix to the element and storing it in tree->attr[1]
        break;
    }
  } else {
    if(symbolComparatorNT(symbol, "<matrix>")) {
      tree->attr[1] = makeAddress(tree, 1);
    }
  }
}

List generateCode(Tree tree) {
  Element temp;
  if(tree->children->size!=0) {
  	temp = tree->children->first;
  	while(temp!=NULL) {
  		generateCode(temp->data.value.tree);
  		temp = temp->next;
  	}
    visitSynCode(tree);
  } else {
    generateLabelAttribute(tree);
  }
  return tree->attr[2];
}

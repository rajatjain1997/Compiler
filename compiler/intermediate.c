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
  tree->attr[0] = createList();
  if(isTerminal(symbol)) {
    switch(symbol->symbolType) {
        case ID:
          tree->attr[1] = makeAddress(retrieveSymbol(scope, getToken(extractSymbol(tree))), 0);
          break;
        case INT:
          tree->attr[1] = makeAddress(getToken(extractChild(tree))->value.integer, 1);
          break;
        case REAL:
          tree->attr[1] = makeAddress(getToken(extractChild(tree))->value.real, 1);
          break;
        case STRING:
          tree->attr[1] = makeAddress(getToken(extractChild(tree))->value.string, 1);
          break;
    }
  }
}
void visitSynCode(Tree tree) {
  Symbol* symbol = extractSymbol(tree);
  SymbolTable scope = (SymbolTable) tree->attr[0];
  Quadruple newCode; List codelist; Address* tempAddress1, temoAddress2;
  Type* temptype;
  Element temp;
  static Type* type = NULL;
  codelist = createList();
  tree->attr[0] = codelist;
  if(type==NULL) {
    type = createType(INT, 0, 0);
  }
  int flag = 1;
  if(isTerminal(symbol)) {
    switch(symbol->symbolType) {
      case PLUS:
        tree->attr[1] = generateTemporary((SymbolTable*) tree->attr[0], tree->attr[1]);
        insertAtEndFast(codelist,
          makeCode(OP_PLUS,
            extractChildNumber(tree, 1)->attr[1],
            extractChildNumber(tree, 2)->attr[1],
            tree->attr[1]
          ));
        tree->attr[0] = appendLists(extractChildNumber(tree, 2)->attr[2], codelist);
        tree->attr[0] = appendLists(extractChildNumber(tree, 1)->attr[2], tree->attr[2]);
      break;
      case MINUS:
        tree->attr[1] = generateTemporary(scope, tree->attr[1]);
        insertAtEndFast(codelist,
          makeCode(OP_MINUS,
            extractChildNumber(tree, 1)->attr[1],
            extractChildNumber(tree, 2)->attr[1],
            tree->attr[1]
          ));
        tree->attr[0] = appendLists(extractChildNumber(tree, 2)->attr[2], codelist);
        tree->attr[0] = appendLists(extractChildNumber(tree, 1)->attr[2], tree->attr[2]);
      break;
      case MUL:
        tree->attr[1] = generateTemporary(scope, tree->attr[1]);
        insertAtEndFast(codelist,
          makeCode(OP_MUL,
            extractChildNumber(tree, 1)->attr[1],
            extractChildNumber(tree, 2)->attr[1],
            tree->attr[1]
          ));
        tree->attr[0] = appendLists(extractChildNumber(tree, 2)->attr[2], codelist);
        tree->attr[0] = appendLists(extractChildNumber(tree, 1)->attr[2], tree->attr[2]);
      break;
      case DIV:
        tree->attr[1] = generateTemporary(scope, tree->attr[1]);
        insertAtEndFast(codelist,
          makeCode(OP_DIV,
            extractChildNumber(tree, 1)->attr[1],
            extractChildNumber(tree, 2)->attr[1],
            tree->attr[1]
          ));
        tree->attr[0] = appendLists(extractChildNumber(tree, 2)->attr[2], codelist);
        tree->attr[0] = appendLists(extractChildNumber(tree, 1)->attr[2], tree->attr[2]);
      break;
      case ID:
        tree->attr[1] = generateTemporary(scope, type);
        tempAddress1 = generateTemporary(scope, type);
        insertAtEndFast(codelist,
          makeCode(OP_MUL,
            makeAddress(sizeLookup(INT), 1),
            makeAddress(getToken(extractSymbol(extractChildNumber(tree, 2)))->value.integer, 1),
            tempAddress1));
        tempAddress2 = generateTemporary(scope, type);
        insertAtEndFast(codelist,
          makeCode(OP_PLUS,
            tempAddress1,
            makeAddress(getToken(extractSymbol(extractChildNumber(tree, 1)))->value.integer, 1),
            tempAddress2));
        insertAtEndFast(codelist,
        makeCode(OP_ADDRPLUS,
          tempAddress2,
          makeAddress(retrieveSymbol(scope, getToken(extractSymbol(tree))), 0),
          tree->attr[1]));
      break;
      case SIZE:
        temptype = fetchType(scope, extractChildNumber(tree, 1));
        if(temptype->type==INT || temptype->type==REAL) {
          tree->attr[1] = makeAddress(sizeLookup(temptype->type), 1);
        } else if(temptype->type==STRING) {
          tree->attr[1] = makeAddress(temptype->columns, 1);
        } else {
          tree->attr[1] = createList();
          insertAtEndFast(tree->attr[1], makeAddress(temptype->rows, 1));
          insertAtEndFast(tree->attr[1], makeAddress(temptype->columns, 1));
        }
      break;
      case FUNID:
        temp = tree->children->first;
        while(temp!=NULL) {
          insertAtEndFast(codelist,
            makeCode(OP_PARAM,
              temp->data.value.tree->attr[1],
              NULL,
              NULL));
          temp = temp->next;
        }
        temptree = fetchfunDefn(scope, tree);
        if(extractChild(temptree, "<parameterList>", 0, 1)->children->size==1) {
          tree->attr[1] = generateTemporary(scope, tree->attr[1]);
        } else {
          tree->attr[1] = createList();
          temp = extractChild(temptree, "<parameterList>", 0, 1)->children->first;
          while(temp!=NULL) {
            tempAddress1 = generateTemporary(scope, fetchType(fetchfunScope(scope, tree),
                                              extractChildNumber(temp->data.value.tree, 1)));
            insertAtEndFast(temp->attr[1], tempAddress1);
            temp = temp->next;
          }
        }
        insertAtEndFast(codelist,
          makeCode(OP_CALL,
            makeAddress(temptree, 0),
            makeAddress(tree->children->size, 1),
            makeAddress(extractChild(temptree, "<parameterList>", 0, 1)->children->size, 1)));
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

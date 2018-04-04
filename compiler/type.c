#include <stdio.h>
#include <stdlib.h>
#include "tree.h"
#include "symbol.h"
#include "token.h"
#include "list.h"
#include "semantic.h"
#include "symboltable.h"

int sizeLookup(int type) {
  switch(type) {
    case INT: return 4;
    case REAL: return 4;
    case STRING: case MATRIX: return 0;
  }
}

void visitInh(Tree tree) {
  Symbol* symbol = extractSymbol(tree);
  Element temp;
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
        temp = tree->children->first;
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
  Symbol* symbol = extractSymbol(tree);
  SymbolTable scope = (SymbolTable) tree->attr[0];
  Element temp, temp2; Tree temptree; Type* type1, type2;
  int flag = 1;
  if(isTerminal(symbol)) {
    switch(symbol->symbolType) {
      case ASSIGNOP:
        temp = tree->children->first;
        temptree = fetchfunDefn(scope, tree->children->last.data.value.tree);
        if(temptree!=NULL) {
          temp2 = temptree->children->first->next->data.value.tree->children->first;
          while(temp->next!=NULL || temp2!=NULL) {
            type1 = fetchType(scope, temp1->data.value.tree);
            if(type1==NULL) {
              //Not declared error
            }
            if(extractSymbol(temp2->data.value.tree)->symbolType!=type1->type) {
              //Type mismatch error
            }
            temp = temp->next;
            temp2 = temp2->next;
          }
          if(temp->next!=NULL) {
            //Extra LHS
          } else if(temp2!=NULL) {
            //Extra RHS
          }
        } else {
          temp = tree->children->first;
          type1 = (Type*) tree->children->last->data.value.tree->attr[1];
          if(type1->columns==2 && type1->type==INT) {
            if(tree->children->size>2) {
              //Extra LHS
            } else if(tree->children->size<2) {
              //Extra RHS
            } else {
              while(temp!=NULL) {
                type2 = fetchType(scope, temp->data.value.tree);
                if(type2==NULL) {
                  //Not declared error
                }
                if(type2->type!=INT) {
                  //type mismatch
                }
                temp = temp->next;
              }
            }
          } else {
            type2 = fetchType(scope, temp->data.value.tree);
            if(type1->type==MATRIX || type1->type==STRING) {
              if(!updateidEntrySize(st, temp->data.value.tree, type1->type, type1->rows, type1->columns)) {
                //Type mismatch
              }
            } else if(type1->type!=type2->type || type1->rows!=type2->rows || type1->columns!=type2->columns) {
              //Type mismatch
            }
          }
        }
        break;
    }
  }
}

Tree typeCheck(Tree tree) {
  Element temp;
	if(tree->children->size!=0) {
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

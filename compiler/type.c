#include <stdio.h>
#include <stdlib.h>
#include "tree.h"
#include "symbol.h"
#include "token.h"
#include "list.h"
#include "semantic.h"
#include "symboltable.h"

/*
 * Checkings reqd:
 * LHS type = RHS type
 * Redefined symbols
 * Redefined functions
 * Symbol table maintainenace
 * All returnvalues must be assigned in a fxn
 * Function call values
 * Any independed funCall must return 0 arguments
 * Boolean Expr must be boolean type
 * Arithmetic type generation
 * Recursion removal
 */

int sizeLookup(int type) {
  switch(type) {
    case INT: return 4;
    case REAL: return 4;
    case STRING: case MATRIX: return 0;
    case STR: return 1;
  }
}

int symbolComparatorNT(Symbol* s, char str[]) {
  return s->symbolType == lookupSymbolDictionary(str, 0)->symbolType;
}

void markDefinedVars(SymbolTable st, List vars) {
  Element temp = vars->first;
  while(temp->next!=NULL) {
    updateidDefined(st, temp->data.value.tree);
    temp = temp->next;
  }
}

int typeComparator(SymbolTable st, Tree identifier, Type* type2, Type* type1) {
  if(type1->type==MATRIX || type1->type==STRING) {
    if(!updateidEntrySize(st, identifier, type1->type, type1->rows, type1->columns)) {
      return 0;
    }
  } else if(type1->type!=type2->type || type1->rows!=type2->rows || type1->columns!=type2->columns) {
    return 0;
  }
  return 1;
}

int checkStmt(Tree tree) {
  Symbol* sy = extractSymbol(tree->parent);
  if(
      sy->symbolType == lookupSymbolDictionary("<functionDefn>", 0)->symbolType && isTerminal(sy) == 0||
      sy->symbolType == MAIN && isTerminal(sy) == 1 ||
      sy->symbolType == IF && isTerminal(sy) == 1 ||
      sy->symbolType == ELSE && isTerminal(sy) == 1
    ) {
    return 1;
  }
  return 0;
}

void generateTypeAttribute(Tree tree) {
  nullifyAttributes(tree);
  Symbol* symbol = extractSymbol(tree);
  SymbolTable scope = (SymbolTable) tree->parent->attr[0];
  Tree temptree; Type type1;
  if(isTerminal(symbol)) {
    switch(symbol->symbolType) {
      case NUM:
        tree->attr[1] = createType(INT, 0, 0);
        break;
      case RNUM:
        tree->attr[1] = createType(REAL, 0, 0);
        break;
      case STR:
        tree->attr[1] = createType(STRING, 0, getToken(extractSymbol(tree))->value.string->size);
        break;
      case ID:
        tree->attr[1] = fetchType(scope, tree);
        if(tree->attr[1]==NULL) {
          //Not declared
          break;
        }
        break;
    }
  }
}

void visitInhType(Tree tree) {
  nullifyAttributes(tree);
  Symbol* symbol = extractSymbol(tree);
  Element temp; SymbolTable scope;
  Type *type1, *type2; Tree temptree;
  int flag = 1;
  if(isTerminal(symbol)) {
    switch(symbol->symbolType) {
      case MAIN:
        tree->attr[0] = createSymbolTable();
        break;
      default:
        tree->attr[0] = tree->parent->attr[0];
    }
    scope = (SymbolTable) tree->attr[0];
    switch(symbol->symbolType) {
      case INT: case REAL: case STRING: case MATRIX:
        temp = tree->children->first;
        while(temp!=NULL) {
          flag = createidEntry((SymbolTable) tree->attr[0], temp->data.value.tree, symbol->symbolType);
          if(!flag) {
            //Redefined symbol
            break;
          }
          temp = temp->next;
        }
        break;
      case ID:
        tree->attr[1] = fetchType(scope, tree);
        if(((Type*) tree->attr[1])->type == MATRIX && tree->children->size==2) {
          tree->attr[1] = createType(INT, 0, 0);
        }
      case SIZE:
        type1 = fetchType(scope, tree->children->first->data.value.tree);
        if(type1->type==MATRIX) {
          tree->attr[1] = createType(INT, 0, 2);
        } else {
          tree->attr[1] = createType(INT, 0, 0);
        }
        break;
      case FUNID:
        temptree = fetchfunDefn(scope, tree);
        if(temptree==NULL) {
          //Not declared
          break;
        }
        temptree = extractChildNumber(temptree, 2);
        if(!temptree->children->size==0 && !temptree->children->size>1) {
          tree->attr[1] = fetchType(fetchfunScope(scope, tree), extractChildNumber(extractChildNumber(temptree,1),1));
        }
        break;
    }
  } else {
    if(symbolComparatorNT(symbol, "<functionDefn>")) {
      tree->attr[0] = createfunEntry((SymbolTable) tree->parent->attr[0], extractChildNumber(tree, 1));
      if(tree->attr[0]==NULL) {
        //Redefined funid error
      }
    } else {
      tree->attr[0] = tree->parent->attr[0];
    }
    if(symbolComparatorNT(symbol, "<matrix>")) {
      tree->attr[1] = createType(MATRIX, tree->children->size, extractChildNumber(tree, 1)->children->size);
    }
  }
}

void visitSynType(Tree tree) {
  Symbol* symbol = extractSymbol(tree);
  SymbolTable scope = (SymbolTable) tree->attr[0];
  Element temp, temp2; Tree temptree; Type *type1, *type2;
  int flag = 1;
  if(isTerminal(symbol)) {
    switch(symbol->symbolType) {
      case ASSIGNOP:
        temp = tree->children->first;
        temptree = fetchfunDefn(scope, extractChildNumber(tree, tree->children->size));
        if(temptree!=NULL) {
          temp2 = extractChild(temptree, "<parameterList>", 0, 1)->children->first;
          while(temp->next!=NULL || temp2!=NULL) {
            type1 = fetchType(scope, temp->data.value.tree);
            type2 = fetchType(fetchfunScope(scope, extractChildNumber(tree, tree->children->size)),
                              extractChildNumber(temp2->data.value.tree, 1));
            if(type1==NULL) {
              //Not declared error
              break;
            }
            if(!typeComparator(scope, temp->data.value.tree, type1, type2)) {
              //Type mismatch error
            }
            temp = temp->next;
            temp2 = temp2->next;
          }
          if(temp->next!=NULL) {
            //Extra LHS
            break;
          } else if(temp2!=NULL) {
            //Extra RHS
            break;
          }
        } else {
          type1 = (Type*) extractChildNumber(tree, tree->children->size)->attr[1];
          if(type1==NULL) {
            //Error type
            break;
          }
          if(type1->columns==2 && type1->type==INT) {
            if(tree->children->size>2) {
              //Extra LHS
              break;
            } else if(tree->children->size<2) {
              //Extra RHS
              break;
            } else {
              while(temp!=NULL) {
                type2 = fetchType(scope, temp->data.value.tree);
                if(type2==NULL) {
                  //Not declared error
                  break;
                }
                if(type2->type!=INT) {
                  //type mismatch
                  break;
                }
                temp = temp->next;
              }
            }
          } else {
            type2 = (Type*) temp->data.value.tree->attr[1];
            if(!typeComparator(scope, temp->data.value.tree, type2, type1)) {
              //Type mismatch
              break;
            }
          }
        }
        markDefinedVars(scope, tree->children);
        break;
      case FUNID:
        temp = tree->children->first;
        temptree = fetchfunDefn(scope, tree);
        if(temptree==NULL) {
          //Function not defined
        }
        temp2 = extractChild(temptree, "<parameterList>", 0, 2)->children->first;
        while(temp!=NULL || temp2!=NULL) {
          type1 = fetchType(scope, temp->data.value.tree);
          type2 = fetchType(fetchfunScope(scope, extractChildNumber(tree, tree->children->size)),
                            extractChildNumber(temp2->data.value.tree, 1));
          if(type1==NULL) {
            //Not declared error
            break;
          }
          if(!typeComparator(scope, extractChildNumber(temp2->data.value.tree, 1), type2, type1)) {
            //Type mismatch error
            break;
          }
          temp = temp->next;
          temp2 = temp2->next;
        }
        if(temp!=NULL || temp2!=NULL) {
          // Arguments mismatch
          break;
        }
        if(checkStmt(tree) && extractChild(temptree, "<parameterList>", 0, 2)->children->size!=0) {
          //Non-zero arg fxn called without assign
          break;
        }
        break;
      case IF:
        type1 = (Type*) extractChildNumber(tree, 1)->attr[1];
        if(type1==NULL || type1->type!=AND) {
          //Non boolean if condn
          break;
        }
      case AND: case OR:
        temptree = extractChildNumber(tree, 1);
        type1 = ((Type*) extractChildNumber(tree, 2)->attr[1]);
        if(type1==NULL || temptree->attr[1] == NULL) {
          //Type mismatch error
          break;
        }
        if(((Type*) temptree->attr[1])->type == AND && type1->type == AND) {
          tree->attr[1] = temptree->attr[1];
        } else {
          //Non-Boolean Expr
          break;
        }
        break;
      case NOT:
        temptree = extractChildNumber(tree, 1);
        if(temptree->attr[1] == NULL) {
          //Type mismatch error
          break;
        }
        if(((Type*) temptree->attr[1])->type == AND) {
          tree->attr[1] = temptree->attr[1];
        } else {
          //Non-Boolean Expr
          break;
        }
        break;
      case LT: case LE: case EQ: case GT: case GE: case NE:
        type1 = (Type*) extractChildNumber(tree, 1)->attr[1];
        type2 = (Type*) extractChildNumber(tree, 2)->attr[1];
        if(type1==NULL || type1==NULL) {
          //Type mismatch error
          break;
        }
        if(
            (type1->type == INT || type1->type == REAL) && type1->rows == 0 && type1->columns == 0 &&
            (type2->type == INT || type2->type == REAL) && type2->rows == 0 && type2->columns == 0
          ) {
          tree->attr[1] = createType(AND, 0, 0);
        } else {
          //Non-Boolean Expr
          break;
        }
        break;
      case PLUS:
        type1 = (Type*) extractChildNumber(tree, 1)->attr[1];
        type2 = (Type*) extractChildNumber(tree, 2)->attr[1];
        if(type1==NULL || type1==NULL) {
          //Type mismatch error
          break;
        }
        if(
            (type1->type == INT) && type1->rows == 0 && type1->columns == 0 &&
            (type2->type == INT) && type2->rows == 0 && type2->columns == 0
          ) {
          tree->attr[1] = type2;
        } else if (
              (type1->type == REAL) && type1->rows == 0 && type1->columns == 0 &&
              (type2->type == REAL) && type2->rows == 0 && type2->columns == 0
          ) {
          tree->attr[1] = type1;
        } else if (
              (type1->type == MATRIX) &&
              (type2->type == MATRIX)
          ) {
          if(type1->rows==type2->rows && type1->columns==type2->columns) {
            tree->attr[1] = type1;
          } else {
            //Matrix dimensions not matching
            break;
          }
        } else if (
              (type1->type == STRING) &&
              (type2->type == STRING)
          ) {
          tree->attr[1] = createType(STRING, 0, type1->columns + type2->columns);
        } else {
          //Operator mismatch
          break;
        }
        break;
      case MINUS: case MUL:
        type1 = (Type*) extractChildNumber(tree, 1)->attr[1];
        type2 = (Type*) extractChildNumber(tree, 2)->attr[1];
        if(type1==NULL || type1==NULL) {
          //Type mismatch error
          break;
        }
        if(
            (type1->type == INT) && type1->rows == 0 && type1->columns == 0 &&
            (type2->type == INT) && type2->rows == 0 && type2->columns == 0
          ) {
          tree->attr[1] = type2;
        } else if (
              (type1->type == REAL) && type1->rows == 0 && type1->columns == 0 &&
              (type2->type == REAL) && type2->rows == 0 && type2->columns == 0
          ) {
          tree->attr[1] = type1;
        } else {
          //Operator mismatch
          break;
        }
        break;
      case DIV:
        type1 = (Type*) extractChildNumber(tree, 1)->attr[1];
        type2 = (Type*) extractChildNumber(tree, 2)->attr[1];
        if(type1==NULL || type1==NULL) {
          //Type mismatch error
          break;
        }
        if(
            (type1->type == INT || type1->type == REAL) && type1->rows == 0 && type1->columns == 0 &&
            (type2->type == INT || type2->type == REAL) && type2->rows == 0 && type2->columns == 0
          ) {
          tree->attr[1] = createType(REAL, 0, 0);
        } else {
          //Operator mismatch
          break;
        }
        break;
    }
  } else {
    if(symbolComparatorNT(symbol, "<functionDefn>")) {
      temp = extractChild(tree, "<parameterList>", 0, 1)->children->first;
      while(temp!=NULL) {
        if(!fetchDefined(scope, extractChildNumber(temp->data.value.tree, 1))) {
          //Undefined argument error
          break;
        }
        temp = temp->next;
      }
    }
  }
}

void typeCheck(Tree tree) {
  Element temp;
  if(tree->children->size!=0) {
    visitInhType(tree);
  	temp = tree->children->first;
  	while(temp!=NULL) {
  		typeCheck(temp->data.value.tree);
  		temp = temp->next;
  	}
    visitSynType(tree);
  } else {
    generateTypeAttribute(tree);
  }
}

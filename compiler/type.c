/**
 *	AUTHOR: Rajat Jain
 *  ID No. 2015A7PS0549P
 */

#include <stdio.h>
#include <stdlib.h>
#include "tree.h"
#include "symbol.h"
#include "token.h"
#include "list.h"
#include "semantic.h"
#include "symboltable.h"
#include "error.h"

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
 * Matrices should be rectangular
 * Read does not allow strings or matrices to be read
 */

 Type* errorType = NULL;

 char* typeLookup(Type* type, char* buf) {
   if(type==NULL) {
     sprintf(buf, "ERROR");
     return buf;
   }
   switch(type->type) {
     case REAL: sprintf(buf, "REAL"); break;
     case STRING: sprintf(buf, "STRING(%d)", type->columns); break;
     case MATRIX: sprintf(buf, "MATRIX(%d, %d)", type->rows, type->columns); break;
     case AND: sprintf(buf, "BOOLEAN");
     case INT:
      if(type->columns==0) {
        sprintf(buf, "INT");
      } else {
        sprintf(buf, "TYPE");
      }
      break;
      default: sprintf(buf, "ERROR");
   }
   return buf;
 }

/**
 * void raiseNotDeclaredException(Tree tree): Raises an error if an identifier or function is not declared.
 */
void raiseNotDeclaredException(Tree tree) {
  char msg[256], buf[20];
  Token* token = getToken(extractSymbol(tree));
 	ErrorType e = ERROR;
  getLexeme(token, buf);
 	sprintf(msg, "SEMANTIC ERROR: %s has not been declared before use.", buf);
 	error(msg, e, token->lineno);
  tree->attr[1] = errorType;
}

/**
 * void raiseReDefinedException(Tree tree): Raises an error if an identifier or function is re-declared.
 */
void raiseReDefinedException(Tree tree) {
  char msg[256], buf[20];
  Token* token = getToken(extractSymbol(tree));
 	ErrorType e = ERROR;
  getLexeme(token, buf);
 	sprintf(msg, "SEMANTIC ERROR: %s has already been defined earlier", buf);
 	error(msg, e, token->lineno);
  tree->attr[1] = errorType;
}

/**
 * void raiseTypeMismatchError(Tree identifier, Type* expectedType, Type* actualType, char strtype[]):
 * Raises an error if types mismatch for 2 identifers.
 */
void raiseTypeMismatchError(Tree identifier, Type* expectedType, Type* actualType, char strtype[]) {
  char msg[256], buf1[20], buf2[20], buf3[20];
  Token* token = getToken(extractSymbol(identifier));
 	ErrorType e = ERROR;
  getLexeme(token, buf1);
  if(expectedType!=NULL) {
 	  sprintf(msg, "SEMANTIC ERROR: %s has type %s but expected type is %s", buf1, typeLookup(actualType, buf2), typeLookup(expectedType, buf3));
  } else {
    sprintf(msg, "SEMANTIC ERROR: %s has type %s but expected type is %s", buf1, typeLookup(actualType, buf2), strtype);
  }
 	error(msg, e, token->lineno);
  identifier->attr[1] = errorType;
  //If you later need to assign an error type for all later occurances you can do it in ASSIGNOP after this call.
}

/**
 * void raiseInvalidOperatorError(Tree identifier, Type* expectedType, Type* actualType):
 * Raises an error if an operator is not compatible with 2 types.
 */
void raiseInvalidOperatorError(Tree identifier, Type* expectedType, Type* actualType) {
  char msg[256], buf1[20], buf2[20], buf3[20];
  Token* token = getToken(extractSymbol(identifier));
 	ErrorType e = ERROR;
  getLexeme(token, buf1);
  sprintf(msg, "SEMANTIC ERROR: %s does not work with types %s and %s", buf1, typeLookup(actualType, buf2), typeLookup(expectedType, buf3));
 	error(msg, e, token->lineno);
  identifier->attr[1] = errorType;
  //If you later need to assign an error type for all later occurances you can do it in ASSIGNOP after this call.
}

/**
 * void raiseInsufficientLHS(Tree tree): Raises an error if there are less elements in LHS of ASSIGNOP than on the right.
 */
void raiseInsufficientLHS(Tree tree) {
  char msg[256];
  Token* token = getToken(extractSymbol(tree));
 	ErrorType e = ERROR;
 	sprintf(msg, "SEMANTIC ERROR: LHS of = doesn't have enough identifiers");
 	error(msg, e, token->lineno);
}

/**
 * void raiseInsufficientRHS(Tree tree): Raises an error if there are less elements in RHS of ASSIGNOP than on the left.
 */
void raiseInsufficientRHS(Tree tree) {
  char msg[256];
  Token* token = getToken(extractSymbol(tree));
 	ErrorType e = ERROR;
 	sprintf(msg, "SEMANTIC ERROR: RHS of = doesn't have enough return values");
 	error(msg, e, token->lineno);
}

/**
 * void raiseUntypedError(Tree tree): Raises an error if an untyped object is inserted in an arithmetic
 * or boolean expression.
 */
void raiseUntypedError(Tree tree) {
  char msg[256], buf[20];
  Token* token = getToken(extractSymbol(tree));
 	ErrorType e = ERROR;
  getLexeme(token, buf);
 	sprintf(msg, "SEMANTIC ERROR: %s is not a typed object", buf);
 	error(msg, e, token->lineno);
  tree->attr[1] = errorType;
}

/**
 * void raiseBadFunctionCall(Tree tree): Raises an error if there are wrong number of parameters ina function call.
 */
void raiseBadFunctionCall(Tree tree) {
  char msg[256], buf[20];
  Token* token = getToken(extractSymbol(tree));
  getLexeme(token, buf);
 	ErrorType e = ERROR;
 	sprintf(msg, "SEMANTIC ERROR: Function call for %s has wrong number of parameters.", buf);
 	error(msg, e, token->lineno);
}

/**
 * void raiseNonZeroRetFunction(Tree tree): Raises an error if a function that returns 1 or more values in a
 * function statement.
 */
void raiseNonZeroRetFunction(Tree tree) {
  char msg[256], buf[20];
  Token* token = getToken(extractSymbol(tree));
  getLexeme(token, buf);
 	ErrorType e = ERROR;
 	sprintf(msg, "SEMANTIC ERROR: Function call for %s has returns some values but is called as a statement.", buf);
 	error(msg, e, token->lineno);
}

void raiseErronousIfCondn(Tree tree) {
  char msg[256];
  Token* token = getToken(extractSymbol(tree));
 	ErrorType e = ERROR;
 	sprintf(msg, "SEMANTIC ERROR: If has a non boolean check condition");
 	error(msg, e, token->lineno);
}

void raiseUnAssignedRet(Tree tree, Tree fundef) {
  char msg[256], buf[20], buf2[20];
  Token* token = getToken(extractSymbol(tree));
  getLexeme(token, buf);
  token = getToken(extractSymbol(fundef));
  getLexeme(token, buf2);
 	ErrorType e = ERROR;
 	sprintf(msg, "SEMANTIC ERROR: %s has been not been assigned a return value in the function %s.", buf, buf2);
 	error(msg, e, token->lineno);
}

void raiseNonRectangularMatrix(Tree tree) {
  char msg[256], buf[20];
 	ErrorType e = ERROR;
 	sprintf(msg, "SEMANTIC ERROR: Matrix is non-rectangular.");
 	error(msg, e, getToken(extractSymbol(extractChildNumber(extractChildNumber(tree, 1), 1)))->lineno);
  tree->attr[1] = errorType;
}

void raiseLargeMatrix(Tree tree) {
  char msg[256], buf[20];
 	ErrorType e = ERROR;
 	sprintf(msg, "SEMANTIC ERROR: Matrix is larger than 10x10.");
 	error(msg, e, getToken(extractSymbol(extractChildNumber(extractChildNumber(tree, 1), 1)))->lineno);
  tree->attr[1] = errorType;
}

void raiseInvalidRead(Tree tree, Type* type) {
  char msg[256], buf[20];
  Token* token = getToken(extractSymbol(tree));
 	ErrorType e = ERROR;
 	sprintf(msg, "SEMANTIC ERROR: Read does not support %s type", typeLookup(type, buf));
 	error(msg, e, token->lineno);
}

void markDefinedVars(SymbolTable st, List vars) {
  Element temp = vars->first;
  while(temp->next!=NULL) {
    updateidDefined(st, temp->data.value.tree);
    temp = temp->next;
  }
}

int typeComparator(SymbolTable st, Tree identifier, Type* type2, Type* type1) {
  if((type1->type==MATRIX || type1->type==STRING) && type2->rows==0 && type2->columns==0) {
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
          raiseNotDeclaredException(tree);
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
        errorType = createType(MAIN, -1, -1);
        tree->attr[0] = createSymbolTable(NULL, NULL);
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
            raiseReDefinedException(temp->data.value.tree);
          }
          temp = temp->next;
        }
        break;
      case ID:
        tree->attr[1] = fetchType(scope, tree);
        if(tree->attr[1]==NULL) {
          raiseNotDeclaredException(tree);
          break;
        }
        if(((Type*) tree->attr[1])->type == MATRIX && tree->children->size==2) {
          tree->attr[1] = createType(INT, 0, 0);
        }
        break;
      case SIZE:
        type1 = fetchType(scope, tree->children->first->data.value.tree);
        if(type1==NULL) {
          raiseNotDeclaredException(tree->children->first->data.value.tree);
          break;
        }
        if(type1->type==MATRIX) {
          tree->attr[1] = createType(INT, 0, 2);
        } else {
          tree->attr[1] = createType(INT, 0, 0);
        }
        break;
      case FUNID:
        temptree = fetchfunDefn(scope, tree);
        if(temptree==NULL) {
          raiseNotDeclaredException(tree);
          break;
        }
        temptree = extractChildNumber(temptree, 2);
        if(temptree->children->size==1) {
          tree->attr[1] = fetchType(fetchfunScope(scope, tree), extractChildNumber(extractChildNumber(temptree,1),1));
        }
        break;
    }
  } else {
    if(symbolComparatorNT(symbol, "<functionDefn>")) {
      tree->attr[0] = createfunEntry((SymbolTable) tree->parent->attr[0], extractChildNumber(tree, 1));
      if(tree->attr[0]==NULL) {
        tree->attr[0] = createSymbolTable(tree->parent->attr[0], extractChildNumber(tree, 1));
        raiseReDefinedException(extractChild(tree, "", FUNID, 1));
      }
    } else {
      tree->attr[0] = tree->parent->attr[0];
    }
    if(symbolComparatorNT(symbol, "<matrix>")) {
      temp = tree->children->first;
      flag = extractChildNumber(tree, 1)->children->size;
      while(temp!=NULL) {
        if(flag!=temp->data.value.tree->children->size) {
          flag = -1;
          break;
        }
        temp = temp->next;
      }
      if(flag>10 || tree->children->size>10) {
        raiseLargeMatrix(tree);
        tree->attr[1] = errorType;
      } else if(flag!=-1) {
        tree->attr[1] = createType(MATRIX, tree->children->size, flag);
      } else {
        raiseNonRectangularMatrix(tree);
        tree->attr[1] = errorType;
      }
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
        temptree = extractChildNumber(tree, tree->children->size);
        if(temptree->attr[1] == errorType) {
          break;
        }
        temptree = fetchfunDefn(scope, temptree);
        if(temptree!=NULL) {
          temp2 = extractChild(temptree, "<parameterList>", 0, 1)->children->first;
          while(temp->next!=NULL && temp2!=NULL) {
            type1 = fetchType(scope, temp->data.value.tree);
            type2 = fetchType(fetchfunScope(scope, extractChildNumber(tree, tree->children->size)),
                              extractChildNumber(temp2->data.value.tree, 1));
            if(type1==NULL) {
              raiseNotDeclaredException(temp->data.value.tree);
            } else if(!typeComparator(scope, temp->data.value.tree, type1, type2)) {
              raiseTypeMismatchError(temp->data.value.tree, type2, type1, "");
            }
            temp = temp->next;
            temp2 = temp2->next;
          }
          if(temp->next!=NULL) {
            raiseInsufficientRHS(tree);
            markDefinedVars(scope, tree->children);
            break;
          } else if(temp2!=NULL) {
            raiseInsufficientLHS(tree);
            markDefinedVars(scope, tree->children);
            break;
          }
        } else {
          type1 = (Type*) extractChildNumber(tree, tree->children->size)->attr[1];
          if(type1==NULL) {
            raiseUntypedError(extractChildNumber(tree, tree->children->size));
            break;
          }
          if(type1->columns==2 && type1->type==INT) {
            if(tree->children->size>3) {
              raiseInsufficientRHS(tree);
              markDefinedVars(scope, tree->children);
              break;
            } else if(tree->children->size<3) {
              raiseInsufficientLHS(tree);
              markDefinedVars(scope, tree->children);
              break;
            } else {
              while(temp->next!=NULL) {
                type2 = fetchType(scope, temp->data.value.tree);
                if(type2==NULL) {
                  raiseNotDeclaredException(temp->data.value.tree);
                } else if(type2->type!=INT) {
                  raiseTypeMismatchError(temp->data.value.tree, NULL, type2, "INT");
                }
                temp = temp->next;
              }
            }
          } else {
            type2 = (Type*) temp->data.value.tree->attr[1];
            if(!typeComparator(scope, temp->data.value.tree, type2, type1)) {
              raiseTypeMismatchError(temp->data.value.tree, type1, type2, "");
            }
          }
        }
        markDefinedVars(scope, tree->children);
        break;
      case FUNID:
        temp = tree->children->first;
        temptree = fetchfunDefn(scope, tree);
        if(temptree==NULL) {
          break;
        }
        temp2 = extractChild(temptree, "<parameterList>", 0, 2)->children->first;
        while(temp!=NULL && temp2!=NULL) {
          type1 = (Type*) temp->data.value.tree->attr[1];
          type2 = fetchType(fetchfunScope(scope, tree),
                            extractChildNumber(temp2->data.value.tree, 1));
          if(type1==NULL) {

          } else if(!typeComparator(scope, extractChildNumber(temp2->data.value.tree, 1), type2, type1)) {
            raiseTypeMismatchError(extractChildNumber(temp2->data.value.tree, 1), type2, type1, "");
          }
          temp = temp->next;
          temp2 = temp2->next;
        }
        if(temp!=NULL || temp2!=NULL) {
          raiseBadFunctionCall(tree);
        } else if(checkStmt(tree) && extractChild(temptree, "<parameterList>", 0, 2)->children->size!=0) {
          raiseNonZeroRetFunction(tree);
        }
        break;
      case IF:
        type1 = (Type*) extractChildNumber(tree, 1)->attr[1];
        if(type1==NULL || type1->type!=AND) {
          raiseErronousIfCondn(tree);
        }
        break;
      case AND: case OR:
        temptree = extractChildNumber(tree, 1);
        type1 = ((Type*) extractChildNumber(tree, 2)->attr[1]);
        if(temptree->attr[1]==NULL) {
          raiseUntypedError(temptree);
          break;
        } else if(type1 == NULL) {
          raiseUntypedError(extractChildNumber(tree, 2));
          break;
        }
        if(((Type*) temptree->attr[1])->type == AND && type1->type == AND) {
          tree->attr[1] = temptree->attr[1];
        } else if (((Type*) temptree->attr[1])->type != AND){
          raiseTypeMismatchError(temptree, NULL, (Type*) temptree->attr[1], "BOOLEAN");
          tree->attr[1] = errorType;
        } else if(type1->type != AND) {
          raiseTypeMismatchError(extractChildNumber(tree, 2), NULL, type1, "BOOLEAN");
          tree->attr[1] = errorType;
        }
        break;
      case NOT:
        temptree = extractChildNumber(tree, 1);
        if(temptree->attr[1] == NULL) {
          raiseUntypedError(temptree);
          break;
        }
        if(((Type*) temptree->attr[1])->type == AND) {
          tree->attr[1] = temptree->attr[1];
        } else {
          raiseTypeMismatchError(temptree, NULL, (Type*) temptree->attr[1], "BOOLEAN");
          tree->attr[1] = errorType;
          break;
        }
        break;
      case LT: case LE: case EQ: case GT: case GE: case NE:
        type1 = (Type*) extractChildNumber(tree, 1)->attr[1];
        type2 = (Type*) extractChildNumber(tree, 2)->attr[1];
        if(type1==NULL) {
          raiseUntypedError(extractChildNumber(tree, 1));
          break;
        } else if(type2==NULL) {
          raiseUntypedError(extractChildNumber(tree, 2));
          break;
        }
        if(
            (type1->type == INT || type1->type == REAL) && type1->rows == 0 && type1->columns == 0 &&
            (type2->type == INT || type2->type == REAL) && type2->rows == 0 && type2->columns == 0
          ) {
          tree->attr[1] = createType(AND, 0, 0);
        } else {
          tree->attr[1] = errorType;
        }
        break;
      case PLUS:
        type1 = (Type*) extractChildNumber(tree, 1)->attr[1];
        type2 = (Type*) extractChildNumber(tree, 2)->attr[1];
        if(type1==NULL) {
          raiseUntypedError(extractChildNumber(tree, 1));
          break;
        } else if(type2==NULL) {
          raiseUntypedError(extractChildNumber(tree, 2));
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
            raiseTypeMismatchError(tree, type2, type1, "");
            tree->attr[1] = errorType;
          }
        } else if (
              (type1->type == STRING) &&
              (type2->type == STRING)
          ) {
          tree->attr[1] = createType(STRING, 0, type1->columns + type2->columns);
        } else {
          raiseInvalidOperatorError(tree, type2, type1);
          tree->attr[1] = errorType;
          break;
        }
        break;
      case MINUS:
        type1 = (Type*) extractChildNumber(tree, 1)->attr[1];
        type2 = (Type*) extractChildNumber(tree, 2)->attr[1];
        if(type1==NULL) {
          raiseUntypedError(extractChildNumber(tree, 1));
          break;
        } else if(type2==NULL) {
          raiseUntypedError(extractChildNumber(tree, 2));
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
            raiseInvalidOperatorError(tree, type2, type1);
            tree->attr[1] = errorType;
          }
        }
      break;
      case MUL:
        type1 = (Type*) extractChildNumber(tree, 1)->attr[1];
        type2 = (Type*) extractChildNumber(tree, 2)->attr[1];
        if(type1==NULL) {
          raiseUntypedError(extractChildNumber(tree, 1));
          break;
        } else if(type2==NULL) {
          raiseUntypedError(extractChildNumber(tree, 2));
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
          raiseInvalidOperatorError(tree, type2, type1);
          tree->attr[1] = errorType;
          break;
        }
        break;
      case DIV:
        type1 = (Type*) extractChildNumber(tree, 1)->attr[1];
        type2 = (Type*) extractChildNumber(tree, 2)->attr[1];
        if(type1==NULL) {
          raiseUntypedError(extractChildNumber(tree, 1));
          break;
        } else if(type2==NULL) {
          raiseUntypedError(extractChildNumber(tree, 2));
          break;
        }
        if(
            (type1->type == INT || type1->type == REAL) && type1->rows == 0 && type1->columns == 0 &&
            (type2->type == INT || type2->type == REAL) && type2->rows == 0 && type2->columns == 0
          ) {
          tree->attr[1] = createType(REAL, 0, 0);
        } else {
          raiseInvalidOperatorError(tree, type2, type1);
          tree->attr[1] = errorType;
          break;
        }
        break;
      case READ:
        type1 = (Type*) extractChildNumber(tree, 1)->attr[1];
        if(type1->type==STRING || type1->type==MATRIX) {
          raiseInvalidRead(tree, type1);
        }
      break;
    }
  } else {
    if(symbolComparatorNT(symbol, "<functionDefn>")) {
      temp = extractChild(tree, "<parameterList>", 0, 1)->children->first;
      while(temp!=NULL) {
        if(!fetchDefined(scope, extractChildNumber(temp->data.value.tree, 1))) {
          raiseUnAssignedRet(extractChildNumber(temp->data.value.tree, 1), extractChild(tree, "", FUNID, 1));
          break;
        }
        temp = temp->next;
      }
    }
  }
}

int typeCheck(Tree tree, int treeSize) {
  Element temp;
  treeSize++;
  if(tree->children->size!=0) {
    visitInhType(tree);
  	temp = tree->children->first;
  	while(temp!=NULL) {
  		treeSize = typeCheck(temp->data.value.tree, treeSize);
  		temp = temp->next;
  	}
    visitSynType(tree);
  } else {
    generateTypeAttribute(tree);
  }
  return treeSize;
}

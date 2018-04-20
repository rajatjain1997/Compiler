/**
 *	AUTHOR: Rajat Jain
 *  ID No. 2015A7PS0549P
 */

#include<stdio.h>
#include<stdlib.h>
#include"tree.h"
#include"token.h"
#include"symbol.h"
#include"quadruple.h"
#include"symboltable.h"
#include"semantic.h"
#include"list.h"

/**
 * void generateLabelAttribute(Tree tree): Generates addresses from constants and variables at leaves of AST.
 */
void generateLabelAttribute(Tree tree) {
  Symbol* symbol = extractSymbol(tree);
  SymbolTable scope = (SymbolTable) tree->parent->attr[0];
  Tree temptree; Type type1;
  tree->attr[0] = createList();
  if(isTerminal(symbol)) {
    switch(symbol->symbolType) {
        case ID:
          tree->attr[1] = makeAddress(retrieveSymbol(scope, getToken(symbol)), 0, 0, 0);
          break;
        case NUM:
          tree->attr[1] = makeAddress(NULL, getToken(symbol)->value.integer, 0, INT);
          break;
        case RNUM:
          tree->attr[1] = makeAddress(NULL, 0, getToken(symbol)->value.real, REAL);
          break;
        case STR:
          tree->attr[1] = makeAddress(getToken(symbol)->value.string, 0, 0, STRING);
          break;
    }
  }
}

/**
 *void visitInhCode(Tree tree): Responsible for propagating labels down a boolean expression.
 */
void visitInhCode(Tree tree) {
  Symbol* symbol = extractSymbol(tree);
  Tree temptree; Type type1;
  if(isTerminal(symbol)) {
    switch(symbol->symbolType) {
      case IF:
        tree->attr[1] = generateLabel();
        tree->attr[2] = generateLabel();
        temptree = extractChildNumber(tree, 1);
        temptree->attr[1] = tree->attr[1];
        temptree->attr[2] = tree->attr[2];
      break;
      case AND:
        temptree = extractChildNumber(tree, 1);
        temptree->attr[1] = generateLabel();
        temptree->attr[2] = tree->attr[2];
        temptree = extractChildNumber(tree, 2);
        temptree->attr[1] = tree->attr[1];
        temptree->attr[2] = tree->attr[2];
      break;
      case OR:
        temptree = extractChildNumber(tree, 1);
        temptree->attr[1] = tree->attr[1];
        temptree->attr[2] = generateLabel();
        temptree = extractChildNumber(tree, 2);
        temptree->attr[1] = tree->attr[1];
        temptree->attr[2] = tree->attr[2];
      break;
      case NOT:
        temptree = extractChildNumber(tree, 1);
        temptree->attr[1] = tree->attr[2];
        temptree->attr[2] = tree->attr[1];
      break;
    }
  }
}

/**
 * void visitSynCode(Tree tree): Computes Intermediate Code Quadruples post-order.
 */
void visitSynCode(Tree tree) {
  Symbol* symbol = extractSymbol(tree);
  SymbolTable scope = (SymbolTable) tree->attr[0];
  Quadruple newCode; List codelist; Address* tempAddress1, *tempAddress2;
  Type* temptype;
  Element temp, temp2;
  Tree temptree;
  static Type* type = NULL;
  codelist = createList();
  tree->attr[0] = codelist;
  if(type==NULL) {
    type = createType(INT, 0, 0);
  }
  int flag = 1;
  if(isTerminal(symbol)) {
    switch(symbol->symbolType) {
      case MAIN:
        temp = tree->children->first;
        while(temp!=NULL) {
          tree->attr[0] = appendLists(codelist, temp->data.value.tree->attr[0]);
          codelist = tree->attr[0];
          temp = temp->next;
        }
      break;
      case INT: case REAL: case STRING: case MATRIX:
        temp = tree->children->first;
        while(temp!=NULL) {
          insertAtEndFast(codelist,
            makeCode(OP_PUSH,
            temp->data.value.tree->attr[1],
            NULL,
            NULL
          ));
          temp = temp->next;
        }
        break;
      case ASSIGNOP:
        tree->attr[0] = appendLists(codelist, extractChildNumber(tree, tree->children->size)->attr[0]);
        codelist = tree->attr[0];
        if(tree->children->size==2) {
          insertAtEndFast(codelist,
            makeCode(OP_MOV,
              extractChildNumber(tree, 1)->attr[1],
              extractChildNumber(tree, 2)->attr[1],
              NULL
            ));
        } else {
          temp = tree->children->first;
          temp2 = ((List) extractChildNumber(tree, tree->children->size)->attr[1])->first;
          while(temp2!=NULL) {
            insertAtEndFast(codelist,
              makeCode(OP_MOV,
                temp->data.value.tree->attr[1],
                temp2->data.value.address,
                NULL));
            temp = temp->next;
            temp2 = temp2->next;
          }
        }
        break;
      case PLUS:
        tree->attr[1] = generateTemporary(scope, tree->attr[1]);
        insertAtEndFast(codelist,
          makeCode(OP_PLUS,
            extractChildNumber(tree, 1)->attr[1],
            extractChildNumber(tree, 2)->attr[1],
            tree->attr[1]
          ));
        tree->attr[0] = appendLists(extractChildNumber(tree, 2)->attr[0], codelist);
        tree->attr[0] = appendLists(extractChildNumber(tree, 1)->attr[0], tree->attr[0]);
      break;
      case MINUS:
        tree->attr[1] = generateTemporary(scope, tree->attr[1]);
        insertAtEndFast(codelist,
          makeCode(OP_MINUS,
            extractChildNumber(tree, 1)->attr[1],
            extractChildNumber(tree, 2)->attr[1],
            tree->attr[1]
          ));
        tree->attr[0] = appendLists(extractChildNumber(tree, 2)->attr[0], codelist);
        tree->attr[0] = appendLists(extractChildNumber(tree, 1)->attr[0], tree->attr[0]);
      break;
      case MUL:
        tree->attr[1] = generateTemporary(scope, tree->attr[1]);
        insertAtEndFast(codelist,
          makeCode(OP_MUL,
            extractChildNumber(tree, 1)->attr[1],
            extractChildNumber(tree, 2)->attr[1],
            tree->attr[1]
          ));
        tree->attr[0] = appendLists(extractChildNumber(tree, 2)->attr[0], codelist);
        tree->attr[0] = appendLists(extractChildNumber(tree, 1)->attr[0], tree->attr[0]);
      break;
      case DIV:
        tree->attr[1] = generateTemporary(scope, tree->attr[1]);
        insertAtEndFast(codelist,
          makeCode(OP_DIV,
            extractChildNumber(tree, 1)->attr[1],
            extractChildNumber(tree, 2)->attr[1],
            tree->attr[1]
          ));
        tree->attr[0] = appendLists(extractChildNumber(tree, 2)->attr[0], codelist);
        tree->attr[0] = appendLists(extractChildNumber(tree, 1)->attr[0], tree->attr[0]);
      break;
      case LT:
        insertAtEndFast(codelist,
          makeCode(OP_CMP,
            extractChildNumber(tree, 1)->attr[1],
            extractChildNumber(tree, 2)->attr[1],
            NULL
          ));
        insertAtEndFast(codelist,
          makeCode(OP_JLT,
            tree->attr[1],
            NULL,
            NULL
          ));
        insertAtEndFast(codelist,
          makeCode(OP_JMP,
            tree->attr[2],
            NULL,
            NULL));
        tree->attr[0] = appendLists(extractChildNumber(tree, 2)->attr[0], codelist);
        tree->attr[0] = appendLists(extractChildNumber(tree, 1)->attr[0], tree->attr[0]);
      break;
      case LE:
        insertAtEndFast(codelist,
          makeCode(OP_CMP,
            extractChildNumber(tree, 1)->attr[1],
            extractChildNumber(tree, 2)->attr[1],
            NULL
          ));
        insertAtEndFast(codelist,
          makeCode(OP_JLE,
            tree->attr[1],
            NULL,
            NULL
          ));
        insertAtEndFast(codelist,
          makeCode(OP_JMP,
            tree->attr[2],
            NULL,
            NULL));
        tree->attr[0] = appendLists(extractChildNumber(tree, 2)->attr[0], codelist);
        tree->attr[0] = appendLists(extractChildNumber(tree, 1)->attr[0], tree->attr[0]);
      break;
      case EQ:
        insertAtEndFast(codelist,
          makeCode(OP_CMP,
            extractChildNumber(tree, 1)->attr[1],
            extractChildNumber(tree, 2)->attr[1],
            NULL
          ));
        insertAtEndFast(codelist,
          makeCode(OP_JEQ,
            tree->attr[1],
            NULL,
            NULL
          ));
        insertAtEndFast(codelist,
          makeCode(OP_JMP,
            tree->attr[2],
            NULL,
            NULL));
        tree->attr[0] = appendLists(extractChildNumber(tree, 2)->attr[0], codelist);
        tree->attr[0] = appendLists(extractChildNumber(tree, 1)->attr[0], tree->attr[0]);
      break;
      case GT:
        insertAtEndFast(codelist,
          makeCode(OP_CMP,
            extractChildNumber(tree, 1)->attr[1],
            extractChildNumber(tree, 2)->attr[1],
            NULL
          ));
        insertAtEndFast(codelist,
          makeCode(OP_JGT,
            tree->attr[1],
            NULL,
            NULL
          ));
        insertAtEndFast(codelist,
          makeCode(OP_JMP,
            tree->attr[2],
            NULL,
            NULL));
        tree->attr[0] = appendLists(extractChildNumber(tree, 2)->attr[0], codelist);
        tree->attr[0] = appendLists(extractChildNumber(tree, 1)->attr[0], tree->attr[0]);
      break;
      case GE:
        insertAtEndFast(codelist,
          makeCode(OP_CMP,
            extractChildNumber(tree, 1)->attr[1],
            extractChildNumber(tree, 2)->attr[1],
            NULL
          ));
        insertAtEndFast(codelist,
          makeCode(OP_JGE,
            tree->attr[1],
            NULL,
            NULL
          ));
        insertAtEndFast(codelist,
          makeCode(OP_JMP,
            tree->attr[2],
            NULL,
            NULL));
        tree->attr[0] = appendLists(extractChildNumber(tree, 2)->attr[0], codelist);
        tree->attr[0] = appendLists(extractChildNumber(tree, 1)->attr[0], tree->attr[0]);
      break;
      case NE:
        insertAtEndFast(codelist,
          makeCode(OP_CMP,
            extractChildNumber(tree, 1)->attr[1],
            extractChildNumber(tree, 2)->attr[1],
            NULL
          ));
        insertAtEndFast(codelist,
          makeCode(OP_JNE,
            tree->attr[1],
            NULL,
            NULL
          ));
        insertAtEndFast(codelist,
          makeCode(OP_JMP,
            tree->attr[2],
            NULL,
            NULL));
        tree->attr[0] = appendLists(extractChildNumber(tree, 2)->attr[0], codelist);
        tree->attr[0] = appendLists(extractChildNumber(tree, 1)->attr[0], tree->attr[0]);
      break;
      case AND:
        tree->attr[0] = appendLists(tree->attr[0], extractChildNumber(tree, 1)->attr[0]);
        insertAtEndFast(tree->attr[0],
          makeCode(OP_LABEL,
            extractChildNumber(tree, 1)->attr[1],
            NULL,
            NULL));
        tree->attr[0] = appendLists(tree->attr[0], extractChildNumber(tree, 2)->attr[0]);
      break;
      case OR:
        tree->attr[0] = appendLists(tree->attr[0], extractChildNumber(tree, 1)->attr[0]);
        insertAtEndFast(tree->attr[0],
          makeCode(OP_LABEL,
            extractChildNumber(tree, 1)->attr[2],
            NULL,
            NULL));
        tree->attr[0] = appendLists(tree->attr[0], extractChildNumber(tree, 2)->attr[0]);
      break;
      case NOT:
        tree->attr[0] = appendLists(tree->attr[0], extractChildNumber(tree, 1)->attr[0]);
      break;
      case IF:
        tree->attr[0] = appendLists(tree->attr[0], extractChildNumber(tree, 1)->attr[0]);
        insertAtEndFast(tree->attr[0],
          makeCode(OP_LABEL,
            tree->attr[1],
            NULL,
            NULL));
        temp = tree->children->first->next;
        codelist = tree->attr[0];
        while(temp->next!=NULL) {
          tree->attr[0] = appendLists(codelist, temp->data.value.tree->attr[0]);
          codelist = tree->attr[0];
          temp = temp->next;
        }
        if(extractSymbol(temp->data.value.tree)->symbolType == ELSE) {
          tempAddress1 = generateLabel();
          insertAtEndFast(codelist, makeCode(OP_JMP, tempAddress1, NULL, NULL));
          insertAtEndFast(codelist, makeCode(OP_LABEL, tree->attr[2], NULL, NULL));
          tree->attr[0] = appendLists(tree->attr[0], temp->data.value.tree->attr[0]);
          insertAtEndFast(tree->attr[0], makeCode(OP_LABEL, tempAddress1, NULL, NULL));
        } else {
          insertAtEndFast(codelist, makeCode(OP_LABEL, tree->attr[2], NULL, NULL));
        }
        break;
      case ELSE:
        temp = tree->children->first;
        while(temp!=NULL) {
          tree->attr[0] = appendLists(codelist, temp->data.value.tree->attr[0]);
          codelist = tree->attr[0];
          temp = temp->next;
        }
        break;
      case ID:
        tree->attr[1] = generateTemporary(scope, type);
        tempAddress1 = generateTemporary(scope, type);
        insertAtEndFast(codelist,
          makeCode(OP_MUL,
            makeAddress(NULL, retrieveSymbol(scope, getToken(symbol))->value.identry->type->rows, 0, INT),
            makeAddress(NULL, getToken(extractSymbol(extractChildNumber(tree, 2)))->value.integer, 0, INT),
            tempAddress1));
        tempAddress2 = generateTemporary(scope, type);
        insertAtEndFast(codelist,
          makeCode(OP_PLUS,
            tempAddress1,
            makeAddress(NULL, getToken(extractSymbol(extractChildNumber(tree, 1)))->value.integer, 0, INT),
            tempAddress2));
        tempAddress1 = generateTemporary(scope, type);
        insertAtEndFast(codelist,
          makeCode(OP_MUL,
            tempAddress2,
            makeAddress(NULL, sizeLookup(INT), 0, INT),
            tempAddress1));
        insertAtEndFast(codelist,
        makeCode(OP_ADDRPLUS,
          tempAddress1,
          makeAddress(retrieveSymbol(scope, getToken(symbol)), 0, 0, 0),
          tree->attr[1]));
      break;
      case SIZE:
        temptype = fetchType(scope, extractChildNumber(tree, 1));
        if(temptype->type==INT || temptype->type==REAL) {
          tree->attr[1] = makeAddress(NULL, sizeLookup(temptype->type), 0, INT);
        } else if(temptype->type==STRING) {
          tree->attr[1] = makeAddress(NULL, temptype->columns, 0, INT);
        } else {
          tree->attr[1] = createList();
          insertAtEndFast(tree->attr[1], makeAddress(NULL, temptype->rows, 0, INT));
          insertAtEndFast(tree->attr[1], makeAddress(NULL, temptype->columns, 0, INT));
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
        if(extractChildNumber(temptree, 2)->children->size==1) {
          tree->attr[1] = generateTemporary(scope, tree->attr[1]);
          insertAtEndFast(codelist,
            makeCode(OP_PUSH,
              tree->attr[1],
              NULL,
              NULL));
        } else {
          tree->attr[1] = createList();
          temp = extractChildNumber(temptree, 2)->children->first;
          while(temp!=NULL) {
            tempAddress1 = generateTemporary(scope, fetchType(fetchfunScope(scope, tree),
                                              extractChildNumber(temp->data.value.tree, 1)));
            insertAtEndFast(tree->attr[1], tempAddress1);
            insertAtEndFast(codelist,
              makeCode(OP_PUSH,
                tempAddress1,
                NULL,
                NULL));
            temp = temp->next;
          }
        }
        insertAtEndFast(codelist,
          makeCode(OP_CALL,
            makeAddress(extractChildNumber(temptree, 1), 0, 0, FUNID),
            makeAddress(NULL, tree->children->size, 0, INT),
            makeAddress(NULL, extractChildNumber(temptree, 2)->children->size, 0, INT)));
        break;
      case READ:
        insertAtEndFast(codelist, makeCode(OP_READ, extractChildNumber(tree, 1)->attr[1], NULL, NULL));
      break;
      case PRINT:
        insertAtEndFast(codelist, makeCode(OP_PRINT, extractChildNumber(tree, 1)->attr[1], NULL, NULL));
      break;
    }
  } else {
    if(symbolComparatorNT(symbol, "<matrix>")) {
      tree->attr[1] = makeAddress(tree, 0, 0, MATRIX);
    } else if(symbolComparatorNT(symbol, "<functionDefn>")) {
      temp = tree->children->first;
      insertAtEndFast(codelist,
        makeCode(OP_DEFINE,
          makeAddress(extractChildNumber(tree, 1), 0, 0, FUNID),
          NULL,
          NULL));
      temp = temp->next;
      while(temp!=NULL) {
        tree->attr[0] = appendLists(codelist, temp->data.value.tree->attr[0]);
        codelist = tree->attr[0];
        temp = temp->next;
      }
      insertAtEndFast(codelist,
        makeCode(OP_RET,
          makeAddress(NULL, extractChildNumber(tree, 2)->children->size, 0, INT),
          NULL,
          NULL));
    } else if(symbolComparatorNT(symbol, "<parameterList>")) {
      temp = tree->children->first;
      while(temp!=NULL) {
        tree->attr[0] = appendLists(codelist, temp->data.value.tree->attr[0]);
        codelist = tree->attr[0];
        temp = temp->next;
      }
    }
  }
}

/**
 *List generateIntermediateCode(Tree tree): Driver function for generating intermediate code.
 */
List generateIntermediateCode(Tree tree) {
  Element temp;
  if(tree->children->size!=0) {
      visitInhCode(tree);
  	temp = tree->children->first;
  	while(temp!=NULL) {
  		generateIntermediateCode(temp->data.value.tree);
  		temp = temp->next;
  	}
    visitSynCode(tree);
  } else {
    generateLabelAttribute(tree);
  }
  return tree->attr[0];
}

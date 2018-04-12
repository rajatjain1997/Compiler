#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"quadruple.h"
#include"token.h"
#include"symbol.h"
#include"tree.h"
#include"symboltable.h"

Quadruple* makeCode(Operator op, void* op1, void* op2, void* op3, int flags) {
  Quadruple* quad = (Quadruple*) malloc(sizeof(Quadruple));
  int i = 0;
  for(;i<3;i++) {
    quad->op[i] = (struct address*) malloc(sizeof(struct address));
  }
  quad->op[0]->address = op1;
  quad->op[0]->isConstant = CONSTANT_OP1 & flags;
  quad->op[1]->address = op2;
  quad->op[1]->isConstant = CONSTANT_OP2 & flags;
  quad->op[2]->address = op3;
  quad->op[3]->isConstant = CONSTANT_OP3 & flags;
  return quad;
}

struct symbolTableEntry* generateTemporary(SymbolTable st, Type type) {
  static char* tempstring = NULL; int number;
  if(tempstring==NULL) {
    tempstring = (char*) malloc(sizeof(char)*20);
    strcpy(tempstring, "a01");
  } else {
    number = atoi(tempstring+2);
    number++;
    if(number>500) {
      sprintf(tempstring, "%c01", tempstring[0]+1);
    } else {
      sprintf(tempstring, "%c0%d", tempstring[0], number);
    }
  }
  Token* to = tokenize(ID, buf, 0);
  Symbol* sy = generateSymbol(ID, 1);
  attachTokenToSymbol(sy, to);
  Tree t = createTree(sy);
  createidEntry(st, t, type->type);
  if(!(rows < 1 && columns < 1)) {
    updateidEntrySize(st, t, type->type, type->rows, type->columns);
  }
  updateidDefined(st, tree);
  tree->attr[0] = st;
  tree->attr[1] = type;
  return retrieveSymbol(st, to);
}

char* generateLabel() {
  static char* tempstring = NULL; int number;
  if(tempstring==NULL) {
    tempstring = (char*) malloc(sizeof(char)*20);
    strcpy(tempstring, "la01");
  } else {
    number = atoi(tempstring+3);
    number++;
    if(number>500) {
      sprintf(tempstring, "l%c01", tempstring[0]+1);
    } else {
      sprintf(tempstring, "l%c0%d", tempstring[0], number);
    }
  }
  char* label = (char*) malloc(sizeof(char)*20);
  strcpy(label, tempstring);
  return label;
}

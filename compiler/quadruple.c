#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"quadruple.h"
#include"token.h"
#include"symbol.h"
#include"tree.h"
#include"symboltable.h"

Address* makeAddress(void* address, int isConstant) {
  Address* addr = (struct address*) malloc(sizeof(struct address));
  addr->address = address;
  addr->isConstant = isConstant;
}

Quadruple* makeCode(Operator op, Address* op1, Address* op2, Address* op3) {
  Quadruple* quad = (Quadruple*) malloc(sizeof(Quadruple));
  quad->op[0] = op1;
  quad->op[1] = op2;
  quad->op[2] = op3;
  return quad;
}

Address* generateTemporary(SymbolTable st, Type* type) {
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
  char buf[20];
  Token* to = tokenize(ID, buf, 0);
  Symbol* sy = generateSymbol(ID, 1);
  attachTokenToSymbol(sy, to);
  Tree t = createTree(sy);
  createidEntry(st, t, type->type);
  if(!(type->rows < 1 && type->columns < 1)) {
    updateidEntrySize(st, t, type->type, type->rows, type->columns);
  }
  updateidDefined(st, t);
  t->attr[0] = st;
  return makeAddress(retrieveSymbol(st, to), 0);
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

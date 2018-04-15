#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"
#include "token.h"
#include "symbol.h"
#include "tree.h"
#include "symboltable.h"

const int size = 29;

int hashingFunction(char a[]) {
  int m = 1;
  unsigned int index = 0, i = 0;
  while(a[i]!='\0') {
    index = (index + (a[i]*m)%size)%size;
    i++;
    m=(m*5)%size;
  }
  return index;
}

SymbolTable createSymbolTable(SymbolTable parent, Tree func) {
  SymbolTable h = (SymbolTable) malloc(sizeof(struct symboltable));
  h->symboltable = (List*) malloc(sizeof(List)* size);
  int i;
  for(i=0; i<size; i++) {
    h->symboltable[i] = createList();
  }
  h->size = size;
  h->lastoffset = 0;
  h->parent = parent;
  h->func = func;
  return h;
}

Type* createType(int t, int rows, int columns) {
  Type* type = (Type*) malloc(sizeof(Type));
  type->type = t;
  type->rows = rows;
  type->columns = columns;
  return type;
}

int checkRecursion(SymbolTable st, Tree tokentree) {
  SymbolTable temp = st;
  while(temp!=NULL) {
    if(temp->func==tokentree) {
      return 1;
    }
    temp = temp->parent;
  }
  return 0;
}

// void deleteSymbolTable(SymbolTable h) {
// 	int i;
// 	for(i=0;i<h->size;i++) {
// 		freeList(h->symboltable[i]);
// 	}
// 	free(h);
// }

int insertSymbol(SymbolTable h,  struct symbolTableEntry* s) {
  Data d;
  char buf1[20], buf2[20];
  getLexeme(getToken(extractSymbol(s->tokentree)), buf2);
	int index = hashingFunction(buf2);
  d.value.symboltableentry = s;
  int duplicate = 0;
  Element temp = h->symboltable[index]->first;
  while(temp!=NULL) {
    getLexeme(getToken(extractSymbol(temp->data.value.symboltableentry->tokentree)), buf1);
    if(strcmp(buf1,buf2)==0) {
      duplicate=1;
      break;
    }
    temp = temp->next;
  }
  if(!duplicate) {
    insertAtEnd(h->symboltable[index], d);
    return 1;
  } else {
    return 0;
  }
}

struct symbolTableEntry* retrieveSymbol(SymbolTable h, Token* token) {
  char buf1[20], buf2[20];
  if(token->type!=ID && token->type!=FUNID) {
    return NULL;
  }
  SymbolTable tempst = h;
  getLexeme(token, buf2);
  int index = hashingFunction(buf2);
  while(tempst!=NULL) {
    Element temp = tempst->symboltable[index]->first;
    while(temp!=NULL) {
      getLexeme(getToken(extractSymbol(temp->data.value.symboltableentry->tokentree)), buf1);
      if(strcmp(buf1, buf2)==0) {
        if(token->type==FUNID && checkRecursion(h, temp->data.value.symboltableentry->tokentree)) {
          return NULL;
        }
        return temp->data.value.symboltableentry;
      }
      temp = temp->next;
    }
    tempst = tempst->parent;
  }
  return NULL;
}

int createidEntry(SymbolTable st, Tree tokentree, int type) {
  if(!isTerminal(extractSymbol(tokentree)) || getToken(extractSymbol(tokentree))->type!=ID) {
    return 0;
  }
  struct symbolTableEntry* ste = (struct symbolTableEntry*) malloc(sizeof(struct symbolTableEntry));
  ste->value.identry = (struct idEntry*) malloc(sizeof(struct idEntry));
  ste->tokentree = tokentree;
  ste->value.identry->type = (Type*) malloc(sizeof(Type));
  ste->value.identry->type->type = type;
  ste->value.identry->type->rows = 0;
  ste->value.identry->type->columns = 0;
  int size = sizeLookup(type);
  ste->value.identry->size = size;
  ste->value.identry->defined = 0;
  ste->value.identry->offset = st->lastoffset;
  st->lastoffset = ste->value.identry->offset + size;
  return insertSymbol(st, ste);
}

int updateidEntrySize(SymbolTable st, Tree tokentree, int type, int rows, int columns) {
  if(!isTerminal(extractSymbol(tokentree)) || getToken(extractSymbol(tokentree))->type!=ID) {
    return 0;
  }
  struct symbolTableEntry* ste = retrieveSymbol(st, getToken(extractSymbol(tokentree)));
  if(ste==NULL || ste->value.identry->size!=0 || (rows < 1 && columns < 1) || ste->value.identry->type->type!=type) {
    return 0;
  }
  if(type==MATRIX) {
    type = INT;
  } else {
    type = STR;
  }
  int size = sizeLookup(type)*rows*columns;
  ste->value.identry->size = size;
  ste->value.identry->offset = st->lastoffset + size;
  st->lastoffset = ste->value.identry->offset;
  ste->value.identry->type->rows = rows;
  ste->value.identry->type->columns = columns;
  return 1;
}

int updateidDefined(SymbolTable st, Tree tokentree) {
  if(!isTerminal(extractSymbol(tokentree)) || getToken(extractSymbol(tokentree))->type!=ID) {
    return 0;
  }
  struct symbolTableEntry* ste = retrieveSymbol(st, getToken(extractSymbol(tokentree)));
  if(ste==NULL) {
    return 0;
  }
  ste->value.identry->defined = (char) 1;
  return 1;
}

Type* fetchType(SymbolTable st, Tree tokentree) {
  if(!isTerminal(extractSymbol(tokentree)) || getToken(extractSymbol(tokentree))->type!=ID) {
    return 0;
  }
  struct symbolTableEntry* ste = retrieveSymbol(st, getToken(extractSymbol(tokentree)));
  if(ste==NULL) {
    return 0;
  }
  return ste->value.identry->type;
}

int fetchDefined(SymbolTable st, Tree tokentree) {
  if(!isTerminal(extractSymbol(tokentree)) || getToken(extractSymbol(tokentree))->type!=ID) {
    return 0;
  }
  struct symbolTableEntry* ste = retrieveSymbol(st, getToken(extractSymbol(tokentree)));
  if(ste==NULL) {
    return 0;
  }
  return (int) ste->value.identry->defined;
}

SymbolTable createfunEntry(SymbolTable st, Tree tokentree) {
  if(!isTerminal(extractSymbol(tokentree)) || getToken(extractSymbol(tokentree))->type!=FUNID) {
    return NULL;
  }
  struct symbolTableEntry* ste = (struct symbolTableEntry*) malloc(sizeof(struct symbolTableEntry));
  ste->value.funentry = (struct funEntry*) malloc(sizeof(struct funEntry));
  ste->tokentree = tokentree;
  if(insertSymbol(st, ste) == 0) {
    return NULL;
  }
  ste->value.funentry->scope = createSymbolTable(st, tokentree);
  return ste->value.funentry->scope;
}

SymbolTable getParentScope(SymbolTable st) {
  return st->parent;
}

Tree fetchfunDefn(SymbolTable st, Tree tokentree) {
  if(!isTerminal(extractSymbol(tokentree)) || getToken(extractSymbol(tokentree))->type!=FUNID) {
    return NULL;
  }
  struct symbolTableEntry* ste = retrieveSymbol(st, getToken(extractSymbol(tokentree)));
  if(ste==NULL) {
    return NULL;
  }
  return ste->tokentree->parent;
}

SymbolTable fetchfunScope(SymbolTable st, Tree tokentree) {
  if(!isTerminal(extractSymbol(tokentree)) || getToken(extractSymbol(tokentree))->type!=FUNID) {
    return NULL;
  }
  struct symbolTableEntry* ste = retrieveSymbol(st, getToken(extractSymbol(tokentree)));
  if(ste==NULL) {
    return NULL;
  }
  return ste->value.funentry->scope;
}

/* GDB Only functions */

// void main() {
//   char buf[] = "abc";
//   SymbolTable st = createSymbolTable();
//   initializeTokenizer();
//   Token* to = tokenize(ID, buf, 0);
//   Symbol* sy = generateSymbol(ID, 1);
//   attachTokenToSymbol(sy, to);
//   Tree t = createTree(sy);
//   printf("%d\n", createidEntry(st, t, MATRIX));
//   printf("%d\n", fetchType(st, t)->type);
//   printf("%d\n", updateidEntrySize(st, t, MATRIX, 5,5));
//   printf("%d\n", fetchType(st, t)->rows);
//   char buf2[] = "_abc";
//   to = tokenize(FUNID, buf, 0);
//   sy = generateSymbol(FUNID, 1);
//   attachTokenToSymbol(sy, to);
//   Tree tr = createTree(sy);
//   st = createfunEntry(st, tr);
//   printf("%d\n", fetchType(st, t)->type);
//   printf("%d\n", updateidEntrySize(st, t, MATRIX, 5,5));
//   printf("%d\n", fetchType(st, t)->rows);
//   printf("%d %d\n", st->lastoffset, st->parent->lastoffset);
// }
//
// int sizeLookup(int type) {
//   switch(type) {
//     case INT: return 4;
//     case REAL: return 4;
//     case STRING: case MATRIX: return 0;
//     case STR: return 1;
//   }
// }

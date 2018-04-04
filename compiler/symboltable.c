#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"
#include "token.h"
#include "symbol.h"
#include "tree.h"
#include "symboltable.h"

const int size = 29;

struct idEntry {
  Type* type;
  int size;
  int offset;
};

struct funEntry {
  SymbolTable scope;
};

union symbolTableEntryElements {
  struct idEntry* identry;
  struct funEntry* funentry;
};

struct symbolTableEntry {
  Tree tokentree;
  union symbolTableEntryElements value;
};

int hashingFunction(char a[]) {
  long long int m = 1;
  int index = 0, i = 0;
  while(a[i]!='\0') {
    index = (index + (a[i]*m)%size)%size;
    m*=37;
  }
}

SymbolTable createSymbolTable() {
  SymbolTable h = (SymbolTable) malloc(sizeof(struct symboltable));
  h->symboltable = (List*) malloc(sizeof(List)* size);
  int i;
  for(i=0; i<size; i++) {
    h->symboltable[i] = createList();
  }
  h->size = size;
  h->lastoffset = 0;
  h->parent = NULL;
  return h;
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
  SymbolTable tempst = h;
  getLexeme(token, buf2);
  int index = hashingFunction(buf2);
  while(tempst!=NULL)
    Element temp = tempst->symboltable[index]->first;
    while(temp!=NULL) {
      getLexeme(getToken(extractSymbol(temp->data.value.symboltableentry->tokentree)), buf1);
      if(strcmp(buf1, buf2)==0) {
        return temp->data.value.symboltableentry;
      }
      temp = temp->next;
    }
    tempst = tempst->parent;
  }
  return NULL;
}

int createidEntry(SymbolTable st, Tree tokentree, int type) {
  if(getToken(extractSymbol(tokentree))->type!=ID) {
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
  ste->value.identry->offset = st->lastoffset + size;
  st->lastoffset = ste->value.identry->offset;
  return insertSymbol(st, ste);
}

int updateidEntrySize(SymbolTable st, Tree tokentree, int rows, int columns) {
  struct symbolTableEntry* ste = retrieveSymbol(st, getToken(extractSymbol(tokentree)));
  if(ste==NULL || ste->value.identry->size!=0 || rows < 1 || columns < 1) {
    return 0;
  }
  int size = rows*columns;
  ste->value.identry->size = size;
  ste->value.identry->offset = st->lastoffset + size;
  st->lastoffset = ste->value.identry->offset;
  ste->value.identry->type->rows = rows;
  ste->value.identry->type->columns = columns;
  return 1;
}

Type* fetchType(SymbolTable st, Tree tokentree) {
  struct symbolTableEntry* ste = retrieveSymbol(st, getToken(extractSymbol(tokentree)));
  if(ste==NULL || getToken(extractSymbol(ste->tokentree))->type!=ID) {
    return NULL;
  }
  return ste->value.identry->type;
}

SymbolTable createfunEntry(SymbolTable st, Tree tokentree) {
  if(getToken(extractSymbol(tokentree))->type!=FUNID) {
    return NULL;
  }
  struct symbolTableEntry* ste = (struct symbolTableEntry*) malloc(sizeof(struct symbolTableEntry));
  ste->value.funentry = (struct funEntry*) malloc(sizeof(struct funEntry));
  ste->tokentree = tokentree;
  ste->value.funentry->scope = createSymbolTable();
  ste->value.funentry->scope->parent = st;
  return ste->value.funentry->scope;
}

SymbolTable getParentScope(SymbolTable st) {
  return st->parent;
}

Tree fetchfunDefn(SymbolTable st, Tree tokentree) {
  if(getToken(extractSymbol(tokentree))->type!=FUNID) {
    return NULL;
  }
  struct symbolTableEntry* ste = retrieveSymbol(st, getToken(extractSymbol(tokentree)));
  if(ste==NULL) {
    return NULL;
  }
  return ste->tokentree;
}

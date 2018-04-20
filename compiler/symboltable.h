/**
 *	AUTHOR: Rajat Jain
 *  ID No. 2015A7PS0549P
 */

struct head;
struct tree;
struct token;

struct symboltable {
  struct head** symboltable;
  int size;
  int lastoffset;
  struct symboltable* parent;
  struct tree* func;
};

typedef struct symboltable* SymbolTable;

typedef struct type {
    int type;
    int rows;
    int columns;
} Type;


struct idEntry {
  Type* type;
  int size;
  int offset;
  char defined;
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

extern int sizeLookup(int type);

SymbolTable createSymbolTable(SymbolTable parent, struct tree* func);

Type* createType(int type, int rows, int columns);

int createidEntry(SymbolTable st, struct tree* tokentree, int type);

int updateidEntrySize(SymbolTable st, struct tree* tokentree, int type, int rows, int columns);

int updateidDefined(SymbolTable st, struct tree* tokentree);

Type* fetchType(SymbolTable st, struct tree* tokentree);

int fetchDefined(SymbolTable st, struct tree* tokentree);

SymbolTable createfunEntry(SymbolTable st, struct tree* tokentree);

SymbolTable getParentScope(SymbolTable st);

struct tree* fetchfunDefn(SymbolTable st, struct tree* tokentree);

SymbolTable fetchfunScope(SymbolTable st, struct tree* tokentree);

struct symbolTableEntry* retrieveSymbol(SymbolTable h, struct Token* token);

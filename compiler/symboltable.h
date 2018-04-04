struct head;
struct tree;

struct symboltable {
  struct head** symboltable;
  int size;
  int lastoffset;
};

typedef struct type {
    int type;
    int rows;
    int columns;
} Type;

typedef struct symboltable* SymbolTable;

extern int sizeLookup(int type);

SymbolTable createSymbolTable();

int createidEntry(SymbolTable st, Tree tokentree, int type);

int updateidEntrySize(SymbolTable st, Tree tokentree, int rows, int columns);

Type* fetchType(SymbolTable st, Tree tokentree);

SymbolTable createfunEntry(SymbolTable st, Tree tokentree);

// SymbolTable getParentScope(SymbolTable st);

Tree fetchfunDefn(SymbolTable st, Tree tokentree);

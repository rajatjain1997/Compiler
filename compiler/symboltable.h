struct head;
struct tree;

struct symboltable {
  struct head** symboltable;
  int size;
  int lastoffset;
  struct symboltable* parent;
};

typedef struct type {
    int type;
    int rows;
    int columns;
} Type;

typedef struct symboltable* SymbolTable;

extern int sizeLookup(int type);

SymbolTable createSymbolTable(SymbolTable parent);

Type* createType(int type, int rows, int columns);

int createidEntry(SymbolTable st, Tree tokentree, int type);

int updateidEntrySize(SymbolTable st, Tree tokentree, int type, int rows, int columns);

int updateidDefined(SymbolTable st, Tree tokentree);

Type* fetchType(SymbolTable st, Tree tokentree);

int fetchDefined(SymbolTable st, Tree tokentree);

SymbolTable createfunEntry(SymbolTable st, Tree tokentree);

SymbolTable getParentScope(SymbolTable st);

Tree fetchfunDefn(SymbolTable st, Tree tokentree);

SymbolTable fetchfunScope(SymbolTable st, Tree tokentree);

struct head;
struct tree;

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

int createidEntry(SymbolTable st, Tree tokentree, int type);

int updateidEntrySize(SymbolTable st, Tree tokentree, int type, int rows, int columns);

int updateidDefined(SymbolTable st, Tree tokentree);

Type* fetchType(SymbolTable st, Tree tokentree);

int fetchDefined(SymbolTable st, Tree tokentree);

SymbolTable createfunEntry(SymbolTable st, Tree tokentree);

SymbolTable getParentScope(SymbolTable st);

Tree fetchfunDefn(SymbolTable st, Tree tokentree);

SymbolTable fetchfunScope(SymbolTable st, Tree tokentree);

struct symbolTableEntry* retrieveSymbol(SymbolTable h, Token* token);

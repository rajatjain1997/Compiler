struct symbolTableEntry;

typedef enum {
  OP_PLUS,
  OP_MINUS,
  OP_DIV,
  OP_MUL,
  OP_LT,
	OP_LE,
	OP_EQ,
	OP_GT,
	OP_GE,
	OP_NE,
  OP_PARAM,
  OP_GOTO,
  OP_IF,
  OP_IFFALSE,
  OP_CALL
} Operator;

typedef struct address {
  void* address;
  int isConstant;
} Address;

typedef struct quadruple {
  Operator operator;
  struct address* op[3];
} Quadruple;

Address* makeAddress(void* address, int isConstant);

Quadruple* makeCode(Operator op, Address* op1, Address* op2, Address* op3);

struct symbolTableEntry* generateTemporary(SymbolTable st, Type type);

char* generateLabel();

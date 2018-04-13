struct symbolTableEntry;
struct symboltable;
struct type;

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
  OP_RETURNPARAM,
  OP_MOV,
  OP_GOTO,
  OP_IF,
  OP_IFFALSE,
  OP_CALL,
  OP_PUSH,
  OP_POP,
  OP_ADDR,
  OP_ADDRPLUS, //Add with second argument as address
  OP_DEFINE,
  OP_RET
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

Address* generateTemporary(struct symboltable* st, struct type* type);

char* generateLabel();

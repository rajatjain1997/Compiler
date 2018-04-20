/**
 *	AUTHOR: Rajat Jain
 *  ID No. 2015A7PS0549P
 */

struct symbolTableEntry;
struct symboltable;
struct type;

typedef enum {
  OP_PLUS,
  OP_MINUS,
  OP_DIV,
  OP_MUL,
  OP_JLT,
	OP_JLE,
	OP_JEQ,
	OP_JGT,
	OP_JGE,
	OP_JNE,
  OP_CMP,
  OP_JMP,
  OP_LABEL,
  OP_PARAM,
  OP_RETURNPARAM,
  OP_MOV,
  OP_CALL,
  OP_PUSH,
  OP_ADDRPLUS, //Add with second argument as address
  OP_DEFINE,
  OP_RET,
  OP_READ,
  OP_PRINT
} Operator;

union addresselem {
  void* entry;
  int integer;
  float real;
};

typedef struct address {
  union addresselem address;
  int type;
} Address;

typedef struct quadruple {
  Operator operator;
  struct address* op[3];
} Quadruple;

Address* makeAddress(void* entry, int integer, float real, int type);

Quadruple* makeCode(Operator op, Address* op1, Address* op2, Address* op3);

Address* generateTemporary(struct symboltable* st, struct type* type);

Address* generateLabel();

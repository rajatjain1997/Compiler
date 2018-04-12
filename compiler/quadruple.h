struct symbolTableEntry;

const unsigned int CONSTANT_OP1 = 1, CONSTANT_OP2 = 2, CONSTANT_OP3 = 4;

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

struct address {
  void* address;
  int isConstant;
};

typedef struct quadruple {
  Operator operator;
  struct address* op[3];
} Quadruple;

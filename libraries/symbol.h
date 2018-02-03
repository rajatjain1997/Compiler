struct Token;

typedef enum {
	T_ASSIGNOP,
	T_COMMENT,
	T_FUNID,
	T_ID,
	T_NUM,
	T_RNUM,
	T_STR,
	T_END,
	T_INT,
	T_REAL,
	T_STRING,
	T_MATRIX,
	T_MAIN,
	T_SQO,
	T_SQC,
	T_OP,
	T_CL,
	T_SEMICOLON,
	T_COMMA,
	T_IF,
	T_ELSE,
	T_ENDIF,
	T_READ,
	T_PRINT,
	T_FUNCTION,
	T_PLUS,
	T_MINUS,
	T_MUL,
	T_DIV,
	T_SIZE,
	T_AND,
	T_OR,
	T_NOT,
	T_LT,
	T_LE,
	T_EQ,
	T_GT,
	T_GE,
	T_NE,
	NT_MainFxn,
	NT_Prog,
	NT_StmtsAndFxnDefs,
	NT_StmtOrFxnDef,
	NT_Stmt,
	NT_FxnDef,
	NT_DeclStmt,
	NT_AssignStmt,
	NT_IfStmt,
	NT_IoStmt,
	NT_FxnCallStmt,
	NT_ParameterList,
	NT_Type,
	NT_RemainingList,
	NT_VarList,
	NT_LeftHandSide,
	NT_RightHandSide,
	NT_Matrix,
	NT_ArithmeticExpr,
	NT_FxnCall,
	NT_BooleanExpr,
	NT_StmtProg,
	NT_ElseStmt,
	NT_Term,
	NT_Var,
	NT_Operator,
	NT_MatrixElem,
	NT_LogicalOp,
	NT_RelationalOp,
	NT_MoreIDs,
	NT_IntegerList,
	NT_RemainingIntegerList,
	NT_ArgList,
	NT_MoreArgs
} SymbolType;

typedef struct symbol {
	SymbolType symbolType;
	struct Token* token;
} Symbol;

Symbol* generateSymbol(SymbolType symbolType);
int attachTokenToSymbol(Symbol* symbol, struct Token* token);
int isTerminal(Symbol* symbol);
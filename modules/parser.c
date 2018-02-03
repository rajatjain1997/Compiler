#include "../libraries/list.h"
#include "../libraries/token.h"
#include "../libraries/error.h"
#include "../libraries/tree.h"
#include "../libraries/symbol.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void raiseUnexpectedSymbolException(Symbol* expected, Token* received) {
	char msg[100];
	ErrorType e = ERROR;
	strcpy(msg, "Expected A recieved B");
	error(msg, e, received->lineno);
}

void raiseUnexpectedTerminationException() {
	char msg[100];
	ErrorType e = ERROR;
	strcpy(msg, "Unexpected token stream termination. You have skipped some symbols.");
	error(msg, e, -1);
}

void PDAPush(Stack PDAStack, StackSymbol lastPopped, SymbolType symbolType) {
	Symbol* symbol = generateSymbol(symbolType);
	Tree symbolTree = add_child(lastPopped.symbolTree, symbol);
	StackSymbol toPush;
	toPush.symbol = symbol;
	toPush.symbolTree = symbolTree;
	StackData data;
	data.value.stackSymbol = toPush;
	push(PDAStack, data);
}

StackSymbol PDAPop(Stack PDAStack, Token* currentToken) {
	StackData data;
	data = pop(PDAStack);
	StackSymbol popped = data.value.stackSymbol;
	if(isTerminal(popped.symbol)) {
		if(attachTokenToSymbol(extractSymbol(popped.symbolTree), currentToken)==-1) {
			raiseUnexpectedSymbolException(popped.symbol, currentToken);
		}
	}
	return popped;
}

Stack createPDAStack() {
	Stack stack = createStack();
	StackData data;
	Symbol* symbol = generateSymbol(NT_MainFxn);
	Tree symbolTree = createTree(symbol);
	StackSymbol toPush;
	toPush.symbol = symbol;
	toPush.symbolTree = symbolTree;
	data.value.stackSymbol = toPush;
	push(stack, data);
	return stack;
}

void rule1(Stack stack, StackSymbol stackSymbol) {
	//<mainFxn> -> MAIN SQO SQC <prog> END
	PDAPush(stack, stackSymbol, T_END);
	PDAPush(stack, stackSymbol, NT_Prog);
	PDAPush(stack, stackSymbol, T_SQC);
	PDAPush(stack, stackSymbol, T_SQO);
	PDAPush(stack, stackSymbol, T_MAIN);
}

void rule2(Stack stack, StackSymbol stackSymbol) {
	//<prog> -> <stmtOrFxnDef><stmtAndFxnDefs>
	PDAPush(stack, stackSymbol, NT_StmtsAndFxnDefs);
	PDAPush(stack, stackSymbol, NT_StmtOrFxnDef);
}

void rule3(Stack stack, StackSymbol stackSymbol) {
	//<stmtAndFxnDefs> -> <stmtOrFxnDef><stmtAndFxnDefs>
	PDAPush(stack, stackSymbol, NT_StmtsAndFxnDefs);
	PDAPush(stack, stackSymbol, NT_StmtOrFxnDef);
}

void rule4(Stack stack, StackSymbol stackSymbol) {
	//<stmtAndFxnDefs> -> e
}

void rule5(Stack stack, StackSymbol stackSymbol) {
	//<stmtOrFxnDef> -> <stmt>
	PDAPush(stack, stackSymbol, NT_Stmt);
}

void rule6(Stack stack, StackSymbol stackSymbol) {
	//<stmtOrFxnDef> -> <fxnDef>
	PDAPush(stack, stackSymbol, NT_FxnDef);
}

void rule7(Stack stack, StackSymbol stackSymbol) {
	//<stmt> -> <declStmt>
	PDAPush(stack, stackSymbol, NT_DeclStmt);
}

void rule8(Stack stack, StackSymbol stackSymbol) {
	//<stmt> -> <assignStmt>
	PDAPush(stack, stackSymbol, NT_AssignStmt);
}

void rule9(Stack stack, StackSymbol stackSymbol) {
	//<stmt> -> <ifStmt>
	PDAPush(stack, stackSymbol, NT_IfStmt);
}

void rule10(Stack stack, StackSymbol stackSymbol) {
	//<stmt> -> <ioStmt>
	PDAPush(stack, stackSymbol, NT_IoStmt);
}

void rule11(Stack stack, StackSymbol stackSymbol) {
	//<stmt> -> <fxnCallStmt>
	PDAPush(stack, stackSymbol, NT_FxnCallStmt);
}

void rule12(Stack stack, StackSymbol stackSymbol) {
	//<fxnDef> -> FUNCTION SQO <parameterList> SQC ASSIGNOP FUNID SQO <parameterList> SQC <prog> END
	PDAPush(stack, stackSymbol, T_END);
	PDAPush(stack, stackSymbol, NT_Prog);
	PDAPush(stack, stackSymbol, T_SQC);
	PDAPush(stack, stackSymbol, NT_ParameterList);
	PDAPush(stack, stackSymbol, T_SQO);
	PDAPush(stack, stackSymbol, T_FUNID);
	PDAPush(stack, stackSymbol, T_ASSIGNOP);
	PDAPush(stack, stackSymbol, T_SQC);
	PDAPush(stack, stackSymbol, NT_ParameterList);
	PDAPush(stack, stackSymbol, T_SQO);
	PDAPush(stack, stackSymbol, T_FUNCTION);
}

void rule13(Stack stack, StackSymbol stackSymbol) {
	//<parameterList> -> <type> ID <remainingList>
	PDAPush(stack, stackSymbol, NT_RemainingList);
	PDAPush(stack, stackSymbol, T_ID);
	PDAPush(stack, stackSymbol, NT_Type);
}

void rule14(Stack stack, StackSymbol stackSymbol) {
	//<type> -> INT
	PDAPush(stack, stackSymbol, T_INT);
}

void rule15(Stack stack, StackSymbol stackSymbol) {
	//<type> -> REAL
	PDAPush(stack, stackSymbol, T_REAL);
}

void rule16(Stack stack, StackSymbol stackSymbol) {
	//<type> -> STRING
	PDAPush(stack, stackSymbol, T_STRING);
}

void rule17(Stack stack, StackSymbol stackSymbol) {
	//<type> -> MATRIX
	PDAPush(stack, stackSymbol, T_MATRIX);
}

void rule18(Stack stack, StackSymbol stackSymbol) {
	//<remainingList> -> COMMA <parameterList>
	PDAPush(stack, stackSymbol, NT_ParameterList);
	PDAPush(stack, stackSymbol, T_COMMA);
}

void rule19(Stack stack, StackSymbol stackSymbol) {
	//<remainingList> -> e
}

void rule20(Stack stack, StackSymbol stackSymbol) {
	//<declStmt> -> <type> <varList> SEMICOLON
	PDAPush(stack, stackSymbol, T_SEMICOLON);
	PDAPush(stack, stackSymbol, NT_VarList);
	PDAPush(stack, stackSymbol, NT_Type);
}

void rule21(Stack stack, StackSymbol stackSymbol) {
	//<assingStmt> -> <leftHandSide> ASSIGNOP <righHandSide> SEMICOLON
	PDAPush(stack, stackSymbol, T_SEMICOLON);
	PDAPush(stack, stackSymbol, NT_RightHandSide);
	PDAPush(stack, stackSymbol, T_ASSIGNOP);
	PDAPush(stack, stackSymbol, NT_LeftHandSide);
}

void rule22(Stack stack, StackSymbol stackSymbol) {
	//<leftHandSide> -> ID
	PDAPush(stack, stackSymbol, T_ID);
}

void rule23(Stack stack, StackSymbol stackSymbol) {
	//<leftHandSide> -> SQO <varList> SQC
	PDAPush(stack, stackSymbol, T_SQC);
	PDAPush(stack, stackSymbol, NT_VarList);
	PDAPush(stack, stackSymbol, T_SQO);
}

void rule24(Stack stack, StackSymbol stackSymbol) {
	//<rightHandSide> -> <arithmeticExpr>
	PDAPush(stack, stackSymbol, NT_ArithmeticExpr);
}

void rule25(Stack stack, StackSymbol stackSymbol) {
	//<rightHandSide> -> <fxnCall>
	PDAPush(stack, stackSymbol, NT_FxnCall);
}

void rule26(Stack stack, StackSymbol stackSymbol) {
	//<rightHandSide> -> SIZE <var>
	PDAPush(stack, stackSymbol, NT_Var);
	PDAPush(stack, stackSymbol, T_SIZE);
}

void rule27(Stack stack, StackSymbol stackSymbol) {
	//<rightHandSide> -> <matrix>
	PDAPush(stack, stackSymbol, NT_Matrix);
}

void rule28(Stack stack, StackSymbol stackSymbol) {
	//<ifStmt> -> IF <booleanExpr> <stmt> <stmtProg> <elseStmt>
	PDAPush(stack, stackSymbol, NT_ElseStmt);
	PDAPush(stack, stackSymbol, NT_StmtProg);
	PDAPush(stack, stackSymbol, NT_Stmt);
	PDAPush(stack, stackSymbol, NT_BooleanExpr);
	PDAPush(stack, stackSymbol, T_IF);
}

void rule29(Stack stack, StackSymbol stackSymbol) {
	//<elseStmt> -> ELSE <stmt> <stmtProg> ENDIF
	PDAPush(stack, stackSymbol, T_ENDIF);
	PDAPush(stack, stackSymbol, NT_StmtProg);
	PDAPush(stack, stackSymbol, NT_Stmt);
	PDAPush(stack, stackSymbol, T_ELSE);
}

void rule30(Stack stack, StackSymbol stackSymbol) {
	//<elseStmt> -> ENDIF
	PDAPush(stack, stackSymbol, T_ENDIF);
}

void rule31(Stack stack, StackSymbol stackSymbol) {
	//<ioStmt> -> READ OP <var> CL SEMICOLON
	PDAPush(stack, stackSymbol, T_SEMICOLON);
	PDAPush(stack, stackSymbol, T_CL);
	PDAPush(stack, stackSymbol, NT_Var);
	PDAPush(stack, stackSymbol, T_OP);
	PDAPush(stack, stackSymbol, T_READ);
}

void rule32(Stack stack, StackSymbol stackSymbol) {
	//<ioStmt> -> PRINT OP <var> CL SEMICOLON
	PDAPush(stack, stackSymbol, T_SEMICOLON);
	PDAPush(stack, stackSymbol, T_CL);
	PDAPush(stack, stackSymbol, NT_Var);
	PDAPush(stack, stackSymbol, T_OP);
	PDAPush(stack, stackSymbol, T_PRINT);
}

void rule33(Stack stack, StackSymbol stackSymbol) {
	//<stmtProg> -> <stmt> <stmtProg>
	PDAPush(stack, stackSymbol, NT_StmtProg);
	PDAPush(stack, stackSymbol, NT_Stmt);
}

void rule34(Stack stack, StackSymbol stackSymbol) {
	//<stmtProg> -> e
}

void rule35(Stack stack, StackSymbol stackSymbol) {
	//<booleanExpr> -> OP <booleanExpr> CL <logicalOp> OP <booleanExpr> CL
	PDAPush(stack, stackSymbol, T_CL);
	PDAPush(stack, stackSymbol, NT_BooleanExpr);
	PDAPush(stack, stackSymbol, T_OP);
	PDAPush(stack, stackSymbol, NT_LogicalOp);
	PDAPush(stack, stackSymbol, T_CL);
	PDAPush(stack, stackSymbol, NT_BooleanExpr);
	PDAPush(stack, stackSymbol, T_OP);
}

void rule36(Stack stack, StackSymbol stackSymbol) {
	//<booleanExpr> -> <var> <relationalOp> <var>
	PDAPush(stack, stackSymbol, NT_Var);
	PDAPush(stack, stackSymbol, NT_RelationalOp);
	PDAPush(stack, stackSymbol, NT_Var);
}

void rule37(Stack stack, StackSymbol stackSymbol) {
	//<booleanExpr> -> NOT OP <booleanExpr> CL
	PDAPush(stack, stackSymbol, T_CL);
	PDAPush(stack, stackSymbol, NT_BooleanExpr);
	PDAPush(stack, stackSymbol, T_OP);
	PDAPush(stack, stackSymbol, T_NOT);
}

void rule38(Stack stack, StackSymbol stackSymbol) {
	//<var> -> ID
	PDAPush(stack, stackSymbol, T_ID);
}

void rule39(Stack stack, StackSymbol stackSymbol) {
	//<var> -> NUM
	PDAPush(stack, stackSymbol, T_NUM);
}

void rule40(Stack stack, StackSymbol stackSymbol) {
	//<var> -> RNUM
	PDAPush(stack, stackSymbol, T_RNUM);
}

void rule41(Stack stack, StackSymbol stackSymbol) {
	//<var> -> <matrixElem>
	PDAPush(stack, stackSymbol, NT_MatrixElem);
}

void rule42(Stack stack, StackSymbol stackSymbol) {
	//<var> -> STR
	PDAPush(stack, stackSymbol, T_STR);
}

void rule43(Stack stack, StackSymbol stackSymbol) {
	//<matrixElem> -> ID SQO NUM COMMA NUM SQC
	PDAPush(stack, stackSymbol, T_SQC);
	PDAPush(stack, stackSymbol, T_NUM);
	PDAPush(stack, stackSymbol, T_COMMA);
	PDAPush(stack, stackSymbol, T_NUM);
	PDAPush(stack, stackSymbol, T_SQO);
	PDAPush(stack, stackSymbol, T_ID);
}

void rule44(Stack stack, StackSymbol stackSymbol) {
	//<logicalOp> -> AND
	PDAPush(stack, stackSymbol, T_AND);
}

void rule45(Stack stack, StackSymbol stackSymbol) {
	//<logicalOp> -> OR
	PDAPush(stack, stackSymbol, T_OR);
}

void rule46(Stack stack, StackSymbol stackSymbol) {
	//<relationalOp> -> LT
	PDAPush(stack, stackSymbol, T_LT);
}

void rule47(Stack stack, StackSymbol stackSymbol) {
	//<relationalOp> -> LE
	PDAPush(stack, stackSymbol, T_LE);
}

void rule48(Stack stack, StackSymbol stackSymbol) {
	//<relationalOp> -> EQ
	PDAPush(stack, stackSymbol, T_EQ);
}

void rule49(Stack stack, StackSymbol stackSymbol) {
	//<relationalOp> -> GT
	PDAPush(stack, stackSymbol, T_GT);
}

void rule50(Stack stack, StackSymbol stackSymbol) {
	//<relationalOp> -> LT
	PDAPush(stack, stackSymbol, T_GE);
}

void rule51(Stack stack, StackSymbol stackSymbol) {
	//<relationalOp> -> LT
	PDAPush(stack, stackSymbol, T_NE);
}

void rule52(Stack stack, StackSymbol stackSymbol) {
	//<varList> -> ID <moreIDs>
	PDAPush(stack, stackSymbol, NT_MoreIDs);
	PDAPush(stack, stackSymbol, T_ID);
}

void rule53(Stack stack, StackSymbol stackSymbol) {
	//<moreIDs> -> COMMA <varList>
	PDAPush(stack, stackSymbol, NT_VarList);
	PDAPush(stack, stackSymbol, T_COMMA);
}

void rule54(Stack stack, StackSymbol stackSymbol) {
	//<moreIDs> -> e
}

void rule55(Stack stack, StackSymbol stackSymbol) {
	//<matrix> -> SQO <integerList> SQC
	PDAPush(stack, stackSymbol, T_SQC);
	PDAPush(stack, stackSymbol, NT_IntegerList);
	PDAPush(stack, stackSymbol, T_SQO);
}

void rule56(Stack stack, StackSymbol stackSymbol) {
	//<integerList> -> NUM <remainingIntegerList>
	PDAPush(stack, stackSymbol, NT_RemainingIntegerList);
	PDAPush(stack, stackSymbol, T_NUM);
}

void rule57(Stack stack, StackSymbol stackSymbol) {
	//<remainingIntegerList> -> COMMA NUM <remainingIntegerList>
	PDAPush(stack, stackSymbol, NT_RemainingIntegerList);
	PDAPush(stack, stackSymbol, T_NUM);
	PDAPush(stack, stackSymbol, T_COMMA);
}

void rule58(Stack stack, StackSymbol stackSymbol) {
	//<reamainingIntegerList> -> SEMICOLON <integerList>
	PDAPush(stack, stackSymbol, NT_IntegerList);
	PDAPush(stack, stackSymbol, T_SEMICOLON);
}

void rule59(Stack stack, StackSymbol stackSymbol) {
	//<remainingIntegerList> -> e
}

void rule60(Stack stack, StackSymbol stackSymbol) {
	//<fxnCallStmt> -> <fxnCall> SEMICOLON
	PDAPush(stack, stackSymbol, T_SEMICOLON);
	PDAPush(stack, stackSymbol, T_FxnCall);
}

void rule61(Stack stack, StackSymbol stackSymbol) {
	//<fxnCall> -> FUNID OP <argList> CL
	PDAPush(stack, stackSymbol, T_CL);
	PDAPush(stack, stackSymbol, NT_ArgList);
	PDAPush(stack, stackSymbol, T_OP);
	PDAPush(stack, stackSymbol, T_FUNID);
}

void rule62(Stack stack, StackSymbol stackSymbol) {
	//<argList> -> <rightHandSide> <moreArgs>
	PDAPush(stack, stackSymbol, NT_MoreArgs);
	PDAPush(stack, stackSymbol, NT_RightHandSide);
}

void rule63(Stack stack, StackSymbol stackSymbol) {
	//<moreArgs> -> COMMA <argList>
	PDAPush(stack, stackSymbol, NT_ArgList);
	PDAPush(stack, stackSymbol, T_COMMA);
}

void rule64(Stack stack, StackSymbol stackSymbol) {
	//<moreArgs> -> e
}

//Need to fix rule 35 and 36 to be left associative

// void rule35(Stack stack, StackSymbol stackSymbol) {
// 	//<arithmeticExpr> -> <term> <operator> <arithmeticExpr>
// 	PDAPush(stack, stackSymbol, NT_ArithmeticExpr);
// 	PDAPush(stack, stackSymbol, NT_Operator);
// 	PDAPush(stack, stackSymbol, NT_Term);
// }

// void rule36(Stack stack, StackSymbol stackSymbol) {
// 	//<arithmeticExpr> -> <term>
// 	PDAPush(stack, stackSymbol, NT_Term);
// }

//Not completing arithmetic and <operator>

Tree parse(Queue tokenStream) {
	Stack stack = createPDAStack();
	Tree parseTree = top(stack).value.stackSymbol.symbolTree;
	Token* currentToken;
	QueueData data;
	StackSymbol stackSymbol;
	while(tokenStream->size>0) {
		data = dequeue(tokenStream);
		currentToken = data.value;
		switch(currentToken->type) {
			case ASSIGNOP: 
				stackSymbol = PDAPop(stack, currentToken);
				if(!isTerminal(stackSymbol.symbol)) {
					raiseUnexpectedSymbolException(stackSymbol.symbol, currentToken);
				}
				break;
			case FUNID:
				while(1) {
					stackSymbol = PDAPop(stack, currentToken);
					if(isTerminal(stackSymbol.symbol)) {
						break;
					}
					switch(stackSymbol.symbol->symbolType) {
						case NT_Prog: rule2(); break;
						case NT_StmtsAndFxnDefs: rule3(); break;
						case NT_StmtOrFxnDef: rule5(); break;
						case NT_Stmt: rule11(); break;
						case NT_FxnCallStmt: rule60(); break;
						case NT_FxnCall: rule61(); break;
						case NT_RightHandSide: rule25(); break;
						case NT_StmtProg: rule33(); break;
						default: raiseUnexpectedSymbolException(stackSymbol.symbol, currentToken);
					}
				}
				break;
		}
	}
	if(stack->size!=0) {
		raiseUnexpectedTerminationException();
	}
	return parseTree;
}
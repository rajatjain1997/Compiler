/**
 *	AUTHOR: Rajat Jain
 *  ID No. 2015A7PS0549P
 */

#include "lexer.h"
#include "parser.h"
#include "error.h"
#include "list.h"
#include "tree.h"
#include "symbol.h"
#include "token.h"
#include "grammar.h"
#include "ast.h"
#include "type.h"
#include "intermediate.h"
#include "symboltable.h"
#include "quadruple.h"
#include "codegenerator.h"
#include <stdio.h>
#include <string.h>


void printQuadruples(List code) {
	Element temp = code->first;
	Quadruple* quad; int i;
	while(temp!=NULL) {
		quad = temp->data.value.quadruple;
		printf("%-5d|", quad->operator);
		for(i=0; i<3;i++) {
			if(quad->op[i]==NULL) {
				printf("||");
				continue;
			}
			switch(quad->op[i]->type) {
				case INT: printf("%-5d|", quad->op[i]->address.integer); break;
				case REAL: printf("%-5f|", quad->op[i]->address.real); break;
				case STRING: printf("%-20s|", ((String*) quad->op[i]->address.entry)->value); break;
				case MATRIX: printf("MATRIX"); break;
				case FUNID: printf("%-20s|", ((Tree) quad->op[i]->address.entry)->symbol->token->value.lexeme); break;
				case IF: printf("%-20s|", (char*) quad->op[i]->address.entry); break;
				default: printf("%-20s|", ((struct symbolTableEntry*) quad->op[i]->address.entry)->tokentree->symbol->token->value.lexeme);
			}
		}
		printf("\n");
		temp = temp->next;
	}
}

/*
 * void visitInOrder(Tree tree, FILE* fp): Used to print each node of the parse tree in order.
 */

void visitDFT(Tree tree, FILE* fp) {
	char buf[20]; Token* token;
	token = getToken(tree->symbol);
	if(isTerminal(tree->symbol) && token!=NULL && tree->parent!=NULL && !isTerminal(tree->parent->symbol)) {
		getLexeme(token, buf);
		fprintf(fp,"%-20s|%-10d|%-10s|%-23s\n", buf, token->lineno, tokenTypeToString[token->type], nonTerminalStrings[tree->parent->symbol->symbolType]);
	} else if(isTerminal(tree->symbol) && token!=NULL && tree->parent!=NULL && isTerminal(tree->parent->symbol)) {
		getLexeme(token, buf);
		fprintf(fp,"%-20s|%-10d|%-10s|%-23s\n", buf, token->lineno, tokenTypeToString[token->type], tokenTypeToString[tree->parent->symbol->symbolType]);
	} else if (!isTerminal(tree->symbol) && tree->parent!=NULL && !isTerminal(tree->parent->symbol)) {
		fprintf(fp, "%-20s|----------|----------|%-23s \n", nonTerminalStrings[tree->symbol->symbolType], nonTerminalStrings[tree->parent->symbol->symbolType]);
	} else if(!isTerminal(tree->symbol) && tree->parent!=NULL && isTerminal(tree->parent->symbol)) {
		fprintf(fp, "%-20s|----------|----------|%-23s \n", nonTerminalStrings[tree->symbol->symbolType], tokenTypeToString[tree->parent->symbol->symbolType]);
	} else if (isTerminal(tree->symbol) && tree->parent==NULL) {
		getLexeme(token, buf);
		fprintf(fp, "%-20s|%-10d|%-10s|----------------------- \n", buf, token->lineno, tokenTypeToString[token->type]);
	} else {
		fprintf(fp, "%d (Error)\n", tree->symbol->symbolType);
	}
}

void visitInOrder(Tree tree, FILE* fp) {
	char buf[20]; Token* token;
	token = getToken(tree->symbol);
	if(token!=NULL && token->type==NUM) {
		getLexeme(token, buf);
		fprintf(fp, "%-20s|%-10d|%-10s|%-10d|%-23s|YES|-----------------------\n", buf, token->lineno, tokenTypeToString[token->type], token->value.integer, nonTerminalStrings[tree->parent->symbol->symbolType]);
	} else if(token!=NULL && token->type==RNUM) {
		getLexeme(token, buf);
		fprintf(fp, "%-20s|%-10d|%-10s|%-10f|%-23s|YES|-----------------------\n", buf, token->lineno, tokenTypeToString[token->type], token->value.real, nonTerminalStrings[tree->parent->symbol->symbolType]);
	} else if(isTerminal(tree->symbol) && token!=NULL && tree->parent!=NULL) {
		getLexeme(token, buf);
		fprintf(fp,"%-20s|%-10d|%-10s|----------|%-23s|YES|-----------------------\n", buf, token->lineno, tokenTypeToString[token->type], nonTerminalStrings[tree->parent->symbol->symbolType]);
	} else if (!isTerminal(tree->symbol) && tree->parent!=NULL) {
		fprintf(fp, "--------------------|----------|----------|----------|%-23s| NO|%-23s \n", nonTerminalStrings[tree->parent->symbol->symbolType], nonTerminalStrings[tree->symbol->symbolType]);
	} else if (!isTerminal(tree->symbol)) {
		fprintf(fp, "--------------------|----------|----------|----------|-----------------------| NO|%-23s \n", nonTerminalStrings[tree->symbol->symbolType]);
	} else {
		fprintf(fp, "%d (Error)\n", tree->symbol->symbolType);
	}
}

/*
 * void inOrderTraversal(Tree tree, FILE* fp): used to traverse the passed tree in order.
 */

void inOrderTraversal(Tree tree, FILE* fp) {
	Element temp;
	if(tree->children->size==0) {
		visitInOrder(tree, fp);
	} else {
		inOrderTraversal(tree->children->first->data.value.tree, fp);
		visitInOrder(tree, fp);
		temp = tree->children->first->next;
		while(temp!=NULL) {
			inOrderTraversal(temp->data.value.tree, fp);
			temp = temp->next;
		}
	}
}

void DFT(Tree tree, FILE* fp) {
	Element temp;
	if(tree->children->size==0) {
		visitDFT(tree, fp);
	} else {
		visitDFT(tree, fp);
		DFT(tree->children->first->data.value.tree, fp);
		temp = tree->children->first->next;
		while(temp!=NULL) {
			DFT(temp->data.value.tree, fp);
			temp = temp->next;
		}
	}
}

/*
 * void printTree(Tree tree, FILE* fp): Used to print the parse tree in order.
 */

void printTree(Tree tree, FILE* fp) {
	fprintf(fp, "Lexeme              |Line No.  |Token     |Value     |Parent Nonterminal     |Y/N|Nonterminal\n");
	fprintf(fp, "====================|==========|==========|==========|=======================|===|=======================\n");
	inOrderTraversal(tree, fp);
	if(fp!=stdout) {
		fclose(fp);
	}
}

void printAST(Tree tree, FILE* fp) {
	fprintf(fp, "Lexeme              |Line No.  |Token     |Parent\n");
	fprintf(fp, "====================|==========|==========|=======================\n");
	DFT(tree, fp);
	if(fp!=stdout) {
		fclose(fp);
	}
}

/*
 * void printTokenStream(Queue tokenStream): Used to print the token stream returned by lexer.
 */

void printTokenStream(Queue tokenStream) {
	char buf[20];
	printf("Token Type|Lexeme              |Line no.\n");
	printf("==========|====================|==========\n");
	QueueElement temp = tokenStream->first;
	while(temp!=NULL) {
		getLexeme(temp->data.value, buf);
		printf("%-10s|%-20s|%-10d\n", tokenTypeToString[temp->data.value->type], buf, temp->data.value->lineno);
		temp=temp->next;
	}
}

void printSymbolTable(SymbolTable st, int level) {
	int i = 0; Element temp; Token* token; char buf[21], buf2[21], buf3[21]; struct idEntry* ste;
	if(level==0) {
		printf("Identifier Name     |Scope Name          |N. Level|Static Parent       |Type     |Width|Offset\n");
		printf("====================|====================|========|====================|=========|=====|=======\n");
	}
	while(i<st->size) {
		temp = st->symboltable[i]->first;
		while(temp!=NULL) {
			token = getToken(extractSymbol(temp->data.value.symboltableentry->tokentree));
			getLexeme(token, buf);
			if(token->type==FUNID) {
				printSymbolTable(temp->data.value.symboltableentry->value.funentry->scope, level+1);
			} else if(st->func==NULL){
				ste = temp->data.value.symboltableentry->value.identry;
				printf("%-20s|MAIN                |%-8d|N.A.                |%-9s|%-5d|%-7d\n", buf, level, tokenTypeToString[ste->type->type], ste->size, ste->offset);
			} else if(st->parent->parent==NULL) {
				ste = temp->data.value.symboltableentry->value.identry;
				getLexeme(getToken(extractSymbol(st->func)), buf2);
				printf("%-20s|%-20s|%-8d|MAIN                |%-9s|%-5d|%-7d\n", buf, buf2, level, tokenTypeToString[ste->type->type], ste->size, ste->offset);
			} else {
				ste = temp->data.value.symboltableentry->value.identry;
				getLexeme(getToken(extractSymbol(st->func)), buf2);
				getLexeme(getToken(extractSymbol(st->parent->func)), buf3);
				printf("%-20s|%-20s|%-8d|%-20s|%-9s|%-5d|%-7d\n", buf, buf2, level, buf3,tokenTypeToString[ste->type->type], ste->size, ste->offset);
			}
			temp = temp->next;
		}
		i++;
	}
}

int choicemenu() {
	int choice;
	printf("What would you like to do :-)?\n");
	printf("0. Exit and print errors if any\n");
	printf("1. Print the captured token stream\n");
	printf("2. Print parse tree\n");
	printf("3. Print AST in pre-order traversal\n");
	printf("4. Print parse tree to AST conversion stats\n");
	printf("5. Print Symbol Table\n");
	printf("6. Print Semantic/Syntactic Errors\n");
	printf("7. Generate Code!\n");
	scanf("%d", &choice);
	return choice;
}

int main(int argc, char* argv[]) {
	int testing = 1; int cleaning  = 1; int i; int choice, ASTSize = 0, outerLoop = 0, parseSize = 0;
	FILE* parseTreeOut;
	char cleanDest[20];
	if(argc<2) {
		printf("No source file specified.");
		return 0;
	}
	// for(i=2; i<argc; i++) {
	// 	testing = strcmp(argv[i], "-test");
	// 	cleaning = strcmp(argv[i], "-clean");
	// 	if(cleaning==0) {
	// 		i++;
	// 		if(i<argc) {
	// 			strcpy(cleanDest, argv[i]);
	// 		} else {
	// 			strcpy(cleanDest, "1");
	// 		}
	// 	}
	// }
	//Compiler always runs in testing mode.
	testing = 0;
	//Compiler always prints cleaned code to console.
	strcpy(cleanDest,"1");
	initializeError(argv[1], !testing);
	Queue tokenstream; Tree parsetree; List intercode; SymbolTable st;

	printf("a) Lexer and Parser are implemented.\n");
	printf("b) First and follow sets are automated.\n");
	printf("c) All test cases run without any erraneous output. All errors specified in test case 5 are reported as required.\n");
	printf("d) Parse tree is successfully generated\n");
	choice = choicemenu();
	do {
		switch(choice) {
			case 0: outerLoop = 1; break;
			default:
				tokenstream = read(argv[1]);
				if(choice==1) {
					printTokenStream(tokenstream);
					choice = choicemenu();
				}
				parsetree = parse(tokenstream, "grammar.txt");
				if(choice==2) {
					printTree(parsetree, stdout);
					choice = choicemenu();
				}
				if(!checkErrorState()) {
					parseSize = createAST(parsetree, 0);
					if(choice==3) {
						printAST(parsetree, stdout);
						choice = choicemenu();
					}
					ASTSize = typeCheck(parsetree, 0);
					if(choice==4) {
						printf("Parse Tree No. of Nodes: %d Parse Tree Size: %d\n", parseSize,(int) (parseSize*sizeof(struct tree)));
						printf("AST No. of Nodes: %d AST Size: %d\n", ASTSize,(int) (ASTSize*sizeof(struct tree)));
						printf("Percentage Compression: %f%%\n", ((float) parseSize - ASTSize)*100/parseSize);
						choice = choicemenu();
					}
					if(choice==6) {
						printErrors();
						choice = choicemenu();
					}
					if(!checkErrorState()) {
						st = parsetree->attr[0];
						if(choice==5) {
							printSymbolTable(st, 0);
							choice = choicemenu();
						}
						intercode = generateIntermediateCode(parsetree);
						if(argc==3 && choice==7) {
							generateCode(argv[2], intercode, st);
							choice = choicemenu();
						} else if(choice>7) {
							choice = choicemenu();
						}
					} else if (choice==5 || choice==7){
						printErrors();
						choice = choicemenu();
					}
				} else if(choice>2) {
					printErrors();
					choice = choicemenu();
				}
			break;
		}
		// if(!checkErrorState()) {
		// 	parsetree = createAST(parsetree);
		// 	typeCheck(parsetree);
		// 	if(!checkErrorState()) {
		// 		SymbolTable st = parsetree->attr[0];
		// 		intercode = generateIntermediateCode(parsetree);
		// 		// printQuadruples(intercode);
		// 		fflush(stdout);
		// 		if(argc==3) {
		// 			generateCode(argv[2], intercode, st);
		// 		}
		// 	}
		// }
		// while(error_testing) {
		// 	switch(choice) {
		// 		case 3:
		// 			if(!checkErrorState()) {
		// 				printf("The code is syntactically correct!\n");
		// 			} else {
		// 				printErrors();
		// 			}
		// 			break;
		// 		case 4:
		// 				parseTreeOut = stdout;
		// 				printTree(parsetree, parseTreeOut);
		// 				break;
		// 		case 5: case 1: case 2: outerLoop = 1;
		// 				break;
		// 	}
		// 	if(outerLoop) {
		// 		break;
		// 	}
		// 	printf("What would you like to do :-)?\n");
		// 	printf("1. Clean the provided source file\n");
		// 	printf("2. Print the captured token stream\n");
		// 	printf("3. Print all errors associated to code\n");
		// 	printf("4. Print parse tree\n");
		// 	printf("5. Exit and print errors if any\n");
		// 	scanf("%d", &choice);
		// }
		// innerLoop = 0;
		// outerLoop = 0;
		// if(choice==5) {
		// 	break;
		// }
	} while(outerLoop==0);
	// if(!cleaning) {
	// 	clean(tokenstream, cleanDest);
	// }
	if(checkErrorState()) {
		printErrors();
	}
	printf("******************************************************\n");
	printf("Made By:\n");
	printf("Rajat Jain\n");
	printf("2015A7PS0549P\n");
	printf("Please suggest any changes you would like to make!\n");
	return 0;
}

#include "lexer.h"
#include "parser.h"
#include "error.h"
#include "list.h"
#include "tree.h"
#include "symbol.h"
#include "token.h"
#include "grammar.h"
#include <stdio.h>
#include <string.h>

void visitDFT(Tree tree) {
	char buf[20];
	if(tree->symbol->token!=NULL && tree->symbol->token->type==NUM) {
		getLexeme(tree->symbol->token, buf);
		printf("%-20s|%-10d|%-10s|%-10d|%-23s|YES|-----------------------\n", buf, tree->symbol->token->lineno, tokenTypeToString[tree->symbol->token->type], tree->symbol->token->value.integer, nonTerminalStrings[tree->parent->symbol->symbolType]);
	} else if(tree->symbol->token!=NULL && tree->symbol->token->type==RNUM) {
		getLexeme(tree->symbol->token, buf);
		printf("%-20s|%-10d|%-10s|%-10f|%-23s|YES|-----------------------\n", buf, tree->symbol->token->lineno, tokenTypeToString[tree->symbol->token->type], tree->symbol->token->value.real, nonTerminalStrings[tree->parent->symbol->symbolType]);
	} else if(isTerminal(tree->symbol) && tree->symbol->token!=NULL) {
		getLexeme(tree->symbol->token, buf);
		printf("%-20s|%-10d|%-10s|----------|%-23s|YES|-----------------------\n", buf, tree->symbol->token->lineno, tokenTypeToString[tree->symbol->token->type], nonTerminalStrings[tree->parent->symbol->symbolType]);
	} else if (!isTerminal(tree->symbol) && tree->parent!=NULL) {
		printf("--------------------|----------|----------|----------|%-23s| NO|%-23s \n", nonTerminalStrings[tree->parent->symbol->symbolType], nonTerminalStrings[tree->symbol->symbolType]);
	} else if (!isTerminal(tree->symbol)) {
		printf("--------------------|----------|----------|----------|-----------------------| NO|%-23s \n", nonTerminalStrings[tree->symbol->symbolType]);
	} else {
		printf("%d (Error)\n", tree->symbol->symbolType);
	}
}

void printTree(Tree tree) {
	printf("Lexeme              |Line No.  |Token     |Value     |Parent Nonterminal     |Y/N|Nonterminal\n");
	printf("====================|==========|==========|==========|=======================|===|=======================\n");
	DFT(tree);
}

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

int main(int argc, char* argv[]) {
	int testing = 1; int cleaning  = 1; int i; int choice, innerLoop = 0, outerLoop = 0;
	char cleanDest[20];
	if(argc<2) {
		printf("No source file specified.");
		return 0;
	}
	for(i=2; i<argc; i++) {
		testing = strcmp(argv[i], "-test");
		cleaning = strcmp(argv[i], "-clean");
		if(cleaning==0) {	
			i++;
			if(i<argc) {
				strcpy(cleanDest, argv[i]);
			} else {
				strcpy(cleanDest, "1");
			}
		}
	}
	//Compiler always runs in testing mode.
	testing = 0;
	initializeError(argv[1], !testing);
	Queue tokenstream; Tree parsetree;

	printf("a) Lexer and Parser are implemented.\n");
	printf("b) First and follow sets are automated.\n");
	printf("c) All test cases run without any erraneous output. All errors specified in test case 5 are reported as required.\n");
	printf("d) Parse tree is successfully generated\n");

	while(error_testing) {
		tokenstream = read(argv[1]);
		while(error_testing) {
			printf("What would you like to do :-)?\n");
			printf("1. Clean the provided source file\n");
			printf("2. Print the captured token stream\n");
			printf("3. Print all errors associated to code\n");
			printf("4. Print parse tree\n");
			printf("5. Exit and print errors if any\n");
			scanf("%d", &choice);
			switch(choice) {
				case 1: clean(tokenstream, "1");
						printf("\n");
						break;
				case 2: printTokenStream(tokenstream);
						break;
				default:innerLoop=1; 
						break;
			}
			if(innerLoop) {
				break;
			}
		}
		parsetree = parse(tokenstream, "grammar.txt");
		while(error_testing) {
			switch(choice) {
				case 3: 
					if(!checkErrorState()) {
						printf("The code is syntactically correct!\n");
					} else {
						printErrors();
					}
					break;
				case 4: printTree(parsetree);
						break;
				case 5: case 1: case 2: outerLoop = 1;
						break;
			}
			if(outerLoop) {
				break;
			}
			printf("What would you like to do :-)?\n");
			printf("1. Clean the provided source file\n");
			printf("2. Print the captured token stream\n");
			printf("3. Print all errors associated to code\n");
			printf("4. Print parse tree\n");
			printf("5. Exit and print errors if any\n");
			scanf("%d", &choice);
		}
		innerLoop = 0;
		outerLoop = 0;
		if(choice==5) {
			break;
		}
	}
	if(!cleaning) {
		clean(tokenstream, cleanDest);
	}
	if(checkErrorState()) {
		printErrors();
	}
	printf("******************************************************\n");
	printf("Made By:\n");
	printf("Rajat Jain\n");
	printf("2015A7PS0549P\n");
	printf("Please suggest any changes you would like to make!");
	return 0;
}
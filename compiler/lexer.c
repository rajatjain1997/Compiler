/**
 *	AUTHOR: Rajat Jain
 *  ID No. 2015A7PS0549P
 */

#include "lexer.h"
#include "list.h"
#include"token.h"
#include"error.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

/*
 * void raiseFileCorruptException(): Raises an error if the file cannot be opened by the lexer. The error has no line number so that it floats to the top.
 */

void raiseFileCorruptException() {
	char msg[100];
	ErrorType e = ERROR;
	strcpy(msg, "Cannot open file");
	error(msg, e, -1);
}

/*
 * void raiseSymbolNotRecognizedException(char lookahead, int lineno): Raises an error if the symbol is not expected in the matched pattern.
 */

void raiseSymbolNotRecognizedException(char lookahead, int lineno) {
	char msg[256];
	ErrorType e = ERROR;
	sprintf(msg, "LEXICAL ERROR: Symbol not expected here %c ASCII(%d)", lookahead, lookahead);
	error(msg, e, lineno);
}

/*
 * void raisePatternNotRecognizedException(char lookahead, int lineno): Raises an error if the pattern leads to a trap state in the DFA.
 */

void raisePatternNotRecognizedException(char* buf, int ptr, int start, int lineno) {
	char msg[256];
	ErrorType e = ERROR;
	char lookahead = buf[ptr];
	buf[ptr] = '\0';
	sprintf(msg, "LEXICAL ERROR: Unidentified pattern: %s", buf+start);
	buf[ptr] = lookahead;
	error(msg, e, lineno);
}

/*
 * void raiseSymbolNotRecognizedException(char lookahead, int lineno): Raises an error if tokens overflow the current buffersize of 50 bits. Else the entire token is printed
 * while error reporting.
 */

void raiseBufferOverflowException(int lineno) {
	char msg[256];
	ErrorType e = ERROR;
	sprintf(msg, "LEXICAL ERROR: Unexpectedly large token at line number %d", lineno);
	error(msg, e, lineno);
}

/*
 * int incrementptr(char* buf, int ptr, int start, FILE* fp): Increments lookahead pointer in the abstract buffer to point to the next character in file.
 * The dual buffer is implemented with a dynamic boundary at buf+start such that all characters in buf2 = buf+start are new while those in buf1 = buf have already been
 * tokenized.
 */

int incrementptr(char* buf, int ptr, int start, FILE* fp) {
	if(ptr<49) {
		return ++ptr;
	} else {
		int size = 0;
		if(start==0 || buf[51]) {
			size = fread(buf, 1, 50, fp);
			buf[size] = '\0';
			buf[51]=1;
			return 0;
		}
		strcpy(buf, buf+start);
		size=fread(buf+50-start, 1, start, fp);
		buf[50-start+size] = '\0';
		return 50-start;
	}
}

/*
 * void finalState(TokenType tokenType, char* buf, int* ptr, int* start, char lookahead, int lineno, Queue tokenStream): Tokenizes tokens in buffer from buf+start to ptr.
 * Also manages the buffer in case the dynamic boundary is overflowing and sets the buffer state.
 * It is also responsible for adding the tokens to the token stream or raise an exception is there is any error.
 */

void finalState(TokenType tokenType, char* buf, int* ptr, int* start, char lookahead, int lineno, Queue tokenStream) {
	QueueData tokenData;
	if(tokenType!=COMMENT) {
		buf[*ptr] = '\0';
		if(!buf[51]) {
			tokenData.value = tokenize(tokenType, buf+*start, lineno);
		} else {
			raiseBufferOverflowException(lineno);
			tokenData.value = NULL;
		}
		if(tokenData.value!=NULL) {
			enqueue(tokenStream, tokenData);
		}
		buf[*ptr]=lookahead;
	}
	buf[51] = 0;
	*start = *ptr;
	*ptr=*ptr-1;
}

/*
 * void lex(Queue tokenStream, FILE* fp): The DFA attached to the lexer. It uses the file provided to it to generate a tokenstream from captured lexemes.
 */

void lex(Queue tokenStream, FILE* fp) {
	initializeTokenizer();
	char* buf = (char*) malloc(52);
	int size = fread(buf, 1, 50, fp);
	buf[size]='\0';
	buf[50] = '\0';
	buf[51] = 0;
	if(!size) {
		return;
	}
	int state = 0;
	int lineno = 1;
	int start = 0;
	int ptr = 0;
	while(1) {
		char lookahead = buf[ptr];
		switch(state) {
			case 0:
				if((lookahead>='a' && lookahead<='z')||(lookahead>='A' && lookahead<='Z'))
					state=1;
				else if(lookahead>='0' && lookahead<='9')
					state=2;
				else {
					switch(lookahead) {
						case '_': state=3; break;
						case '\"': state=4; break;
						case '+': state=5; break;
						case '-': state=6; break;
						case '*': state=7; break;
						case '/': state=8; break;
						case '<': state=9; break;
						case '>': state=10; break;
						case '=': state=11; break;
						case '.': state=12; break;
						case '@': state=13; break;
						case ',': state=14; break;
						case '[': state=15; break;
						case ']': state=16; break;
						case '(': state=17; break;
						case ')': state=18; break;
						case ';': state=19; break;
						case '#': state=20; break;
						case '\n': case '\r': lineno++; start++; break;
						case '\0': return;
						case ' ': start++; break;
						case '\t': start++; break;
						default: raiseSymbolNotRecognizedException(lookahead, lineno);
						ptr = incrementptr(buf, ptr, start, fp);
						start = ptr;
						state=0;
					};
				}
				break;
			case 1:
				if((lookahead>='a' && lookahead<='z')||(lookahead>='A' && lookahead<='Z'))
					state=1;
				else if(lookahead>='0' && lookahead<='9')
					state=21;
				else {
					finalState(ID, buf, &ptr, &start, lookahead, lineno, tokenStream);
					state=0;
				}
				break;
			case 2:
				if(lookahead>='0' && lookahead<='9')
					state=2;
				else if(lookahead=='.')
					state=22;
				else {
					finalState(NUM, buf, &ptr, &start, lookahead, lineno, tokenStream);
					state=0;
				}
				break;
			case 3:
				if((lookahead>='a' && lookahead<='z')||(lookahead>='A' && lookahead<='Z'))
					state=23;
				else {
					raisePatternNotRecognizedException(buf, ptr, start, lineno);
					start = ptr+1;
					ptr--;
					state=0;
				}
				break;
			case 4:
				if((lookahead>='a' && lookahead<='z') || lookahead==' ')
					state=24;
				else {
					raisePatternNotRecognizedException(buf, ptr, start, lineno);
					start = ptr+1;
					ptr--;
					state=0;
				}
				break;
			case 5:
				finalState(PLUS, buf, &ptr, &start, lookahead, lineno, tokenStream);
				state=0;
				break;
			case 6:
				finalState(MINUS, buf, &ptr, &start, lookahead, lineno, tokenStream);
				state=0;
				break;
			case 7:
				finalState(MUL, buf, &ptr, &start, lookahead, lineno, tokenStream);
				state=0;
				break;
			case 8:
				finalState(DIV, buf, &ptr, &start, lookahead, lineno, tokenStream);
				state=0;
				break;
			case 9:
				if(lookahead=='=')
					state=25;
				else {
					finalState(LT, buf, &ptr, &start, lookahead, lineno, tokenStream);
					state=0;
				}
				break;
			case 10:
				if(lookahead=='=')
					state=33;
				else {
					finalState(GT, buf, &ptr, &start, lookahead, lineno, tokenStream);
					state=0;
				}
				break;
			case 11:
				switch(lookahead) {
					case '/': state=26; break;
					case '=': state=27; break;
					default:
						finalState(ASSIGNOP, buf, &ptr, &start, lookahead, lineno, tokenStream);
						state=0;
				}
				break;
			case 12:
				switch(lookahead) {
					case 'a': state=28; break;
					case 'o': state=29; break;
					case 'n': state=30; break;
					default:
						raisePatternNotRecognizedException(buf, ptr, start, lineno);
						start = ptr;
						ptr--;
						state=0;
				}
				break;
			case 13:
				finalState(SIZE, buf, &ptr, &start, lookahead, lineno, tokenStream);
				state=0;
				break;
			case 14:
				finalState(COMMA, buf, &ptr, &start, lookahead, lineno, tokenStream);
				state=0;
				break;
			case 15:
				finalState(SQO, buf, &ptr, &start, lookahead, lineno, tokenStream);
				state=0;
				break;
			case 16:
				finalState(SQC, buf, &ptr, &start, lookahead, lineno, tokenStream);
				state=0;
				break;
			case 17:
				finalState(OP, buf, &ptr, &start, lookahead, lineno, tokenStream);
				state=0;
				break;
			case 18:
				finalState(CL, buf, &ptr, &start, lookahead, lineno, tokenStream);
				state=0;
				break;
			case 19:
				finalState(SEMICOLON, buf, &ptr, &start, lookahead, lineno, tokenStream);
				state=0;
				break;
			case 20:
				if(lookahead=='\n' || lookahead=='\0' || lookahead=='\r') {
					finalState(COMMENT, buf, &ptr, &start, lookahead, lineno, tokenStream);
					state=0;
				}
				else {
					state=20;
				}
				break;
			case 21:
				finalState(ID, buf, &ptr, &start, lookahead, lineno, tokenStream);
				state=0;
				break;
			case 22:
				if(lookahead>='0' && lookahead<='9')
					state=31;
				else {
					int tempptr = ptr-1;
					switch(lookahead) {
						case 'a': state=28; finalState(NUM, buf, &tempptr, &start, '.', lineno, tokenStream); break;
						case 'o': state=29; finalState(NUM, buf, &tempptr, &start, '.', lineno, tokenStream); break;
						case 'n': state=30; finalState(NUM, buf, &tempptr, &start, '.', lineno, tokenStream); break;
						default:
							raiseSymbolNotRecognizedException(lookahead, lineno);
							start = ptr;
							ptr--;
							state=0;
					}
				}
				break;
			case 23:
				if((lookahead>='a' && lookahead<='z')||(lookahead>='A' && lookahead<='Z')||(lookahead>='0' && lookahead<='9'))
					state=23;
				else {
					finalState(FUNID, buf, &ptr, &start, lookahead, lineno, tokenStream);
					state=0;
				}
				break;
			case 24:
				if((lookahead>='a' && lookahead<='z') || lookahead==' ')
					state=24;
				else if(lookahead=='\"')
					state=32;
				else {
					raisePatternNotRecognizedException(buf, ptr, start, lineno);
					start = ptr;
					ptr--;
					state=0;
				}
				break;
			case 25:
				finalState(LE, buf, &ptr, &start, lookahead, lineno, tokenStream);
				state=0;
				break;
			case 26:
				if(lookahead=='=')
					state=34;
				else {
					raisePatternNotRecognizedException(buf, ptr, start, lineno);
					start = ptr;
					ptr--;
					state=0;
				}
				break;
			case 27:
				finalState(EQ, buf, &ptr, &start, lookahead, lineno, tokenStream);
				state=0;
				break;
			case 28:
				if(lookahead=='n')
					state=35;
				else {
					raisePatternNotRecognizedException(buf, ptr, start, lineno);
					start = ptr;
					ptr--;
					state=0;
				}
				break;
			case 29:
				if(lookahead=='r')
					state=36;
				else {
					raisePatternNotRecognizedException(buf, ptr, start, lineno);
					start = ptr;
					ptr--;
					state=0;
				}
				break;
			case 30:
				if(lookahead=='o')
					state=37;
				else {
					raisePatternNotRecognizedException(buf, ptr, start, lineno);
					start = ptr;
					ptr--;
					state=0;
				}
				break;
			case 31:
				if(lookahead>='0' && lookahead<='9')
					state=38;
				else {
					raisePatternNotRecognizedException(buf, ptr, start, lineno);
					start = ptr;
					ptr--;
					state=0;
				}
				break;
			case 32:
				finalState(STR, buf, &ptr, &start, lookahead, lineno, tokenStream);
				state=0;
				break;
			case 33:
				finalState(GE, buf, &ptr, &start, lookahead, lineno, tokenStream);
				state=0;
				break;
			case 34:
				finalState(NE, buf, &ptr, &start, lookahead, lineno, tokenStream);
				state=0;
				break;
			case 35:
				if(lookahead=='d')
					state=39;
				else {
					raisePatternNotRecognizedException(buf, ptr, start, lineno);
					start = ptr;
					ptr--;
					state=0;
				}
				break;
			case 36:
				if(lookahead=='.')
					state=40;
				else {
					raisePatternNotRecognizedException(buf, ptr, start, lineno);
					start = ptr;
					ptr--;
					state=0;
				}
				break;
			case 37:
				if(lookahead=='t')
					state=41;
				else {
					raisePatternNotRecognizedException(buf, ptr, start, lineno);
					start = ptr;
					ptr--;
					state=0;
				}
				break;
			case 38:
				finalState(RNUM, buf, &ptr, &start, lookahead, lineno, tokenStream);
				state=0;
				break;
			case 39:
				if(lookahead=='.')
					state=42;
				else {
					raisePatternNotRecognizedException(buf, ptr, start, lineno);
					start = ptr;
					ptr--;
					state=0;
				}
				break;
			case 40:
				finalState(OR, buf, &ptr, &start, lookahead, lineno, tokenStream);
				state=0;
				break;
			case 41:
				if(lookahead=='.')
					state=43;
				else {
					raisePatternNotRecognizedException(buf, ptr, start, lineno);
					start = ptr;
					ptr--;
					state=0;
				}
				break;
			case 42:
				finalState(AND, buf, &ptr, &start, lookahead, lineno, tokenStream);
				state=0;
				break;
			case 43:
				finalState(NOT, buf, &ptr, &start, lookahead, lineno, tokenStream);
				state=0;
				break;
		};
		ptr = incrementptr(buf, ptr, start, fp);
		if(ptr<start) {
			start=0;
		}
	}
}

/*
 * Queue read(char* filename): Driver function for the lexer.
 * Etymology: The function is named "read" because this is the only instance where a stream to the user source code is opened.
 */

Queue read(char* filename) {
	Queue tokenStream = createQueue();
	FILE* fp = fopen(filename, "r");
	if(fp==NULL) {
		raiseFileCorruptException();
		return NULL;
	}
	lex(tokenStream, fp);
	fclose(fp);
	return tokenStream;
}

/*
 * void clean(Queue tokenStream, char* filename): Cleans the source code of any comments and prettifies it. This function does auto-indentation as well :-).
 * If filename is passed as "1", the cleaned code is printed on stdout.
 */

void clean(Queue tokenStream, char* filename) {
	QueueElement temp = tokenStream->first;
	char lexeme[20];
	Token* token;
	int indent = 0; int prevLineNo = 1; int i; int prevTokenType = -1; int firstword=1;
	FILE* fp;
	if(strcmp(filename, "1")) {
		fp = fopen(filename, "w");
	} else {
		fp = stdout;
	}
	while(temp!=NULL) {
		token = temp->data.value;
		getLexeme(token, lexeme);
		switch(token->type) {
			case ELSE: case END: case ENDIF: indent-=1; break;
		}
		if(token->lineno>prevLineNo) {
			prevLineNo = token->lineno;
			fprintf(fp, "\n");
			for(i=0; i<indent; i++) {
				fprintf(fp, "\t");
			}
			firstword = 1;
		}
		switch(token->type) {
			case ID: case FUNID: case END: case INT: case REAL: case STRING:
			case MATRIX: case MAIN: case IF: case ELSE: case ENDIF: case READ:
			case PRINT: case FUNCTION:
			if(!firstword) {
				fprintf(fp, " ");
			}
		}
		fprintf(fp,"%s", lexeme);
		switch(token->type) {
			case MAIN: case FUNCTION: case IF: case ELSE: indent+=1; break;
		}
		if(prevTokenType==token->type) {
			fprintf(fp, " ");
		}
		prevTokenType = token->type;
		firstword = 0;
		temp=temp->next;
	}
	if(fp!=stdout) {
		fclose(fp);
	}
}

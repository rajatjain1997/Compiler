#include"../libraries/token.h"
#include"../libraries/error.h"
#include<stdio.h>
#include<stdlib.h>

void raiseFileCorruptException(char* filename) {
	char msg[100];
	ErrorType e = ERROR;
	strcpy(msg, "Cannot open file with name ");
	strcat(msg, filename);
	error(msg, e, -1);
}

void raiseSymbolNotRecognizedException(char lookahead, int lineno) {
	char msg[100];
	ErrorType e = ERROR;
	strcpy(msg, "Symbol not expected here ");
	msg[25] = lookahead;
	msg[26] = '\0';
	error(msg, e, lineno);
}

void raiseBufferOverflowException() {
	char msg[100];
	ErrorType e = ERROR;
	strcpy(msg, "Lexical Buffer Overflow");
	error(msg, e, -1);
}

int incrementptr(char* buf, int ptr, int start, FILE* fp) {
	if(ptr<49) {
		return ptr++;
	} else {
		if(start=0) {
			raiseBufferOverflowException();
		}
		buf[50] = '\0';
		strcpy(buf, buf+start);
		if(!fgets(buf+50-start, start, fp))
			return -1;
		return 50-start;
	}
}

void traverse(char* buf, int ptr, FILE* fp) {
	int state = 0;
	int lineno = 0;
	int start = 0;
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
						case '\n': lineno++; break;
						case ' ': break;
						default: raiseSymbolNotRecognizedException(lookahead, lineno);
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
					ptr--;
					// tokenize(); Remember to add to token stream and update start to ptr and set the state back to 0
				}
				break;
			case 2:
				if(lookahead>='0' && lookahead<='9')
					state=2;
				else if(lookahead=='.')
					state=22;
				else {
					ptr--;
					// tokenize();
				}
				break;
			case 3:
				if((lookahead>='a' && lookahead<='z')||(lookahead>='A' && lookahead<='Z'))
					state=23;
				else {
					raiseSymbolNotRecognizedException(lookahead, lineno);
					state=0;
				}
				break;
			case 4:
				if(lookahead>='a' && lookahead<='z')
					state=24;
				else {
					raiseSymbolNotRecognizedException(lookahead, lineno);
					state=0;
				}
				break;
			case 5:
				ptr--;
				// tokenize();+
				break;
			case 6:
				ptr--;
				// tokenize-
				break;
			case 7:
				ptr--;
				// tokenize*
				break;
			case 8:
				ptr--;
				// tokenize/
				break;
			case 9:
				if(lookahead=='=')
					state=25;
				else {
					ptr--;
					// tokenize<
				}
				break;
			case 10:
				if(lookahead=='=')
					state=33;
				else {
					ptr--;
					// tokenize>
				}
				break;
			case 11:
				switch(lookahead) {
					case '/': state=26; break;
					case '=': state=27; break;
					default:
						ptr--;
						// tokenize=
				}
				break;
			case 12:
				switch(lookahead) {
					case 'a': state=28; break;
					case 'o': state=29; break;
					case 'n': state=30; break;
					default: 
						raiseSymbolNotRecognizedException(lookahead, lineno);
						state=0;
				}
				break;
			case 13:
				ptr--;
				// tokenize@
				break;
			case 14:
				ptr--;
				// tokenize,
				break;
			case 15:
				ptr--;
				// tokenize[
				break;
			case 16:
				ptr--;
				// tokenize]
				break;
			case 17:
				ptr--;
				// tokenize(
				break;
			case 18:
				ptr--;
				// tokenize)
				break;
			case 19:
				ptr--;
				// tokenize;
				break;
			case 20:
				if(lookahead=='\n')
					state=0;
				else
					state=20;
				break;
			case 21:
				ptr--;
				// tokenizeID
				break;
			case 22:
				if(lookahead>='0' && lookahead<='9')
					state=31;
				else {
					raiseSymbolNotRecognizedException(lookahead, lineno);
					state=0;
				}
				break;
			case 23:
				if((lookahead>='a' && lookahead<='z')||(lookahead>='A' && lookahead<='Z')||(lookahead>='0' && lookahead<='9'))
					state=23;
				else {
					raiseSymbolNotRecognizedException(lookahead, lineno);
					state=0;
				}
				break;
			case 24:
				if(lookahead>='a' && lookahead<='z')
					state=24;
				else if(lookahead=='\"')
					state=32;
				else {
					raiseSymbolNotRecognizedException(lookahead, lineno);
					state=0;
				}
				break;
			case 25:
				ptr--;
				// tokenize<=
				break;
			case 26:
				if(lookahead=='=')
					state=34;
				else {
					raiseSymbolNotRecognizedException(lookahead, lineno);
					state=0;
				}
				break;
			case 27:
				ptr--;
				// tokenize==;
				break;
			case 28:
				if(lookahead=='n')
					state=35;
				else {
					raiseSymbolNotRecognizedException(lookahead, lineno);
					state=0;
				}
				break;
			case 29:
				if(lookahead=='r')
					state=36;
				else {
					raiseSymbolNotRecognizedException(lookahead, lineno);
					state=0;
				}
				break;
			case 30:
				if(lookahead=='o')
					state=37;
				else {
					raiseSymbolNotRecognizedException(lookahead, lineno);
					state=0;
				}
				break;
			case 31:
				if(lookahead>='0' && lookahead<='9') 
					state=38;
				else {
					raiseSymbolNotRecognizedException(lookahead, lineno);
					state=0;
				}
				break;
			case 32:
				ptr--;
				// tokenizestr
				break;
			case 33:
				ptr--;
				// tokenize>=
				break;
			case 34:
				ptr--;
				// tokenizenoteq
				break;
			case 35:
				if(lookahead=='d')
					state=39;
				else {
					raiseSymbolNotRecognizedException(lookahead, lineno);
					state=0;
				}
				break;
			case 36:
				if(lookahead=='.')
					state=40;
				else {
					raiseSymbolNotRecognizedException(lookahead, lineno);
					state=0;
				}
				break;
			case 37:
				if(lookahead=='t')
					state=41;
				else {
					raiseSymbolNotRecognizedException(lookahead, lineno);
					state=0;
				}
				break;
			case 38:
				ptr--;
				// tokenizereal
				break;
			case 39:
				if(lookahead=='.')
					state=42;
				else {
					raiseSymbolNotRecognizedException(lookahead, lineno);
					state=0;
				}
				break;
			case 40:
				ptr--;
				// tokenizeor
				break;
			case 41:
				if(lookahead=='.')
					state=43;
				else {
					raiseSymbolNotRecognizedException(lookahead, lineno);
					state=0;
				}
				break;
			case 42:
				ptr--;
				// tokenizeand
				break;
			case 43:
				ptr--;
				// tokenizenot
				break;
		};
		ptr = incrementptr(buf, ptr, start, fp);
		if(ptr==-1) {
			break;
		} else if(ptr<start) {
			start=0;
		}
	}
}

Token* read(char* filename) {
	int lineno = 0;
	char* buf = (char*) malloc(51);
	FILE* fp = fopen(filename, "r");
	if(fp==NULL) {
		raiseFileCorruptException(filename);
		return NULL;
	}
	fgets(buf, 50, fp)
	traverse(buf, 0, fp);
}
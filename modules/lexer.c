#include"../libraries/list.h"
#include"../libraries/token.h"
#include"../libraries/error.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

void raiseFileCorruptException() {
	char msg[100];
	ErrorType e = ERROR;
	strcpy(msg, "Cannot open file");;
	error(msg, e, -1);
}

void raiseSymbolNotRecognizedException(char lookahead, int lineno) {
	char msg[100];
	ErrorType e = ERROR;
	strcpy(msg, "Symbol not expected here ");
	msg[25] = lookahead;
	msg[26] = '\0';
	strcat(msg, " ASCII (");
	int ascii = lookahead;
	itoa(ascii, msg+34, 10);
	strcat(msg, ")");
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
		return ++ptr;
	} else {
		int size = 0;
		if(start==0) {
			raiseBufferOverflowException();
		}
		strcpy(buf, buf+start);
		size=fread(buf+50-start, 1, start, fp);
		buf[50-start+size] = '\0';
		return 50-start;
	}
}

void lex(Queue tokenStream, FILE* fp) {
	char* buf = (char*) malloc(51);
	buf[50]='\0';
	if(!fread(buf, 1, 50, fp)) {
		return;
	}
	int state = 0;
	int lineno = 0;
	int start = 0;
	int ptr = 0;
	Data tokenData;
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
						case '\n': lineno++; start++; break;
						case '\0': return;
						case ' ': start++; break;
						case '\t': start++; break;
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
					buf[ptr] = '\0';
					tokenData.value = tokenize(ID, buf+start, lineno);
					enqueue(tokenStream, tokenData);
					buf[ptr]=lookahead;
					start = ptr;
					state=0;
					ptr--;
				}
				break;
			case 2:
				if(lookahead>='0' && lookahead<='9')
					state=2;
				else if(lookahead=='.')
					state=22;
				else {
					buf[ptr] = '\0';
					tokenData.value = tokenize(NUM, buf+start, lineno);
					enqueue(tokenStream, tokenData);
					buf[ptr]=lookahead;
					start = ptr;
					state=0;
					ptr--;
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
				buf[ptr] = '\0';
				tokenData.value = tokenize(PLUS, buf+start, lineno);
				enqueue(tokenStream, tokenData);
				buf[ptr]=lookahead;
				start = ptr;
				state=0;
				ptr--;
				break;
			case 6:
				buf[ptr] = '\0';
				tokenData.value = tokenize(MINUS, buf+start, lineno);
				enqueue(tokenStream, tokenData);
				buf[ptr]=lookahead;
				start = ptr;
				state=0;
				ptr--;
				break;
			case 7:
				buf[ptr] = '\0';
				tokenData.value = tokenize(MUL, buf+start, lineno);
				enqueue(tokenStream, tokenData);
				buf[ptr]=lookahead;
				start = ptr;
				state=0;
				ptr--;
				break;
			case 8:
				buf[ptr] = '\0';
				tokenData.value = tokenize(DIV, buf+start, lineno);
				enqueue(tokenStream, tokenData);
				buf[ptr]=lookahead;
				start = ptr;
				state=0;
				ptr--;
				break;
			case 9:
				if(lookahead=='=')
					state=25;
				else {
					buf[ptr] = '\0';
					tokenData.value = tokenize(LT, buf+start, lineno);
					enqueue(tokenStream, tokenData);
					buf[ptr]=lookahead;
					start = ptr;
					state=0;
					ptr--;
				}
				break;
			case 10:
				if(lookahead=='=')
					state=33;
				else {
					buf[ptr] = '\0';
					tokenData.value = tokenize(GT, buf+start, lineno);
					enqueue(tokenStream, tokenData);
					buf[ptr]=lookahead;
					start = ptr;
					state=0;
					ptr--;
				}
				break;
			case 11:
				switch(lookahead) {
					case '/': state=26; break;
					case '=': state=27; break;
					default:
						buf[ptr] = '\0';
						tokenData.value = tokenize(ASSIGNOP, buf+start, lineno);
						enqueue(tokenStream, tokenData);
						buf[ptr]=lookahead;
						start = ptr;
						state=0;
						ptr--;
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
				buf[ptr] = '\0';
				tokenData.value = tokenize(SIZE, buf+start, lineno);
				enqueue(tokenStream, tokenData);
				buf[ptr]=lookahead;
				start = ptr;
				state=0;
				ptr--;
				break;
			case 14:
				buf[ptr] = '\0';
				tokenData.value = tokenize(COMMA, buf+start, lineno);
				enqueue(tokenStream, tokenData);
				buf[ptr]=lookahead;
				start = ptr;
				state=0;
				ptr--;
				break;
			case 15:
				buf[ptr] = '\0';
				tokenData.value = tokenize(SQO, buf+start, lineno);
				enqueue(tokenStream, tokenData);
				buf[ptr]=lookahead;
				start = ptr;
				state=0;
				ptr--;
				break;
			case 16:
				buf[ptr] = '\0';
				tokenData.value = tokenize(SQC, buf+start, lineno);
				enqueue(tokenStream, tokenData);
				buf[ptr]=lookahead;
				start = ptr;
				state=0;
				ptr--;
				break;
			case 17:
				buf[ptr] = '\0';
				tokenData.value = tokenize(OP, buf+start, lineno);
				enqueue(tokenStream, tokenData);
				buf[ptr]=lookahead;
				start = ptr;
				state=0;
				ptr--;
				break;
			case 18:
				buf[ptr] = '\0';
				tokenData.value = tokenize(CL, buf+start, lineno);
				enqueue(tokenStream, tokenData);
				buf[ptr]=lookahead;
				start = ptr;
				state=0;
				ptr--;
				break;
			case 19:
				buf[ptr] = '\0';
				tokenData.value = tokenize(SEMICOLON, buf+start, lineno);
				enqueue(tokenStream, tokenData);
				buf[ptr]=lookahead;
				start = ptr;
				state=0;
				ptr--;
				break;
			case 20:
				if(lookahead=='\n')
					state=0;
				else
					state=20;
				break;
			case 21:
				buf[ptr] = '\0';
				tokenData.value = tokenize(ID, buf+start, lineno);
				enqueue(tokenStream, tokenData);
				buf[ptr]=lookahead;
				start = ptr;
				state=0;
				ptr--;
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
					buf[ptr] = '\0';
					tokenData.value = tokenize(FUNID, buf+start, lineno);
					enqueue(tokenStream, tokenData);
					buf[ptr]=lookahead;
					start = ptr;
					state=0;
					ptr--;
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
				buf[ptr] = '\0';
				tokenData.value = tokenize(LE, buf+start, lineno);
				enqueue(tokenStream, tokenData);
				buf[ptr]=lookahead;
				start = ptr;
				state=0;
				ptr--;
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
				buf[ptr] = '\0';
				tokenData.value = tokenize(EQ, buf+start, lineno);
				enqueue(tokenStream, tokenData);
				buf[ptr]=lookahead;
				start = ptr;
				state=0;
				ptr--;
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
				buf[ptr] = '\0';
				tokenData.value = tokenize(STR, buf+start, lineno);
				enqueue(tokenStream, tokenData);
				buf[ptr]=lookahead;
				start = ptr;
				state=0;
				ptr--;
				break;
			case 33:
				buf[ptr] = '\0';
				tokenData.value = tokenize(GE, buf+start, lineno);
				enqueue(tokenStream, tokenData);
				buf[ptr]=lookahead;
				start = ptr;
				state=0;
				ptr--;
				break;
			case 34:
				buf[ptr] = '\0';
				tokenData.value = tokenize(NE, buf+start, lineno);
				enqueue(tokenStream, tokenData);
				buf[ptr]=lookahead;
				start = ptr;
				state=0;
				ptr--;
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
				buf[ptr] = '\0';
				tokenData.value = tokenize(RNUM, buf+start, lineno);
				enqueue(tokenStream, tokenData);
				buf[ptr]=lookahead;
				start = ptr;
				state=0;
				ptr--;
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
				buf[ptr] = '\0';
				tokenData.value = tokenize(OR, buf+start, lineno);
				enqueue(tokenStream, tokenData);
				buf[ptr]=lookahead;
				start = ptr;
				state=0;
				ptr--;
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
				buf[ptr] = '\0';
				tokenData.value = tokenize(AND, buf+start, lineno);
				enqueue(tokenStream, tokenData);
				buf[ptr]=lookahead;
				start = ptr;
				state=0;
				ptr--;
				break;
			case 43:
				buf[ptr] = '\0';
				tokenData.value = tokenize(NOT, buf+start, lineno);
				enqueue(tokenStream, tokenData);
				buf[ptr]=lookahead;
				start = ptr;
				state=0;
				ptr--;
				break;
		};
		ptr = incrementptr(buf, ptr, start, fp);
		if(ptr<start) {
			start=0;
		}
	}
}

Queue read(char* filename) {
	Queue tokenStream = createQueue();
	FILE* fp = fopen(filename, "r");
	if(fp==NULL) {
		raiseFileCorruptException();
		return NULL;
	}
	lex(tokenStream, fp);
	return tokenStream;
}

void main() {
	Queue q = read("test");
	int i;
	while(q->size!=0) {
		Data d = dequeue(q);
		printf("%d\n", d.value->type);
	}
}
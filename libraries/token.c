#include"token.h"
#include"error.h"
#include"trie.h"
#include<stdlib.h>
#include<stdio.h>
#include<string.h>

void raiseIdentifierSizeExceededException(Token* token) {
	char msg[100];
	ErrorType e = ERROR;
	strcpy(msg, "Exceeded maximum idenitifier size for identifier ");
	strcat(msg, token->value.lexeme);
	error(msg, e, token->lineno);
}

void raiseStringSizeExceededException(Token* token) {
	char msg[100];
	ErrorType e = ERROR;
	strcpy(msg, "Exceeded maximum idenitifier size for string ");
	strcat(msg, token->value.string->value);
	error(msg, e, token->lineno);
}

Token* tokenizeId(char* buf, int lineno) {
	int bufsize = strlen(buf);
	Token* token = (Token*) malloc(sizeof(Token));
	TokenType t = (TokenType) findInTrie(keywords, buf);
	if(t!=-1) {
		token->type = t;
	} else {
		token->type = ID;
	}
	token->value.lexeme = (char*) malloc(bufsize + 1);
	strcpy(token->value.lexeme, buf);
	token->lineno = lineno;
	if(bufsize>20)
		raiseIdentifierSizeExceededException(token);
	return token;
}

Token* tokenizeFunId(char* buf,int lineno) {
	Token* token = (Token*) malloc(sizeof(Token));
	token->value.lexeme = (char*) malloc(strlen(buf) + 1);
	strcpy(token->value.lexeme, buf);
	token->lineno = lineno;
	if(strcmp(buf, "_main")==0) {
		token->type = MAIN;
	} else {
		token->type = FUNID;
	}
	return token;
}

Token* tokenizeNum(char* buf, int lineno) {
	Token* token = (Token*) malloc(sizeof(Token));
	token->type = NUM;
	token->value.integer = atoi(buf);
	token->lineno = lineno;
	return token;
}

Token* tokenizeRNum(char* buf, int lineno) {
	Token* token = (Token*) malloc(sizeof(Token));
	token->type = RNUM;
	token->value.real = atof(buf);
	token->lineno = lineno;
	return token;
}

Token* tokenizeStr(char* buf, int lineno) {
	Token* token = (Token*) malloc(sizeof(Token));
	token->type = STR;
	buf[strlen(buf)-1] = '\0';
	token->value.string = (String*) malloc(sizeof(String));
	token->value.string->size = strlen(buf) - 2;
	token->value.string->value = (char*) malloc(token->value.string->size+1);
	strcpy(token->value.string->value, buf + 1);
	token->lineno = lineno;
	if(token->value.string->size>20)
		raiseStringSizeExceededException(token);
	return token;
}

void getLexeme(Token* token, char* lexeme) {
	switch(token->type) {
		case NUM: sprintf(lexeme, "%d", token->value.integer); break;
		case RNUM: sprintf(lexeme, "%f", token->value.real); break;
		case STR:  sprintf(lexeme, "\"%s\"", token->value.string->value); break;
		default: strcpy(lexeme, token->value.lexeme);
	}
	return;
}

Token* tokenize(TokenType type, char* buf, int lineno) {
	Token* token;
	switch(type) {
		case ID: return tokenizeId(buf, lineno);
		case FUNID: return tokenizeFunId(buf, lineno);
		case NUM: return tokenizeNum(buf, lineno);
		case RNUM: return tokenizeRNum(buf, lineno);
		case STR: return tokenizeStr(buf, lineno);
		default: 1+1;
			token = (Token*) malloc(sizeof(Token));
			token->type = type;
			token->value.lexeme = (char*) malloc(strlen(buf));
			strcpy(token->value.lexeme, buf);
			token->lineno = lineno;
			return token;
	};
}

void initializeTokenizer() {
	keywords = makeTrie(TRIE_CASE_SENSITIVE);
	TokenType t;
	t = END;
	insertInTrie(keywords, "end", t);
	t = INT;
	insertInTrie(keywords, "int", t);
	t = REAL;
	insertInTrie(keywords, "real", t);
	t = STRING;
	insertInTrie(keywords, "string", t);
	t = MATRIX;
	insertInTrie(keywords, "matrix", t);
	t = IF;
	insertInTrie(keywords, "if", t);
	t = ELSE;
	insertInTrie(keywords, "else", t);
	t = ENDIF;
	insertInTrie(keywords, "endif", t);
	t = READ;
	insertInTrie(keywords, "read", t);
	t = PRINT;
	insertInTrie(keywords, "print", t);
	t = FUNCTION;
	insertInTrie(keywords, "function", t);
}

Trie getTokenMapping() {
	Trie mapping = makeTrie(TRIE_CASE_INSENSITIVE);
	insertInTrie(mapping, "ASSIGNOP", ASSIGNOP);
	insertInTrie(mapping, "COMMENT", COMMENT);
	insertInTrie(mapping, "FUNID", FUNID);
	insertInTrie(mapping, "ID", ID);
	insertInTrie(mapping, "NUM", NUM);
	insertInTrie(mapping, "RNUM", RNUM);
	insertInTrie(mapping, "STR", STR);
	insertInTrie(mapping, "END", END);
	insertInTrie(mapping, "INT", INT);
	insertInTrie(mapping, "REAL", REAL);
	insertInTrie(mapping, "STRING", STRING);
	insertInTrie(mapping, "MATRIX", MATRIX);
	insertInTrie(mapping, "MAIN", MAIN);
	insertInTrie(mapping, "SQO", SQO);
	insertInTrie(mapping, "SQC", SQC);
	insertInTrie(mapping, "OP", OP);
	insertInTrie(mapping, "CL", CL);
	insertInTrie(mapping, "SEMICOLON", SEMICOLON);
	insertInTrie(mapping, "COMMA", COMMA);
	insertInTrie(mapping, "IF", IF);
	insertInTrie(mapping, "ELSE", ELSE);
	insertInTrie(mapping, "ENDIF", ENDIF);
	insertInTrie(mapping, "READ", READ);
	insertInTrie(mapping, "PRINT", PRINT);
	insertInTrie(mapping, "FUNCTION", FUNCTION);
	insertInTrie(mapping, "PLUS", PLUS);
	insertInTrie(mapping, "MINUS", MINUS);
	insertInTrie(mapping, "MUL", MUL);
	insertInTrie(mapping, "DIV", DIV);
	insertInTrie(mapping, "SIZE", SIZE);
	insertInTrie(mapping, "AND", AND);
	insertInTrie(mapping, "OR", OR);
	insertInTrie(mapping, "NOT", NOT);
	insertInTrie(mapping, "LT", LT);
	insertInTrie(mapping, "LE", LE);
	insertInTrie(mapping, "EQ", EQ);
	insertInTrie(mapping, "GT", GT);
	insertInTrie(mapping, "GE", GE);
	insertInTrie(mapping, "NE", NE);
	return mapping;
}

// void main() {
// 	char* str = (char*) malloc(50);
// 	scanf("%s", str);
// 	Token* t = tokenize(STR, str, 1);
// 	printf("%s", t->value.string->value);
// }
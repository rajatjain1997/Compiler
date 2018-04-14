all: compiler.o lexer.o parser.o list.o error.o grammar.o set.o symbol.o token.o tree.o trie.o ast.o semantic.o symboltable.o type.o quadruple.o intermediate.o
	gcc -o stage1exe compiler.o lexer.o parser.o list.o error.o grammar.o set.o symbol.o token.o trie.o tree.o ast.o semantic.o symboltable.o type.o intermediate.o quadruple.o

stage1: compiler.o lexer.o parser.o list.o error.o grammar.o set.o symbol.o token.o tree.o trie.o
	gcc -o stage1exe compiler.o lexer.o parser.o list.o error.o grammar.o set.o symbol.o token.o trie.o tree.o

compiler.o: lexer.h parser.h error.h list.h tree.h symbol.h token.h grammar.h compiler.c
	gcc -c compiler.c

lexer.o: lexer.h lexer.c list.h token.h error.h
	gcc -c lexer.c

parser.o: parser.h parser.c set.h grammar.h symbol.h tree.h error.h token.h list.h
	gcc -c parser.c

ast.o: ast.h ast.c tree.h token.h symbol.h list.h trie.h semantic.h grammar.h
	gcc -c ast.c

symboltable.o: symboltable.c symboltable.h list.h token.h symbol.h tree.h
	gcc -c symboltable.c

type.o: type.c type.h tree.h symbol.h token.h list.h semantic.h symboltable.h error.h
	gcc -c type.c

quadruple.o: quadruple.c quadruple.h
	gcc -c quadruple.c

intermediate.o: intermediate.c intermediate.h
	gcc -c intermediate.c

semantic.o:  semantic.h semantic.c tree.h token.h symbol.h list.h trie.h semantic.h grammar.h
	gcc -c semantic.c

list.o: list.c list.h
	gcc -c list.c

error.o: list.h error.h error.c
	gcc -c error.c

grammar.o: grammar.h grammar.c token.h symbol.h error.h trie.h list.h set.h
	gcc -c grammar.c

set.o: set.c set.h
	gcc -c set.c

symbol.o: symbol.h symbol.c token.h
	gcc -c symbol.c

token.o: token.h token.c trie.h error.h
	gcc -c token.c

tree.o: tree.c tree.h list.h
	gcc -c tree.c

trie.o: trie.c trie.h
	gcc -c trie.c

remove: all
	rm *.o
	rm stage1exe

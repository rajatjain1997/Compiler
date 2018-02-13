#include "./modules/lexer.h"
#include "./libraries/token.h"
#include "./libraries/error.h"
#include "./libraries/list.h"
#include "./libraries/tree.h"
#include <stdio.h>

int main(int argc, char* argv[]) {
	initializeError(argv[1]);
	Queue tokenstream = read(argv[1]);
	Tree parsetree = parse(tokenstream, "grammar.txt");
	return 0;
}
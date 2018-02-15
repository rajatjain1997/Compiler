#include "./modules/lexer.h"
#include "./modules/parser.h"
#include "./libraries/token.h"
#include "./libraries/error.h"
#include "./libraries/list.h"
#include "./libraries/tree.h"
#include <stdio.h>
#include <string.h>

int main(int argc, char* argv[]) {
	int testing = 1;
	if(argc==3) {
		testing = strcmp(argv[2], "-test");
	}
	initializeError(argv[1], !testing);
	Queue tokenstream = read(argv[1]);
	Tree parsetree = parse(tokenstream, "grammar.txt");
	return 0;
}
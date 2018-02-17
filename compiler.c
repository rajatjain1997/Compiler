#include "./modules/lexer.h"
#include "./modules/parser.h"
#include "./libraries/token.h"
#include "./libraries/error.h"
#include "./libraries/list.h"
#include "./libraries/tree.h"
#include <stdio.h>
#include <string.h>

int main(int argc, char* argv[]) {
	int testing = 1; int cleaning  = 1; int i;
	char cleanDest[20];
	for(i=2; i<argc; i++) {
		testing = strcmp(argv[i], "-test");
		cleaning = strcmp(argv[i], "-clean");
		if(cleaning==0) {
			strcpy(cleanDest, argv[i+1]);
			i++;
		}
	}
	initializeError(argv[1], !testing);
	Queue tokenstream = read(argv[1]);
	if(!cleaning) {
		clean(tokenstream, cleanDest);
	}
	Tree parsetree = parse(tokenstream, "grammar.txt");
	return 0;
}
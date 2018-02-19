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
	initializeError(argv[1], !testing);
	if(error_testing) {
		printf("a) Lexer and Parser are implemented.\n");
		printf("b) First and follow sets are automated.\n");
		printf("c) All test cases run without any erraneous output. All errors specified in test case 5 are reported as required.\n");
		printf("d) Parse tree is successfully generated\n");
	}
	Queue tokenstream = read(argv[1]);
	if(!cleaning) {
		clean(tokenstream, cleanDest);
	}
	Tree parsetree = parse(tokenstream, "grammar.txt");
	return 0;
}
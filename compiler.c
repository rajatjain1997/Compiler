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
			if(i+1<argc) {
				strcpy(cleanDest, argv[i+1]);
			} else {
				strcpy(cleanDest, "tempOutFile");
			}
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
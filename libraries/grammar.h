struct head;
struct set;

typedef struct nonTerminal {
	int symbolType;
	struct head* rules;
	struct head* occurances;
	struct set* first;
	struct set* follow;
} NonTerminal;

struct grammar {
	NonTerminal NonTerminals[50];
	int size;
};

typedef struct grammar* Grammar;

Grammar readGrammar(char* filename);
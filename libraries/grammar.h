struct head;

typedef struct nonTerminal {
	int symbolType;
	struct head* rules;
	struct head* occurances;
} NonTerminal;

struct grammar {
	NonTerminal NonTerminals[50];
	int size;
};

typedef struct grammar* Grammar;
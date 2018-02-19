struct head;
struct node;
struct set;

typedef struct nonTerminal {
	int symbolType;
	struct head* rules;
	struct head* occurances;
	struct set* first;
	struct set* follow;
} NonTerminal;

struct occurance {
	struct node* node;
	int owner;
};

typedef struct occurance* Occurance;

struct grammar {
	NonTerminal NonTerminals[50];
	int size;
};

typedef struct grammar* Grammar;

extern char nonTerminalStrings[50][30];

Grammar readGrammar(char* filename);
void freeGrammar(Grammar g);
/**
 *	AUTHOR: Rajat Jain
 *  ID No. 2015A7PS0549P
 */

struct head;
struct node;
struct set;
struct trie;

#define MAX_GRAMMAR_SIZE 50

typedef struct rule {
		struct head* rule;
		int index;
} Rule;

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
	NonTerminal NonTerminals[MAX_GRAMMAR_SIZE];
	int size;
};

typedef struct grammar* Grammar;

extern char nonTerminalStrings[50][30];
extern struct trie* nonTerminalMapping;

Grammar readGrammar(char* filename);
void freeGrammar(Grammar g);
struct head* extractRule(Rule* rule);
int getRuleIndex(Rule* rule);

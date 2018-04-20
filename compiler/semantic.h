/**
 *	AUTHOR: Rajat Jain
 *  ID No. 2015A7PS0549P
 */

struct tree;
struct symbol;

struct symbol* lookupSymbolDictionary(char nonterminal[], int terminal);

struct tree* extractChild(struct tree* tree ,char nonterminal[], TokenType terminal, int childno);

struct tree* extractChildNumber(struct tree* tree, int childno);

int sizeLookup(int type);

int symbolComparatorNT(struct symbol* s, char str[]);

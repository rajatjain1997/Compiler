/**
 *	AUTHOR: Rajat Jain
 *  ID No. 2015A7PS0549P
 */

struct symbol;
struct head;

#define ATTR_NOS 3

struct tree {
	struct symbol* symbol;
	struct head* children;
	struct tree* parent;
	void* attr[ATTR_NOS];
};

typedef struct tree* Tree;

Tree createTree(struct symbol* root);
Tree add_child(Tree tree, struct symbol* symbol);
void nullifyAttributes(Tree tree);
void freeTree(Tree tree, int freeAttr);
struct symbol* extractSymbol(Tree tree);

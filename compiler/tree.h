struct symbol;

struct head;

struct tree {
	struct symbol* symbol;
	struct head* children;
	struct tree* parent;
};

typedef struct tree* Tree;

Tree createTree(struct symbol* root);
Tree add_child(Tree tree, struct symbol* symbol);
struct symbol* extractSymbol(Tree tree);
void DFT(Tree tree);
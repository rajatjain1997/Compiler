struct symbol;

struct head;

struct tree {
	struct symbol* symbol;
	struct head* children;
};

typedef struct tree* Tree;

Tree createTree(struct symbol* root);
void add_child(Tree tree, struct symbol* symbol);
void DFT(Tree tree);
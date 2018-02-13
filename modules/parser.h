#define EPSILON 63

struct tree;
struct queue;

struct tree* parse(struct queue* tokenstream, char* grammarfile);
/**
 *	AUTHOR: Rajat Jain
 *  ID No. 2015A7PS0549P
 */

#define EPSILON 63

struct tree;
struct queue;

struct tree* parse(struct queue* tokenstream, char* grammarfile);

/**
 *	AUTHOR: Rajat Jain
 *  ID No. 2015A7PS0549P
 */

#include <stdint.h>
struct set{
	uint64_t vector;
};

typedef struct set* Set;

Set createSet();
void putInSet(Set set, int value);
int getFromSet(Set set, int value);
Set setUnion(Set s1, Set s2);
Set intersection(Set s1, Set s2);
Set compliment(Set s);
Set difference(Set s1, Set s2);
int isEmpty(Set s);
void freeSet(Set s);

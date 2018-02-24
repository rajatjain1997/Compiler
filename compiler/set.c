/**
 *	AUTHOR: Rajat Jain
 *  ID No. 2015A7PS0549P
 */

#include"set.h"
#include<stdio.h>
#include<stdlib.h>

Set createSet() {
	Set bv = (Set) malloc(sizeof(Set));
	bv->vector = 0;
	return bv;
}

void putInSet(Set bv, int j) {
	 bv->vector = bv->vector|(((uint64_t)1)<<j);
}

int getFromSet(Set bv, int j) {
	if(bv->vector&(((uint64_t)1)<<j)) {
		return 1;
	}
	return 0;
}

Set setUnion(Set s1, Set s2) {
	Set result = createSet();
	result->vector = s1->vector | s2->vector;
	return result;
}

Set intersection(Set s1, Set s2) {
	Set result = createSet();
	result->vector = s1->vector & s2->vector;
	return result;
}

Set compliment(Set s) {
	Set result = createSet();
	result->vector = ~(s->vector);
	return result;
}

Set difference(Set s1, Set s2) {
	return intersection(s1, compliment(s2));
}

int isEmpty(Set s) {
	return s->vector == 0;
}

void freeSet(Set s) {
	free(s);
}

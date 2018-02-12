#include"set.h"
#include<stdio.h>
#include<stdlib.h>

Set createSet() {
	Set bv = (Set) malloc(sizeof(Set));
	bv->vector = 0;
	return bv;
}

void putInSet(Set bv, int j) {
	 bv->vector = bv->vector|(1<<j);
}

int getFromSet(Set bv, int j) {
	if(bv->vector&(1<<j)) {
		return 1;
	}
	return 0;
}

Set union(Set s1, Set s2) {
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
	result->vector = ~s->vector;
	return s;
}

Set difference(Set s1, Set s2) {
	return intersection(s1, compliment(s2));
}
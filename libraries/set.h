typedef struct set{
	uint64_t vector;
};

typedef struct set* Set;

Set createSet();
void putInSet(Set set, int value);
int getFromSet(Set set, int value);
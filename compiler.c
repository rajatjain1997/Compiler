#include "./modules/lexer.h"
#include "./libraries/token.h"
#include "./libraries/error.h"
#include "./libraries/list.h"
#include <stdio.h>

int main(int argc, char* argv[]) {
	initializeError(argv[1]);
	Queue q = read(argv[1]);
	while(q->size!=0) {
		QueueData d = dequeue(q);
		printf("%d\n", d.value->type);
	}
	return 0;
}
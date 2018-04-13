/**
 *	AUTHOR: Rajat Jain
 *  ID No. 2015A7PS0549P
 */

#include<stdio.h>
#include<stdlib.h>
#include"list.h"


//Linked List Methods Start

Head* createList() {
	Head* h = (Head*) malloc(sizeof(Head));
	h->first = NULL;
	h->last = NULL;
	h->size = 0;
	return h;
}

void freeList(List h) {
	Element temp = h->first, prev;
	while(temp!=NULL) {
		prev = temp;
		temp = temp->next;
		free(prev);
	}
	free(h);
}


Node* traverse(Head* h, int index) {
	if(index>h->size||index<0) {
		return NULL;
	}
	Node* temp = h->first;
	int count=0;
	while(count<index) {
		temp=temp->next;
		count++;
	}
	return temp;
}

Data get(Head* h, int index) {
	return traverse(h,index)->data;
}

void insertInFront(Head* h, Data data) {
	Node* temp = (Node*) malloc(sizeof(Node));
	temp -> data = data;
	if(h->size==0) {
		temp->next = NULL;
		temp->prev = NULL;
		h->first = temp;
		h->last = temp;
		h->size++;
		return;
	}
	temp -> next = h->first;
	temp -> prev = NULL;
	h->first->prev = temp;
	h->first = temp;
	h->size++;
	return;
}

void insertAtEnd(Head* h, Data data) {
	Node* temp = (Node*) malloc(sizeof(Node));
	temp -> data = data;
	if(h->size==0) {
		h->size++;
		temp -> next = NULL;
		temp -> prev = NULL;
		h->first = temp;
		h->last = temp;
		return;
	}
	temp -> next = NULL;
	temp -> prev = h->last;
	h->last->next = temp;
	h->last = temp;
	h->size++;
	return;
}

void insertAtIndex(Head* h, Data data, int index) {
	if(index>h->size) {
		return;
	} else if(index==0) {
		insertInFront(h,data);
		return;
	} else if(index==h->size) {
		insertAtEnd(h,data);
		return;
	}
	Node* temp = (Node*) malloc(sizeof(Node));
	temp -> data = data;
	int count = 0;
	Node* traverser = h->first;
	while(++count<index && traverser!=NULL) {
		traverser = traverser -> next;
	}
	h->size++;
	temp->next = traverser->next;
	temp->prev = traverser;
	traverser->next = temp;
	temp->next->prev = temp;
}

void printList(Head* h) {
	Node* temp = h->first;
	while(temp!=NULL) {
		printf("%s\t",temp->data.value);
		temp=temp->next;
	}
	printf("\n");
}

void insertAtEndFast(List list, void* element) {
  if(element==NULL) {
    return;
  }
  Data d;
  d.value.tree = (List) element;
  insertAtEnd(list, d);
}

void insertInFrontFast(List list, void* element) {
  if(element==NULL) {
    return;
  }
  Data d;
  d.value.tree = (List) element;
  insertInFront(list, d);
}

Data deleteAtEnd(Head* h) {
	if(h->size==0) {
		printf("-2\n");
		return;
	} else if(h->size==1) {
		h->size--;
		Data deletion = h->first->data;
		free(h->first);
		h->first=NULL;
		h->last=NULL;
		return deletion;
	}
	Data deletion = h->last->data;
	Node* delete = h->last;
	h->last->prev->next = NULL;
	h->last = h->last->prev;
	h->size--;
	free(delete);
	return deletion;
}

Data deleteFromFront(Head* h) {
	if(h->size==0) {
		printf("-2\n");
		return;
	} else if(h->size==1) {
		Data deletion = h->first->data;
		Node* delete = h->first;
		h->first = NULL;
		h->size--;
		free(delete);
		return deletion;
	}
	Data deletion = h->first->data;
	Node* delete = h->first;
	h->first->next->prev = NULL;
	h->first = h->first->next;
	h->size--;
	free(delete);
	return deletion;
}

Data deleteAtIndex(Head* h, int index) {
	if(index>h->size) {
		return;
	} else if(index==0) {
		return deleteFromFront(h);
	} else if(index==h->size-1) {
		return deleteAtEnd(h);
	}
	int count = 0;
	Node* temp = h->first;
	while(count!=index-1) {
		temp=temp->next;
	}
	Data deletion = temp->next->data;
	Node* delete = temp->next;
	temp->next = delete->next;
	delete->next->prev = temp;
	free(delete);
	return deletion;
}


int checkCircular(Head* h) {
	if(h->size==0) {
		return 0;
	}
	Node* hare = h->first;
	Node* tort = h->first;
	int circular = 0;
	while(hare->next!=NULL&&hare->next->next!=NULL) {
		hare= hare->next->next;
		tort= tort->next;
		if(hare==tort||hare->next==tort) {
			circular = 1;
			hare=tort;
			break;
		}
	}
	if(circular) {
		hare = hare->next;
		while(hare!=tort) {
			circular++;
			hare=hare->next;
		}
		circular++;
		return circular;
	}
	return 0;
}

List appendLists(List list1, List list2) {
	list1->last->next = list2->first;
	list2->first->prev = list1->last;
	list1->last = list2->last;
	free(list2);
	return list1;
}

//Linked List Methods End
//Queue Methods Start

Queue createQueue() {
	Queue q = (Queue) malloc(sizeof(queue));
	q->first = NULL;
	q->last = NULL;
	q->size = 0;
	return q;
}

void freeQueue(Queue h) {
	QueueElement temp = h->first, prev;
	while(temp!=NULL) {
		prev = temp;
		temp = temp->next;
		free(prev);
	}
	free(h);
}

void enqueue(Queue q, QueueData d) {
	QueueElement ele = (QueueElement) malloc(sizeof(QueueNode));
	ele->data = d;
	ele->next = NULL;
	if(q->size==0) {
		q->size++;
		q->first = ele;
		q->last = ele;
		return;
	}
	q->size++;
	q->last->next = ele;
	q->last = ele;
	return;
}

QueueData dequeue(Queue q) {
	if(q->size==0) {
		return;
	}
	QueueData d = q->first->data;
	q->size--;
	if(q->size==0) {
		free(q->last);
		q->first = NULL;
		q->last = NULL;
	} else {
		QueueElement temp = q->first->next;
		free(q->first);
		q->first = temp;
	}
	return d;
}

QueueData getFirst(Queue q) {
	if(q->size==0) {
		return;
	}
	return q->first->data;
}

//Queue Methods End
//Stack Methods Start

Stack createStack() {
	Stack stack = (Stack) malloc(sizeof(StackHead));
	stack->size = 0;
	stack->top = NULL;
	return stack;
}

void freeStack(Stack h) {
	StackElement temp = h->top, prev;
	while(temp!=NULL) {
		prev = temp;
		temp = temp->next;
		free(prev);
	}
	free(h);
}

void push(Stack stack, StackData d) {
	StackElement ele = (StackElement) malloc(sizeof(Node));
	ele->data = d;
	if(stack->size==0) {
		ele->next = NULL;
		stack->top = ele;
		stack->size++;
		return;
	}
	ele->next = stack->top;
	stack->top = ele;
	stack->size++;
	return;
}

StackData pop(Stack stack) {
	if(stack->size==0) {
		return;
	}
	StackData deletion = stack->top->data;
	StackElement delete = stack->top;
	stack->top = stack->top->next;
	stack->size--;
	free(delete);
	return deletion;
}

StackData top(Stack stack) {
	if(stack->size==0) {
		return;
	}
	return stack->top->data;
}

//Stack Methods End

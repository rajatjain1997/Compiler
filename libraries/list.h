struct Token;

typedef struct {
	struct Token* value;
} Data;

struct node{
	Data data;
	struct node* next;
};
typedef struct node Node;
typedef Node* Element;

//Linked List Data Definition Start

struct head{
	int size;
	Node* first;
};
typedef struct head Head;
typedef Head* List;

//Linked List Data Definition Ends
//Queue Data Definition Start

typedef struct 
{
	Element first, last;
	int size;
} queue;
typedef queue* Queue;

//Queue Data Definition Ends
//Stack Data Definition Start

typedef struct {
	Element top;
	int size;
} StackHead;
typedef StackHead* Stack;

//Stack Data Definition Ends
//Data Definition Ends
//Linked List Methods Start

Head* createList();

Data get(Head*, int);

void printList(Head*);

void insertInFront(Head*, Data);

void insertAtEnd(Head*, Data);

void insertAtIndex(Head*, Data, int);

Data deleteFromFront(Head*);

Data deleteAtEnd(Head*);

Data deleteAtIndex(Head*, int);

int contains(Head*, Data);

int checkCircular(Head*);

//Linked List Methods End
//Queue Methods Start

Queue createQueue();

void enqueue(Queue,Data);

Data dequeue(Queue);

Data getFirst(Queue);

//Queue Methods End
//Stack Methods Start

Stack createStack();

void push(Stack,Data);

Data pop(Stack);

Data top(Stack);

//Stack Methods End

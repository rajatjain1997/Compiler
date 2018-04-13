/**
 *	AUTHOR: Rajat Jain
 *  ID No. 2015A7PS0549P
 */


//Linked List Data Definition Start

struct symbol;
struct head;
struct tree;
struct occurance;
struct errorMessage;
struct rule;
struct symbolTableEntry;
struct quadruple;

typedef union {
	struct tree* tree;
	struct head* list;
	struct symbol* symbol;
	struct occurance* occurance;
	struct errorMessage* errorMessage;
	struct rule* rule;
	struct symbolTableEntry* symboltableentry;
	struct quadruple* quadruple;
	struct address* address;
} LinkedDataItems;

typedef struct {
	LinkedDataItems value;
} Data;

struct node{
	Data data;
	struct node* next;
	struct node* prev;
};
typedef struct node Node;
typedef Node* Element;

struct head{
	int size;
	Node* first;
	Node* last;
};
typedef struct head Head;
typedef Head* List;

//Linked List Data Definition Ends
//Queue Data Definition Start

struct Token;

typedef struct {
	struct Token* value;
} QueueData;

struct Queuenode{
	QueueData data;
	struct Queuenode* next;
};

typedef struct Queuenode QueueNode;
typedef QueueNode* QueueElement;

typedef struct queue
{
	QueueElement first, last;
	int size;
} queue;
typedef queue* Queue;

//Queue Data Definition Ends
//Stack Data Definition Start

typedef struct {
	struct symbol* symbol;
	struct tree* symbolTree;
} StackSymbol;

typedef union {
	StackSymbol stackSymbol;
	Element element;
} StackDataItems;

typedef struct {
	StackDataItems value;
} StackData;

struct Stacknode{
	StackData data;
	struct Stacknode* next;
};

typedef struct Stacknode StackNode;
typedef StackNode* StackElement;

typedef struct {
	StackElement top;
	int size;
} StackHead;
typedef StackHead* Stack;

//Stack Data Definition Ends
//Data Definition Ends
//Linked List Methods Start

Head* createList();

void freeList(List h);

Data get(Head*, int);

void printList(Head*);

void insertInFront(Head*, Data);

void insertAtEnd(Head*, Data);

void insertAtIndex(Head*, Data, int);

void insertAtEndFast(List list, void* element);

void insertInFrontFast(List list, void* element);

Data deleteFromFront(Head*);

Data deleteAtEnd(Head*);

Data deleteAtIndex(Head*, int);

int checkCircular(Head*);

List appendLists(List list1, List list2);

//Linked List Methods End
//Queue Methods Start

Queue createQueue();

void freeQueue(Queue h);

void enqueue(Queue, QueueData);

QueueData dequeue(Queue);

QueueData getFirst(Queue);

//Queue Methods End
//Stack Methods Start

Stack createStack();

void freeStack(Stack h);

void push(Stack,StackData);

StackData pop(Stack);

StackData top(Stack);

//Stack Methods End

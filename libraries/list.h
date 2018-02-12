
//Linked List Data Definition Start

struct symbol;
struct head;
struct tree;
struct occurance;

typedef union {
	struct tree* tree;
	struct head* list;
	struct symbol* symbol;
	struct occurance* occurance;
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

Data get(Head*, int);

void printList(Head*);

void insertInFront(Head*, Data);

void insertAtEnd(Head*, Data);

void insertAtIndex(Head*, Data, int);

Data deleteFromFront(Head*);

Data deleteAtEnd(Head*);

Data deleteAtIndex(Head*, int);

int checkCircular(Head*);

//Linked List Methods End
//Queue Methods Start

Queue createQueue();

void enqueue(Queue, QueueData);

QueueData dequeue(Queue);

QueueData getFirst(Queue);

//Queue Methods End
//Stack Methods Start

Stack createStack();

void push(Stack,StackData);

StackData pop(Stack);

StackData top(Stack);

//Stack Methods End

struct trie {
	struct trie* level[27];
	int insensitive;
};

extern const int TRIE_CASE_SENSITIVE;
extern const int TRIE_CASE_INSENSITIVE;

typedef struct trie* Trie;

Trie makeTrie(int insensitive);

void insertInTrie(Trie trie, char* str, int info);

int findInTrie(Trie trie, char* str);

void deleteFromTrie(Trie trie, char* str);
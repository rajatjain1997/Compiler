struct trie {
	struct trie* level[27];
};

typedef struct trie* Trie;

Trie makeTrie();

void insertInTrie(Trie trie, char* str, int info);

int findInTrie(Trie trie, char* str);

void deleteFromTrie(Trie trie, char* str);
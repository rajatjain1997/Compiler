Symbol* lookupSymbolDictionary(char nonterminal[], int terminal);

Tree extractChild(Tree tree ,char nonterminal[], TokenType terminal, int childno);

Tree extractChildNumber(Tree tree, int childno);

int sizeLookup(int type);

int symbolComparatorNT(Symbol* s, char str[]);

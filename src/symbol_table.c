#include "common.h"
#include "syntax_tree.h"
#include "symbol_table.h"

//symbolHashTable use openhashing
SymbolNode *symbolHashTable[kHashSize];
SymbolStackNode *symbolStackHead = NULL;
//structTable is a linklist
SymbolNode *structTableHead = NULL;

//main function
void symbolTableMain(TreeNode *p){
	symbolStackHead = (SymbolStackNode *)malloc(sizeof(SymbolStackNode));
	symbolStackHead->symbolStackHead = NULL;
	symbolStackHead->funcptr = NULL;
	symbolStackHead->next = NULL;
	buildSymbolTable(p);
	return;
}

void buildSymbolTable(TreeNode *p){
//TODO: add leaf
	int i;
	for (i = 0; i < p->arity; i ++){
		buildSymbolTable(p->children[i]);
	}
	return;
}


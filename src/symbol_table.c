#include "common.h"
#include "syntax_tree.h"
#include "symbol_table.h"

//symbolHashTable use openhashing
SymbolNode *symbolHashTable[kHashSize];
SymbolStackNode *symbolStackHead = NULL;
//structTable is a linklist
SymbolNode *structTableHead = NULL;

void symbolTableMain(TreeNode *p){
	symbolStackHead = (SymbolStackNode *)malloc(sizeof(SymbolStackNode));
	symbolStackHead->symbolStackHead = NULL;
	symbolStackHead->funcptr = NULL;
	symbolStackHead->next = NULL;
	buildSymbolTable(p);
	return;
}


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
	if (strcmp(p->symbol, "ExtDef") == 0){
		buildExtDef(p);
		return;
	}
	if (strcmp(p->symbol, "Def") == 0){
		buildDef(p);
		return;
	}
	if (strcmp(p->symbol, "Stmt") == 0){
		buildStmt(p);
		return;
	}
	if (strcmp(p->symbol, "Exp") == 0){
		buildExp(p);
		return;
	}
	if (strcmp(p->symbol, "LC") == 0){
		pushSymbolStack();
	}
	if (strcmp(p->symbol, "RC") == 0){
		popSymbolStack();
	}
	int i;
	for (i = 0; i < p->arity; i ++){
		buildSymbolTable(p->children[i]);
	}
	return;
}


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

void buildExtDef(TreeNode *p){

}

void buildDef(TreeNode *p){

}

void buildStmt(TreeNode *p){

}

void buildExp(TreeNode *p){

}

void freeType(Type *p){
	if (p->type == BASIC) free(p);
	if (p->type == ARRAY){
		Type *temp = p;
		while(p->type != BASIC){
			p = p->array.element;
			free(temp);
			temp = p;
		}
		free(p);
	}
	if (p->type == STRUCT){
		while(p->structure){
			 StructContent *temp = p->structure;
			 p->structure = p->structure->next;
			 free(temp);
		}
		free(p);
	}
}
void pushSymbolStack(){
	//insert a node in the head of nodelist
	SymbolStackNode *newStackNode = (SymbolStackNode *)malloc(sizeof(SymbolStackNode));
	newStackNode->next = symbolStackHead;
	symbolStackHead = newStackNode;
	symbolStackhead->funcptr = NULL;
	symbolStackhead->symbolHead = NULL;
}

void popSymbolStack(){
	SymbolStackNode *deleteNode = symbolStackHead;
	symbolStackHead = symbolStackHead->next;
	while (deleteNode->symbolHead){
		SymbolNode *temp = deleteNode->symbolHead;
		if (temp->isfunc && temp->funcInfo.argumentNum > 0){
			free(temp->funcInfo.argumentType);
		}
		if (!temp->isfunc){
			freeType(temp->defInfo);
		}
		deleteNode->symbolHead = deleteNode->symbolHead->stackNext;
		free(temp);
	}
	free(deleteNode);
	return;
}

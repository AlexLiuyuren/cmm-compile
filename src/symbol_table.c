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

void symbolError(char errorType, TreeNode *p){
	switch(errorType) {
	case '1': printf("Error type 1 at line %d: Undefined variable \"%s\".\n", p->lineno, p->text); break;
	case '2': printf("Error type 2 at line %d: Undefined function \"%s\".\n", p->lineno, p->text); break;
	case '3': printf("Error type 3 at line %d: Redefined variable \"%s\".\n", p->lineno, p->text); break;
	case '4': printf("Error type 4 at line %d: Redefined function \"%s\".\n", p->lineno, p->text); break;
	case '5': printf("Error type 5 at line %d: Type mismatched for assignment.\n", p->lineno); break;
	case '6': printf("Error type 6 at line %d: The left-hand side of an assignment must be a variable.\n", p->lineno); break;
	case '7': printf("Error type 7 at line %d: Type mismatched for operands.\n", p->lineno); break;
	case '8': printf("Error type 8 at line %d: Type mismatched for return.\n", p->lineno); break;
	case '9': printf("Error type 9 at line %d: func is not applicable for arguments", p->lineno); break;
	case 'a': printf("Error type 10 at line %d: \"%s\" is not an array.\n", p->lineno, p->text); break;
	case 'b': printf("Error type 11 at line %d: \"%s\" is not a function.\n", p->lineno, p->text); break;
	case 'c': printf("Error type 12 at line %d: \"%s\" is not an integer.\n", p->lineno, p->text); break;
	case 'd': printf("Error type 13 at line %d: Illegal use of \".\".\n", p->lineno); break;
	case 'e': printf("Error type 14 at line %d: Non-existent field \"%s\".\n", p->lineno, p->text); break;
	case 'f': printf("Error type 15 at line %d: Redefined field \"%s\".\n", p->lineno, p->text); break;
	case 'g': printf("Error type 16 at line %d: Duplicated name \"%s\".\n", p->lineno, p->text); break;
	case 'h': printf("Error type 17 at line %d: Undefined structure \"%s\".\n", p->lineno, p->text); break;
	}
	return;
}
void buildExtDef(TreeNode *p){
	if (strcmp(p->children[1]->symbol, "ExtDefList") == 0){
		Type nodetype = buildSpecifier(p->children[0]);
		TreeNode *temp = p->children[1];
		while (temp->arity > 1){
			buildVarDec(nodetype, temp->children[0]);
			temp = temp->children[2];
		}
		buildVarDec(nodetype, temp->children[0]);
	}
	if (strcmp(p->children[1]->symbol, "SEMI") == 0){ 
		buildSpecifier(p->children[0]);
	}
	if (strcmp(p->children[1]->symbol, "FunDec") == 0){
		Type nodetype = buildSpecifier(p->children[0]);
		buildFunDec(nodetype, p->children[1]);
		int i;
		for (i = 1; i < p->children[2]->arity; i ++){
			buildSymbolTable(p->children[2]->children[i]);
		}
	}
	return;
}

//VarDec -> ID | VarDec LB INT RB
void buildVarDec(Type nodetype, TreeNode *p){
	if (p->arity == 1){
		if (searchSymbol(p->children[0]->text) != NULL){
			
		}
	}

}

void buildFunDec(Type nodetype, TreeNode *p){

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

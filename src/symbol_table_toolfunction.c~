#include "common.h"
#include "symbol_table.h"
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

int typeEqual(Type val1, Type val2)
{
	if (val1.type == NOTDEF || val2.type == NOTDEF) return -1;
	if (val1.type != val2.type) return 0;
	if (val1.type == BASIC) {
		if (val1.basic != val2.basic) return 0;
	}
	else if (val1.type == ARRAY) {
		if (val1.array.size != val2.array.size) return 0;
		if (typeEqual(*val1.array.element, *val2.array.element) == 0) return 0;
	}
	else {
		StructContent *list1 = val1.structure, *list2 = val2.structure;
		while (list1 != NULL && list2 != NULL) {
			if (typeEqual(*list1->type, *list2->type) == 0) return 0;
			list1 = list1->next, list2 = list2->next;
		}
		if (!(list1 == NULL && list2 == NULL)) return 0;
	}
	return 1;
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

unsigned int hash(const char *name)
{
	unsigned int val = 0, i;
	for (; *name; name++) {
		val = (val << 2) + *name;
		if (i = val & ~HASH_MASK)
			val = (val ^ (i >> 12)) & HASH_MASK;
	}
	return val;
}

SymbolNode *searchSymbolTable(const char *name)
{
	SymbolNode *temp = kSymbolHashTable[hash(name)]; // generate the hash slot
	for (; temp; temp = temp->hash_next) {
		if (strcmp(temp->name, name) == 0)
			return temp;
	}
	return NULL;
}

SymbolNode *searchStructTable(const char *name)
{
	SymbolNode *temp = kStructTableHead;
	for (; temp; temp = temp->StackNext) {
		if (strcmp(temp->name, name) == 0)
			return temp;
	}
	return NULL;
}

StructContent *searchStructContent(StructContent *structure, const char *name)
{
	StructContent *temp = structure;
	for (; temp; temp = temp->next) {
		if (strcmp(temp->name, name) == 0)
			return temp;
	}
	return NULL;
}
// Return a SymbolNode according to name
SymbolNode *addSymbol(const char *name)
{
	unsigned int hash_num = hash(name);
	SymbolNode *hash_slot = kSymbolHashTable[hash_num];
	SymbolNode *stack_slot = kSymbolStackHead->symbol_head;
	SymbolNode *new_node = (SymbolNode *) malloc(sizeof(SymbolNode));
	
	if (hash_slot == NULL) {
		hash_slot = new_node;
		new_node->hash_next = NULL;
	}
	else {
		new_node->hash_next = hash_slot;
		hash_slot = new_node;
	}
	
	if (stack_slot == NULL) {
		stack_slot = new_node;
		new_node->stack_next = NULL;
	}
	else {
		new_node->stack_next = stack_slot;
		stack_slot = new_node;
	}

	kSymbolHashTable[hash_num] = hash_slot;
	kSymbolStackHead->symbol_head = stack_slot;
	
	return new_node;
}
void pushSymbolStack(){
	//insert a node in the head of nodelist
	SymbolStackNode *newStackNode = (SymbolStackNode *)malloc(sizeof(SymbolStackNode));
	newStackNode->next = kSymbolStackHead;
	kSymbolStackHead = newStackNode;
	kSymbolStackHead->funcptr = NULL;
	kSymbolStackHead->symbolHead = NULL;
}

void popSymbolStack(){
	SymbolStackNode *deleteNode = kSymbolStackHead;
	kSymbolStackHead = kSymbolStackHead->next;
	while (deleteNode->symbolHead){
		SymbolNode *temp = deleteNode->symbolHead;
		if (temp->is_func && temp->funcInfo.argumentNum > 0){
			free(temp->funcInfo.argumentType);
		}
		if (!temp->is_func){
			freeType(temp->defInfo);
		}
		deleteNode->symbolHead = deleteNode->symbolHead->stackNext;
		free(temp);
	}
	free(deleteNode);
	return;
}

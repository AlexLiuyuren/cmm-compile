#include "common.h"
#include "syntax_tree.h"
#include "symbol_table.h"

//kSymbolHashTable use openhashing
SymbolNode *kSymbolHashTable[kHashSize];
SymbolStackNode *kSymbolStackHead = NULL;
//structTable is a linklist
SymbolNode *kStructTableHead = NULL;

//main function
void symbolTableMain(TreeNode *p){
	kSymbolStackHead = (SymbolStackNode *)malloc(sizeof(SymbolStackNode));
	kSymbolStackHead->symbol_head = NULL;
	kSymbolStackHead->func_ptr = NULL;
	kSymbolStackHead->next = NULL;
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
	if (strcmp(p->children[1]->symbol, "ExtDefList") == 0){
		Type node_type = buildSpecifier(p->children[0]);
		TreeNode *temp = p->children[1];
		while (temp->arity > 1){
			buildVarDec(node_type, temp->children[0]);
			temp = temp->children[2];
		}
		buildVarDec(node_type, temp->children[0]);
	}
	if (strcmp(p->children[1]->symbol, "SEMI") == 0){ 
		buildSpecifier(p->children[0]);
	}
	if (strcmp(p->children[1]->symbol, "FunDec") == 0){
		Type node_type = buildSpecifier(p->children[0]);
		buildFunDec(node_type, p->children[1]);
		int i;
		for (i = 1; i < p->children[2]->arity; i ++){
			buildSymbolTable(p->children[2]->children[i]);
		}
	}
	return;
}

//VarDec -> ID | VarDec LB INT RB
void buildVarDec(Type node_type, TreeNode *p){
	if (p->arity == 1) {
		//redefined variable
		if (searchSymbol(p->children[0]->text)) {
			symbolError('3', p->children[0]);
			return;
		}
		SymbolNode *new_node = addSymbol(p->children[0]->text);
		strcpy(new_node->name, p->children[0]->text);
		new_node->is_func = false;
		new_node->is_def = true;
		new_node->lineno = p->children[0]->lineno;
		new_node->def_info = (Type *) malloc(sizeof(Type));
		memcpy(new_node->def_info, &node_type, sizeof(Type));
	}
	else {
		TreeNode *temp = p;
		Type *array_head = (Type *) malloc(sizeof(Type));
		array_head->type = BASIC;
		array_head->basic = node_type.basic;
		while (temp->arity > 1) {
			Type *array_temp = (Type *) malloc(sizeof(Type));
			array_temp->type = ARRAY;
			array_temp->array.size = temp->children[2]->int_value;
			array_temp->array.element = array_head;
			array_head = array_temp;
			temp = temp->children[0];
		}
		if (searchSymbol(temp->children[0]->text)) {
			symbolError('3', temp->children[0]);
			return;
		}
		SymbolNode *new_node = addSymbol(temp->children[0]->text);
		strcpy(new_node->name, temp->children[0]->text);
		new_node->is_func = false;
		new_node->is_def = true;
		new_node->lineno = p->children[0]->lineno;
		new_node->def_info = array_head;
	}
	return;
}

// FunDec -> ID LP VarList RP | ID LP RP
// VarList -> ParamDec COMMA VarList | ParamDec
// ParamDec -> Specifier VarDec
void buildFunDec(Type node_type, TreeNode *p){
	SymbolNode *symbol_temp = searchSymbol(p->children[0]->text);
	if (symbol_temp != NULL)
	{
		symbolError('4', p->children[0]);
		pushSymbolStack();
		return;
	}
	
	SymbolNode *new_node = addSymbol(p->children[0]->text);
	strcpy(new_node->name, p->children[0]->text);
	new_node->is_func = true;
	new_node->is_def = false;
	new_node->lineno = p->children[0]->lineno;
	new_node->func_info.return_value = node_type;
	pushSymbolStack();
	SymbolStackHead->func_ptr = new_node;
	if (p->arity == 4) {
		int cnt = 1;
		TreeNode *temp = p->children[2];
		while (temp->arity > 1) {
			cnt++;
			temp = temp->children[2];
		}
		new_node->func_info.argument_num = cnt;
		new_node->func_info.argument_type = (Type *) malloc(sizeof(Type) * cnt);
		cnt = 0;
		temp = p->children[2];
		while (temp->arity > 1) {
			Type new_node_type = buildSpecifier(temp->children[0]->children[0]);
			buildVarDec(new_node_type, temp->children[0]->children[1]);
			new_node->func_info.argument_type[cnt] = new_node_type;
			cnt++;
			temp = temp->children[2];
		}
		Type new_node_type = buildSpecifier(temp->children[0]->children[0]);
		buildVarDec(new_node_type, temp->children[0]->children[1]);
		new_node->func_info.argument_type[cnt] = new_node_type;
		cnt++;
	}
	else if (p->arity == 3) {
		new_node->func_info.argument_number = 0;
		new_node->func_info.argument_type = NULL; 
	}
	else {
		assert(0);
	}
}
void buildDef(TreeNode *p){
	Type node_type = buildSpecifier(p->children[0]);
	TreeNode *temp = p->children[1];
	while (temp->arity > 1) {		
		buildVarDec(node_type, temp->children[0]->children[0]);
		temp = temp->children[2];
	}
	buildVarDec(node_type, temp->children[0]->children[0]);
}

void buildStmt(TreeNode *p){
	if (strcmp(p->children[0]->symbol, "RETURN") == 0) {
		SymbolStackNode *func_field = kSymbolStackHead;
		while (func_field && !func_field->func_ptr) 
			func_field = func_field->next;
		if (func_field && typeEqual(func_field->func_ptr->func_info.return_value, buildExp(p->children[1])) == 0)
			symbolError('8', p);
	}
	int i;
	for (i = 0; i < p->arity; i++)
		buildSymbolTable(p->children[i]);
}

void buildExp(TreeNode *p){
	if (strcmp(p->children[0]->symbol, "RETURN") == 0) {
		SymbolStackNode *func_field = kSymbolStackHead;
		while (func_field && !func_field->func_ptr) 
			func_field = func_field->next;
		if (func_field != NULL && typeEqual(func_field->func_ptr->func_info.return_value, buildExp(p->children[1])) == 0)
			symbolError('8', p);
	}
	int i;
	for (i = 0; i < p->arity; i++)
		buildSymbolTable(p->children[i]);
}




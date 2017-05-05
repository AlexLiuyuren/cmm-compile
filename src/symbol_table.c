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
		if (searchSymbolTable(p->children[0]->text)) {
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
		if (searchSymbolTable(temp->children[0]->text)) {
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
	SymbolNode *symbol_temp = searchSymbolTable(p->children[0]->text);
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
	kSymbolStackHead->func_ptr = new_node;
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
		new_node->func_info.argument_num = 0;
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

Type buildExp(TreeNode *p){
	Type return_val;
	return_val.type = OTHER;
	// single element
	if (p->arity == 1) {
		if (strcmp(p->children[0]->symbol, "ID") == 0) {
			SymbolNode *symbol_temp = searchSymbolTable(p->children[0]->text);
			if(symbol_temp == NULL) {
				symbolError('1', p->children[0]);
				return_val.type = NOTDEF;
				return return_val;
			}
			return_val = *symbol_temp->def_info;
			return return_val;
		}
		else if (strcmp(p->children[0]->symbol, "INT") == 0) {
			return_val.type = BASIC;
			return_val.basic = B_INT;
		}
		else {
			return_val.type = BASIC;
			return_val.basic = B_FLOAT;
		}
		return return_val;
	}
	
	// array
	if (p->arity > 2 && strcmp(p->children[1]->symbol, "LB") == 0) {
		TreeNode *temp = p;
		while (temp->arity > 1)
			temp = temp->children[0];
		SymbolNode *symbol_temp = searchSymbolTable(temp->children[0]->text);
		if(symbol_temp == NULL) {
			symbolError('1', p->children[0]);
			return_val.type = NOTDEF;
			return return_val;
		}
		else if (symbol_temp->def_info->type != ARRAY) {
			symbolError('a', temp->children[0]);
			return_val.type = NOTDEF;
			return return_val;
		}
		temp = p;
		while (temp->arity > 1) {
			Type int_temp;
			int_temp.type = BASIC, int_temp.basic = B_INT;
			if (typeEqual(buildExp(temp->children[2]), int_temp) == 0) {
				symbolError('c', temp->children[2]->children[0]);
				return_val.type = NOTDEF;
				return return_val;
			}
			temp = temp->children[0];
		}
	}
	
	// struct
	if (p->arity > 2 && strcmp(p->children[1]->symbol, "DOT") == 0) {
		Type type_temp = buildExp(p->children[0]);
		if (type_temp.type != STRUCT) {
			symbolError('d', p->children[0]);
			return_val.type = NOTDEF;
			return return_val;
		}
		else {
			StructContent *list_temp = searchStructContent(type_temp.structure, p->children[2]->text);
			if (list_temp == NULL) {
				symbolError('e', p->children[2]);
				return_val.type = NOTDEF;
				return return_val;
			}
			else
				return_val = *list_temp->type;
		}
		return return_val;
	}
	
		
	// function
	// Exp -> ID LP Args RP
	//      | ID LP RP
	// Args -> Exp COMMA Args
	//       | Exp
	if (p->arity > 1 && strcmp(p->children[1]->symbol, "LP") == 0) {
		SymbolNode *symbol_temp = searchSymbolTable(p->children[0]->text);
		if (symbol_temp == NULL) 
			symbolError('2', p->children[0]);
		else if (symbol_temp->is_func == false)
			symbolError('b', p->children[0]);
		else {
			int cnt = 1;
			TreeNode *args_temp = p->children[2];
			while (args_temp->arity > 1) {
				cnt++;
				args_temp = args_temp->children[2];
			}
			Type *call = (Type *) malloc(sizeof(Type) * cnt);
			cnt = 0;
			args_temp = p->children[2];
			while (args_temp->arity > 1) {
				call[cnt] = buildExp(args_temp->children[0]);
				cnt++;
				args_temp = args_temp->children[2];
			}
			call[cnt] = buildExp(args_temp->children[0]);
			cnt++;
			int flag = true;
			if (cnt != symbol_temp->func_info.argument_num)
				flag = false;
			else {
				int i;
				for (i = 0; i < cnt; i++)
					if (typeEqual(call[cnt], symbol_temp->func_info.argument_type[cnt]) == 0) {
						flag = false;
						break;
					}
			}
			if (!flag) {
				symbolError('9', p);
				printf("Function \"%s(", symbol_temp->name);
				int i;
				char str[40];
				for (i = 0; i < symbol_temp->func_info.argument_num - 1; i++) {
					printType(symbol_temp->func_info.argument_type[i], str);
					printf("%s, ", str);
				}
				printType(symbol_temp->func_info.argument_type[i], str);
				printf("%s", str);
				printf(")\" is not applicable for arguments \"(");
				for (i = 0; i < cnt - 1; i++) {
					printType(call[i], str);
					printf("%s, ", str);
				}
				printType(call[i], str);
				printf("%s)\".\n", str);
			}
			free(call);
		}
		return return_val;
	}
	
	// three elements
	if (p->arity == 3 && strcmp(p->children[0]->symbol, "LP") == 0) { // brackets
		return buildExp(p->children[1]);
	}
	if (p->arity == 3 && strcmp(p->children[1]->symbol, "ASSIGNOP") == 0) {
		int flag = false;
		if (strcmp(p->children[0]->children[0]->symbol, "ID") == 0) flag = true;
		if (p->children[0]->arity > 1 && strcmp(p->children[0]->children[1]->symbol, "LB") == 0) flag = true;
		if (p->children[0]->arity > 1 && strcmp(p->children[0]->children[1]->symbol, "DOT") == 0) flag = true;
		if (!flag) {
			symbolError('6', p);
			return return_val;
		}
		Type val1 = buildExp(p->children[0]), val2 = buildExp(p->children[2]);
		if (typeEqual(val1, val2) == 0) {
			symbolError('5', p);
			return return_val;
		}
		return val1;
	}
	if (p->arity == 3 && strcmp(p->children[1]->symbol, "Exp") && strcmp(p->children[1]->symbol, "DOT")) {
		Type val1 = buildExp(p->children[0]), val2 = buildExp(p->children[2]);	
		if (typeEqual(val1, val2) == 0) {
			symbolError('7', p);
			return return_val;
		}
		return val1;
	}
	int i;
	for (i = 0; i < p->arity; i++) {
		if (p->arity > i && strcmp(p->children[i]->symbol, "Exp") == 0)
			buildExp(p->children[i]);
	}
	return return_val;
}

Type buildSpecifier(TreeNode *p)
{
	Type node_type;
	if (strcmp(p->children[0]->symbol, "TYPE") == 0) {
		node_type.type = BASIC;
		if (strcmp(p->children[0]->text, "int") == 0) node_type.basic = B_INT;
		else node_type.basic = B_FLOAT;
	}
	else if (strcmp(p->children[0]->symbol, "StructSpecifier") == 0) {
		TreeNode *struct_temp = p->children[0];
		if (struct_temp->arity > 3) {
			if (searchStructTable(struct_temp->children[1]->children[0]->text) != NULL) {
				symbolError('g', struct_temp->children[1]->children[0]);
				node_type.type = NOTDEF;
				return node_type;
			}
			SymbolNode *new_nodec = pushStruct(struct_temp->children[1]->children[0]->text);
			strcpy(new_nodec->name, struct_temp->children[1]->children[0]->text);
			new_nodec->def_info = (Type *) malloc(sizeof(Type));
			new_nodec->def_info->type = STRUCT;
			TreeNode *def_temp = struct_temp->children[3];
			while (def_temp->arity > 1) {
				buildStructDef(def_temp->children[0]);
				def_temp = def_temp->children[1];
			}
			buildStructDef(def_temp->children[0]);
			node_type = *new_nodec->def_info;
		}
		else {
			SymbolNode *symbol_temp = searchStructTable(struct_temp->children[1]->children[0]->text);
			if (symbol_temp == NULL)
				symbolError('h', struct_temp->children[1]->children[0]);
			else 
				node_type = *symbol_temp->def_info;
		}
	}
	else {
		assert(0);
	}
	return node_type;
}

void buildStructDef(TreeNode *p)
{
	Type node_type = buildSpecifier(p->children[0]);
	TreeNode *temp = p->children[1];
	while (temp->arity > 1) {
		buildStructVarDec(node_type, temp->children[0]->children[0]);
		temp = temp->children[2];
	}
	buildStructVarDec(node_type, temp->children[0]->children[0]);
}

void buildStructVarDec(Type node_type, TreeNode *p)
{
	if (p->arity == 1) {
		if (searchStructContent(kStructTableHead->def_info->structure, p->children[0]->text) != NULL) {
			symbolError('f', p->children[0]);
			return;
		}
		StructContent *new_node = pushStructContent(p->children[0]->text);
		strcpy(new_node->name, p->children[0]->text);
		new_node->type = (Type *) malloc(sizeof(Type));
		memcpy(new_node->type, &node_type, sizeof(Type));
	}
	else {
	}
}

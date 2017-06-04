#include "common.h"
#include "syntax_tree.h"
#include "symbol_table.h"
#include "ir.h"

//kSymbolHashTable use openhashing
SymbolNode *kSymbolHashTable[kHashSize];
//kSymbolStackHead is a linklist
//It's aimed to allow nested scope
SymbolStackNode *kSymbolStackHead = NULL;
//structTable is a linklist
SymbolNode *kStructTableHead = NULL;

//main function
void symbolTableMain(TreeNode *p){
	kSymbolStackHead = (SymbolStackNode *)malloc(sizeof(SymbolStackNode));
	kSymbolStackHead->symbol_head = NULL;
	kSymbolStackHead->func_ptr = NULL;
	kSymbolStackHead->next = NULL;
    
    // push in function "read" and "write"
    Type type_temp;
    type_temp.type = BASIC, type_temp.basic = B_INT;
    SymbolNode *new_node = addSymbol("read");
    strcpy(new_node->name, "read");
    new_node->is_func = true, new_node->is_def = true;
    new_node->func_info.return_value = type_temp, new_node->func_info.argument_num = 0, new_node->func_info.argument_type = NULL;
    new_node = addSymbol("write");
    strcpy(new_node->name, "write");
    new_node->is_func = true, new_node->is_def = true;
    new_node->func_info.return_value = type_temp, new_node->func_info.argument_num = 1, new_node->func_info.argument_type = (Type *) malloc(sizeof(Type));
    new_node->func_info.argument_type[0] = type_temp;
    
    initIR();
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
		buildStmt(p, &InterCodeHead);
		return;
	}
	if (strcmp(p->symbol, "Exp") == 0){
		buildExp(p, NULLOP, &InterCodeHead);
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
    
    InterCode irtemp;
    irtemp.kind = FUNCTION;
    strcpy(irtemp.function, p->children[0]->text);
    InterCodeAppend(&InterCodeHead, irtemp);

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
            //ir
			irtemp.kind = PARAM;
            symbol_temp = searchSymbolTable(temp->children[0]->children[1]->children[0]->text);
            irtemp.param = generateVar(symbol_temp);
            InterCodeAppend(&InterCodeHead, irtemp);
			cnt++;
			temp = temp->children[2];
		}
		Type new_node_type = buildSpecifier(temp->children[0]->children[0]);
		buildVarDec(new_node_type, temp->children[0]->children[1]);
		new_node->func_info.argument_type[cnt] = new_node_type;
        irtemp.kind = PARAM;
        symbol_temp = searchSymbolTable(temp->children[0]->children[1]->children[0]->text);
        irtemp.param = generateVar(symbol_temp);
        InterCodeAppend(&InterCodeHead, irtemp);
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

//void buildStmt(TreeNode *p, , InterCodeNode *retIr){
//	if (strcmp(p->children[0]->symbol, "RETURN") == 0) {
//		SymbolStackNode *func_field = kSymbolStackHead;
//		while (func_field && !func_field->func_ptr) 
//			func_field = func_field->next;
//		if (func_field && typeEqual(func_field->func_ptr->func_info.return_value, buildExp(p->children[1])) == 0)
//			symbolError('8', p);
//	}
//	int i;
//	for (i = 0; i < p->arity; i++)
//		buildSymbolTable(p->children[i]);
//}

void buildStmt(TreeNode *p, InterCodeNode *retIr)
{
    if (p->arity == 2) {
        buildExp(p->children[0], NULLOP, retIr);
    }
    else if (p->arity == 1) {
        int i;
        for (i = 0; i < p->arity; i++)
            buildSymbolTable(p->children[i]);
    }
    else if (p->arity == 3) {
        SymbolStackNode *funcfield = kSymbolStackHead;
        Operand optemp = generateTemp();
        while (funcfield != NULL && funcfield->func_ptr == NULL)
            funcfield = funcfield->next;
        if (funcfield != NULL && typeEqual(funcfield->func_ptr->func_info.return_value, buildExp(p->children[1], optemp, retIr)) == 0)
            symbolError('8', p);
        InterCode irtemp;
        irtemp.kind = RETURN_, irtemp.return_ = optemp;
        InterCodeAppend(retIr, irtemp);
    }
    else if (p->arity == 5 && STREQ(p->children[0]->symbol, "IF")) {
        Operand labeltemp1 = generateLabel(), labeltemp2 = generateLabel();
        InterCodeNode code1, code2;
        INITICN(code1); INITICN(code2);
        buildCond(p->children[2], labeltemp1, labeltemp2, &code1);
        buildStmt(p->children[4], &code2);
        InterCode irtemp;
        irtemp.kind = LABEL_CODE, irtemp.label_code = labeltemp1;
        InterCodeAppend(&code1, irtemp);
        irtemp.label_code = labeltemp2;
        InterCodeAppend(&code2, irtemp);
        InterCodeCat(3, retIr, &code1, &code2);
    }
    else if (p->arity == 7) {
        Operand labeltemp1 = generateLabel(), labeltemp2 = generateLabel(), labeltemp3 = generateLabel();
        InterCodeNode code1, code2, code3;
        INITICN(code1); INITICN(code2); INITICN(code3);
        buildCond(p->children[2], labeltemp1, labeltemp2, &code1);
        buildStmt(p->children[4], &code2);
        buildStmt(p->children[6], &code3);
        InterCode irtemp;
        irtemp.kind = LABEL_CODE, irtemp.label_code = labeltemp1;
        InterCodeAppend(&code1, irtemp);
        irtemp.kind = LABEL_GOTO, irtemp.label_goto = labeltemp3;
        InterCodeAppend(&code2, irtemp);
        irtemp.kind = LABEL_CODE, irtemp.label_code = labeltemp2;
        InterCodeAppend(&code2, irtemp);
        irtemp.label_code = labeltemp3;
        InterCodeAppend(&code3, irtemp);
        InterCodeCat(4, retIr, &code1, &code2, &code3);
    }
    else {
        Operand labeltemp1 = generateLabel(), labeltemp2 = generateLabel(), labeltemp3 = generateLabel();
        InterCodeNode code1, code2;
        INITICN(code1); INITICN(code2);
        buildCond(p->children[2], labeltemp2, labeltemp3, &code1);
        buildStmt(p->children[4], &code2);
        InterCode irtemp;
        irtemp.kind = LABEL_CODE, irtemp.label_code = labeltemp1;
        InterCodeAppend(retIr, irtemp);
        irtemp.label_code = labeltemp2;
        InterCodeAppend(&code1, irtemp);
        irtemp.kind = LABEL_GOTO, irtemp.label_goto = labeltemp1;
        InterCodeAppend(&code2, irtemp);
        irtemp.kind = LABEL_CODE, irtemp.label_code = labeltemp3;
        InterCodeAppend(&code2, irtemp);
        InterCodeCat(3, retIr, &code1, &code2);
    }
}
Type buildExp(TreeNode *p, Operand place, InterCodeNode *retIr){
	Type return_value;
	return_value.type = OTHER;
	// single element
	if (p->arity == 1) {
		InterCode return_code;
		if (strcmp(p->children[0]->symbol, "ID") == 0) {
			SymbolNode *symbol_temp = searchSymbolTable(p->children[0]->text);
			if(symbol_temp == NULL) {
				symbolError('1', p->children[0]);
				return_value.type = NOTDEF;
				return return_value;
			}
			return_value = *symbol_temp->def_info;
            
            return_code.kind = ASSIGN;
            return_code.assign.left = place;
            return_code.assign.right = generateVar(symbol_temp);
            InterCodeAppend(retIr, return_code);
			return return_value;
		}
		else if (strcmp(p->children[0]->symbol, "INT") == 0) {
			return_value.type = BASIC;
			return_value.basic = B_INT;
            
            return_code.kind = ASSIGN;
            return_code.assign.left = place;
            return_code.assign.right.kind = CONSTANT, return_code.assign.right.value = p->children[0]->int_value;
            InterCodeAppend(retIr, return_code);
		}
		else {
			return_value.type = BASIC;
			return_value.basic = B_FLOAT;
		}
		return return_value;
	}
	
	// array
	if (p->arity > 2 && strcmp(p->children[1]->symbol, "LB") == 0) {
		TreeNode *temp = p;
		while (temp->arity > 1)
			temp = temp->children[0];
		SymbolNode *symbol_temp = searchSymbolTable(temp->children[0]->text);
		if(symbol_temp == NULL) {
			symbolError('1', p->children[0]);
			return_value.type = NOTDEF;
			return return_value;
		}
		else if (symbol_temp->def_info->type != ARRAY) {
			symbolError('a', temp->children[0]);
			return_value.type = NOTDEF;
			return return_value;
		}
		temp = p;
		while (temp->arity > 1) {
			Type int_temp;
			int_temp.type = BASIC, int_temp.basic = B_INT;
			if (typeEqual(buildExp(temp->children[2], NULLOP, NULL), int_temp) == 0) {
				symbolError('c', temp->children[2]->children[0]);
				return_value.type = NOTDEF;
				return return_value;
			}
			temp = temp->children[0];
		}
	}
	
	// struct
	if (p->arity > 2 && strcmp(p->children[1]->symbol, "DOT") == 0) {
		Type type_temp = buildExp(p->children[0], NULLOP, NULL);
		if (type_temp.type != STRUCT) {
			symbolError('d', p->children[0]);
			return_value.type = NOTDEF;
			return return_value;
		}
		else {
			StructContent *list_temp = searchStructContent(type_temp.structure, p->children[2]->text);
			if (list_temp == NULL) {
				symbolError('e', p->children[2]);
				return_value.type = NOTDEF;
				return return_value;
			}
			else
				return_value = *list_temp->type;
		}
		return return_value;
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
        	else if (strcmp(p->children[2]->symbol, "RP") == 0) {
		    if (symbol_temp->func_info.argument_num > 0) {
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
		        printf(")\" is not applicable for arguments \"().\n");
		        return return_value;
		    }
		    if (strcmp(p->children[0]->text, "read") == 0) {
		        InterCode irtemp;
		        irtemp.kind = READ, irtemp.read = place;
		        InterCodeAppend(retIr, irtemp);
		    }
		    else {
		        InterCode irtemp;
		        irtemp.kind = CALL_FUNC, irtemp.call_func.dest = place;
		        strcpy(irtemp.call_func.func, p->children[0]->text);
		        InterCodeAppend(retIr, irtemp);
		    }
		    return_value = symbol_temp->func_info.return_value;
        	}
		else {
		    int cnt = 1;
		    TreeNode *args_temp = p->children[2];
		    while (args_temp->arity > 1) {
			cnt++;
			args_temp = args_temp->children[2];
		    }
		    Type *call = (Type *) malloc(sizeof(Type) * cnt);
		    Operand *arg_list = (Operand *) malloc(sizeof(Operand) * cnt);
		    InterCodeNode icntemp;
		    INITICN(icntemp);
		    cnt = 0;
		    args_temp = p->children[2];
		    while (args_temp->arity > 1) {
                	arg_list[cnt] = generateTemp();
			call[cnt] = buildExp(args_temp->children[0], arg_list[cnt], &icntemp);
			cnt++;
			args_temp = args_temp->children[2];
		    }
            	    arg_list[cnt] = generateTemp();
		    call[cnt] = buildExp(args_temp->children[0], arg_list[cnt], &icntemp);
		    cnt++;
		    int flag = true;
		    if (cnt != symbol_temp->func_info.argument_num)
			flag = false;
		    else {
			int i;
			for (i = 0; i < cnt; i++)
				if (typeEqual(call[i], symbol_temp->func_info.argument_type[i]) == 0) {
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
                		return return_value;
			}
            
		        if (strcmp(p->children[0]->text, "write") == 0) {
			    InterCode irtemp;
			    irtemp.kind = WRITE, irtemp.write = arg_list[0];
			    InterCodeAppend(&icntemp, irtemp);
			    InterCodeCat(2, retIr, &icntemp);
		        }
		        else {
			    int i;
			    InterCode irtemp;
			    for (i = 0; i < cnt; i++) {
			        irtemp.kind = ARG, irtemp.arg = arg_list[i];
			        InterCodeAppend(&icntemp, irtemp);
			    }
			    irtemp.kind = CALL_FUNC, irtemp.call_func.dest = place;
			    strcpy(irtemp.call_func.func, p->children[0]->text);
			    InterCodeAppend(&icntemp, irtemp);
			    InterCodeCat(2, retIr, &icntemp);
		    	}
            
			free(call);
            		free(arg_list);
            		return_value = symbol_temp->func_info.return_value;
		}
		return return_value;
	}
	
    // two elements
    if (STREQ(p->children[0]->symbol, "MINUS")) {
        InterCode irtemp;
        irtemp.kind = SUB;
        irtemp.binop.result = place;
        irtemp.binop.op1.kind = CONSTANT, irtemp.binop.op1.value = 0;
        irtemp.binop.op2 = generateTemp();
        InterCodeNode rir;
        INITICN(rir);
        Type val2 = buildExp(p->children[1], irtemp.binop.op2, &rir);
        InterCodeAppend(&rir, irtemp);
        InterCodeCat(2, retIr, &rir);
        return val2;
    }
    
    if (STREQ(p->children[0]->symbol, "NOT")) {
        Operand labeltemp1 = generateLabel(), labeltemp2 = generateLabel();
        InterCode irtemp;
        irtemp.kind = ASSIGN;
        irtemp.assign.left = place;
        irtemp.assign.right.kind = CONSTANT, irtemp.assign.right.value = 0;
        InterCodeAppend(retIr, irtemp);
        InterCodeNode icntemp;
        INITICN(icntemp);
        Type vartemp = buildCond(p, labeltemp1, labeltemp2, &icntemp);
        irtemp.kind = LABEL_CODE;
        irtemp.label_code = labeltemp1;
        InterCodeAppend(&icntemp, irtemp);
        irtemp.kind = ASSIGN;
        irtemp.assign.left = place;
        irtemp.assign.right.kind = CONSTANT, irtemp.assign.right.value = 1;
        InterCodeAppend(&icntemp, irtemp);
        irtemp.kind = LABEL_CODE;
        irtemp.label_code = labeltemp2;
        InterCodeAppend(&icntemp, irtemp);
        InterCodeCat(2, retIr, &icntemp);
        return vartemp;
    }

	// three elements
	if (p->arity == 3 && strcmp(p->children[0]->symbol, "LP") == 0) { // brackets
		return buildExp(p->children[1], place, retIr);
	}
	if (p->arity == 3 && strcmp(p->children[1]->symbol, "ASSIGNOP") == 0) {
		int flag = false;
		if (strcmp(p->children[0]->children[0]->symbol, "ID") == 0) flag = true;
		if (p->children[0]->arity > 1 && strcmp(p->children[0]->children[1]->symbol, "LB") == 0) flag = true;
		if (p->children[0]->arity > 1 && strcmp(p->children[0]->children[1]->symbol, "DOT") == 0) flag = true;
		if (!flag) {
			symbolError('6', p);
			return return_value;
		}
        SymbolNode *symbol_temp = searchSymbolTable(p->children[0]->children[0]->text);
        InterCode irtemp;
        irtemp.kind = ASSIGN;
        irtemp.assign.right = generateTemp();
        InterCodeNode lir, rir;
        INITICN(lir); INITICN(rir);
        Type val1 = buildExp(p->children[0], place, &lir), val2 = buildExp(p->children[2], irtemp.assign.right, &rir);
        irtemp.assign.left = generateVar(symbol_temp);
        InterCodeAppend(&rir, irtemp);
        InterCodeCat(3, retIr, &rir, &lir);

		if (typeEqual(val1, val2) == 0) {
			symbolError('5', p);
			return return_value;
		}
		return val1;
	}
	if (p->arity == 3 && strcmp(p->children[1]->symbol, "Exp") && strcmp(p->children[1]->symbol, "DOT")) {
        if (STREQ(p->children[1]->symbol, "PLUS")|| STREQ(p->children[1]->symbol, "MINUS") || STREQ(p->children[1]->symbol, "STAR") || STREQ(p->children[1]->symbol, "DIV")) {
            InterCodeNode lir, rir;
            INITICN(lir); INITICN(rir);
            Operand optemp1 = generateTemp(), optemp2 = generateTemp();
            Type val1 = buildExp(p->children[0], optemp1, &lir), val2 = buildExp(p->children[2], optemp2, &rir);
            InterCode irtemp;
            if (STREQ(p->children[1]->symbol, "PLUS")) {
                irtemp.kind = ADD, irtemp.binop.result = place, irtemp.binop.op1 = optemp1, irtemp.binop.op2 = optemp2;
            }
            if (STREQ(p->children[1]->symbol, "MINUS")) {
                irtemp.kind = SUB, irtemp.binop.result = place, irtemp.binop.op1 = optemp1, irtemp.binop.op2 = optemp2;
            }
            if (STREQ(p->children[1]->symbol, "STAR")) {
                irtemp.kind = MUL, irtemp.binop.result = place, irtemp.binop.op1 = optemp1, irtemp.binop.op2 = optemp2;
            }
            if (STREQ(p->children[1]->symbol, "DIV")) {
                irtemp.kind = DIV_, irtemp.binop.result = place, irtemp.binop.op1 = optemp1, irtemp.binop.op2 = optemp2;
            }
            InterCodeAppend(&rir, irtemp);
            InterCodeCat(3, retIr, &lir, &rir);
            if (typeEqual(val1, val2) == 0) {
                symbolError('7', p);
                return return_value;
            }
            return val1;
//		Type val1 = buildExp(p->children[0]), val2 = buildExp(p->children[2]);
//		if (typeEqual(val1, val2) == 0) {
//			symbolError('7', p);
//			return return_value;
//		}
//		return val1;
        }else {
            Operand labeltemp1 = generateLabel(), labeltemp2 = generateLabel();
            InterCode irtemp;
            irtemp.kind = ASSIGN;
            irtemp.assign.left = place;
            irtemp.assign.right.kind = CONSTANT, irtemp.assign.right.value = 0;
            InterCodeAppend(retIr, irtemp);
            InterCodeNode icntemp;
            INITICN(icntemp);
            Type vartemp = buildCond(p, labeltemp1, labeltemp2, &icntemp);
            irtemp.kind = LABEL_CODE;
            irtemp.label_code = labeltemp1;
            InterCodeAppend(&icntemp, irtemp);
            irtemp.kind = ASSIGN;
            irtemp.assign.left = place;
            irtemp.assign.right.kind = CONSTANT, irtemp.assign.right.value = 1;
            InterCodeAppend(&icntemp, irtemp);
            irtemp.kind = LABEL_CODE;
            irtemp.label_code = labeltemp2;
            InterCodeAppend(&icntemp, irtemp);
            InterCodeCat(2, retIr, &icntemp);
            return vartemp;
        }
	}    
	int i;
	for (i = 0; i < p->arity; i++) {
		if (p->arity > i && strcmp(p->children[i]->symbol, "Exp") == 0)
			buildExp(p->children[i], NULLOP, NULL);
	}
	return return_value;
}

Type buildSpecifier(TreeNode *p){
	Type node_type;
	if (strcmp(p->children[0]->symbol, "TYPE") == 0) {
		node_type.type = BASIC;
		if (strcmp(p->children[0]->text, "int") == 0) node_type.basic = B_INT;
		else node_type.basic = B_FLOAT;
	}else if (strcmp(p->children[0]->symbol, "StructSpecifier") == 0) {
		TreeNode *struct_temp = p->children[0];
		if (struct_temp->arity > 3) {
			if (searchStructTable(struct_temp->children[1]->children[0]->text) != NULL) {
				symbolError('g', struct_temp->children[1]->children[0]);
				node_type.type = NOTDEF;
				return node_type;
			}
			SymbolNode *new_node = pushStruct(struct_temp->children[1]->children[0]->text);
			strcpy(new_node->name, struct_temp->children[1]->children[0]->text);
			new_node->def_info = (Type *) malloc(sizeof(Type));
			new_node->def_info->type = STRUCT;
			TreeNode *def_temp = struct_temp->children[3];
			while (def_temp->arity > 1) {
				buildStructDef(def_temp->children[0]);
				def_temp = def_temp->children[1];
			}
			buildStructDef(def_temp->children[0]);
			node_type = *new_node->def_info;
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

Type buildCond(TreeNode *p, Operand label_true, Operand label_false, InterCodeNode *retIr){
    if (strcmp(p->children[0]->symbol, "NOT") == 0) {
        buildCond(p->children[1], label_false, label_true, retIr);
    }
    else if (strcmp(p->children[1]->symbol, "RELOP") == 0) {
        Operand optemp1 = generateTemp(), optemp2 = generateTemp();
        InterCodeNode lir, rir;
        INITICN(lir); INITICN(rir);
        buildExp(p->children[0], optemp1, &lir);
        buildExp(p->children[2], optemp2, &rir);
        InterCode irtemp;
        irtemp.kind = LABEL_COND;
        irtemp.label_cond.left = optemp1, irtemp.label_cond.right = optemp2, irtemp.label_cond.dest = label_true;
        strcpy(irtemp.label_cond.op, p->children[1]->text);
        InterCodeAppend(&rir, irtemp);
        irtemp.kind = LABEL_GOTO;
        irtemp.label_goto = label_false;
        InterCodeAppend(&rir, irtemp);
        InterCodeCat(3, retIr, &lir, &rir);
    }
    else if (strcmp(p->children[1]->symbol, "AND") == 0) {
        Operand labeltemp = generateLabel();
        InterCodeNode lir, rir;
        INITICN(lir); INITICN(rir);
        buildCond(p->children[0], labeltemp, label_false, &lir);
        buildCond(p->children[2], label_true, label_false, &rir);
        InterCode irtemp;
        irtemp.kind = LABEL_CODE;
        irtemp.label_code = labeltemp;
        InterCodeAppend(&lir, irtemp);
        InterCodeCat(3, retIr, &lir, &rir);
    }
    else if (strcmp(p->children[1]->symbol, "OR") == 0) {
        Operand labeltemp = generateLabel();
        InterCodeNode lir, rir;
        INITICN(lir); INITICN(rir);
        buildCond(p->children[0], label_true, labeltemp, &lir);
        buildCond(p->children[2], label_true, label_false, &rir);
        InterCode irtemp;
        irtemp.kind = LABEL_CODE;
        irtemp.label_code = labeltemp;
        InterCodeAppend(&lir, irtemp);
        InterCodeCat(3, retIr, &lir, &rir);
    }
    else {
        Operand optemp = generateTemp();
        InterCodeNode icntemp;
        INITICN(icntemp);
        buildExp(p, optemp, &icntemp);
        InterCode irtemp;
        irtemp.kind = LABEL_COND;
        irtemp.label_cond.left = optemp, irtemp.label_cond.right.kind = CONSTANT, irtemp.label_cond.right.value = 0, irtemp.label_cond.dest = label_true;
        strcpy(irtemp.label_cond.op, "!=");
        InterCodeAppend(&icntemp, irtemp);
        irtemp.kind = LABEL_GOTO;
        irtemp.label_goto = label_false;
        InterCodeAppend(&icntemp, irtemp);
        InterCodeCat(2, retIr, &icntemp);
    } 
    Type return_value;
    return_value.type = BASIC;
	return_value.basic = B_INT;
    return return_value;
}


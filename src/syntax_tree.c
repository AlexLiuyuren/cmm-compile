#include "common.h"
#include "syntax_tree.h"

TreeNode *createTreeNode(int arity, ...){
	assert(arity <= kMaxChildren); 
	TreeNode *p = (TreeNode *)malloc(sizeof(TreeNode));
	p->arity = arity;
	va_list arg_ptr;
	va_start(arg_ptr, arity);
	int i;
	for (i = 0; i < p->arity; i ++){
		TreeNode *temp = va_arg(arg_ptr, TreeNode *);
		if (temp){
			p->children[i] = temp;
		}else{
			i --;
			p->arity --;
		}
	}
	if (p->arity > 0){
		p->lineno = p->children[0]->lineno;
	}
	return p;
}

void printTree(TreeNode *p, int depth){
	printf("%*s%s", depth * kIndent, "", p->symbol);
	if (p->arity == 0){
		if (strcmp(p->symbol, "TYPE") == 0){
			printf(": %s\n", p->text);
		}else if (strcmp(p->symbol, "ID") == 0){
			printf(": %s\n", p->text);
		}else if (strcmp(p->symbol, "RELOP") == 0){
			printf(": %s\n", p->text);
		}else if (strcmp(p->symbol, "INT") == 0){
			printf(": %d\n", p->int_value);
		}else if (strcmp(p->symbol, "FLOAT") == 0){
			printf(": %f\n", p->float_value);
		}else{
			printf("\n");
		}	
	}else{
		printf(" (%d)\n", p->lineno);
		int i;
		for (i = 0; i < p->arity; i++){
			printTree(p->children[i], depth + 1);
		}
	}
	return;
}

void deleteTreeNode(TreeNode *p){
	int i;
	for (i = 0; i < p->arity; i++){
		deleteTreeNode(p->children[i]);
	}
	free(p);
	p = NULL;
	return;
}

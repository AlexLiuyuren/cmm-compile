#include <stdio.h>
#include "syntax_tree.h"

TreeNode *createTreeNode(int arity, ...){
	assert(arity <= kMaxChildren); 
	TreeNode *p = (TreeNode *)malloc(sizeof(TreeNode));i
	p->arity = arity;
	va_list arg_ptr;
	va_start(arg_ptr, arity);
	for (int i = 0; i < p->arity; i ++){
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

void printTree(TreeNode *p){
	//TODO: 
}

void deleteTreeNode(TreeNode *p){
	//TODO
}

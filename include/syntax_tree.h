#ifndef SYNTAX_TREE_H_
#define SYNTAX_TREE_H_

#include "common.h"
#define kMaxChildren 15 //Max number of children
#define kIndent 2
typedef struct TreeNode{
	char symbol[kMaxLen], text[kMaxLen];
	int lineno;
	int arity;
	union{
		int int_value;
		float float_value;
	};
	struct TreeNode *children[kMaxChildren];

}TreeNode;

TreeNode *createTreeNode(int arity, ...);
void printTree(TreeNode *p, int depth);
void deleteTreeNode(TreeNode *p);
#endif

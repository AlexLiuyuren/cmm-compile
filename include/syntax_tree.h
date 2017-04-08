#ifndef SYNTAX_TREE_H_
#define SYNTAX_TREE_H_

#define kMaxLen 32 //Max length of symbol
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

TreeNode *createTreeNode();
void printTree(TreeNode *p);
void deleteTreeNode(TreeNode *p);
#endif

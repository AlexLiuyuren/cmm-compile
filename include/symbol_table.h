#ifndef SYMBOL_TABLE_H_
#define SYMBOL_TABLE_H_
#include "common.h"
#define kHashSize 0x3fff

typedef struct Type{
	enum{BASIC, ARRAY, STRUCT} type;
	union{
		enum{B_INT, B_FLOAT} basic;
		struct { struct Type *element; int size;} array;
		struct StructContent *structure;
	};
} Type;

typedef struct StructContent{
	char name[kMaxLen];
	Type *type;
	struct StructContent *next;
} StructContent;

typedef struct SymbolNode{
	char name[kMaxLen];
	int lineno;
	// either isfunc is true or isdef is true
	int isfunc;
	int isdef;
	union{
		struct{
			Type returnValue;
			int argumentNum;
			Type *argumentType;
		}funcInfo;
		Type *defInfo; //symbol is basic/array/struct
	};
	struct SymbolNode *stackNext, *hashNext;
} SymbolNode;

typedef struct SymbolStackNode{
	SymbolStackNode *symbolStackHead;
	SymbolStackNode *funcptr;
	SymbolStackNode *next;
} SymbolStackNode;

void symbolTableMain(TreeNode *p);
void buildSymbolTable(TreeNode *p);
#endif

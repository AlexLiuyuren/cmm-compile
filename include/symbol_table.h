#ifndef SYMBOL_TABLE_H_
#define SYMBOL_TABLE_H_
#include "common.h"
#include "syntax_tree.h"
#define kHashSize 0x3fffc

typedef struct Type{
	enum{BASIC, ARRAY, STRUCT, OTHER, NOTDEF} type;
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
	int is_func;
	int is_def;
	union{
		struct{
			Type return_value;
			int argument_num;
			Type *argument_type;
		}func_info;
		Type *def_info; //symbol is basic/array/struct
	};
	struct SymbolNode *stack_next, *hash_next;
} SymbolNode;

typedef struct SymbolStackNode{
	struct SymbolNode *symbol_head;
	struct SymbolNode *func_ptr;
	struct SymbolStackNode *next;
} SymbolStackNode;

void symbolTableMain(TreeNode *p);
void buildSymbolTable(TreeNode *p);
void buildExtDef(TreeNode *p);
void buildDef(TreeNode *p);
void buildStmt(TreeNode *p);
void buildVarDec(Type, TreeNode *);
void buildFunDec(Type node_tpye, TreeNode *p);
void buildStructDef(TreeNode *p);
void buildStructVarDec(Type node_type, TreeNode *p);
Type buildSpecifier(TreeNode *p);
Type buildExp(TreeNode *p);

SymbolNode * addSymbol(const char *name);
void printType(Type t, char *str);
void freeType(Type *p);
SymbolNode *searchSymbolTable(const char *name);
SymbolNode *searchStructTable(const char *name);
StructContent *searchStructContent(StructContent *structure, const char *name);
void pushSymbolStack();
void popSymbolStack();
StructContent *pushStructContent(const char *name);
SymbolNode *pushStruct(const char *name);
void clearSymbolStack();
void clearStructTable();


extern SymbolNode *kSymbolHashTable[];
extern SymbolStackNode *kSymbolStackHead;
extern SymbolNode *kStructTableHead;


#endif

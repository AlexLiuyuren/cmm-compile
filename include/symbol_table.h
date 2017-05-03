#ifndef SYMBOL_TABLE_H_
#define SYMBOL_TABLE_H_

#define kHashSize 0x3fff

typedef struct Type{
	enum{BASIC, ARRAY, STRUCT} type;
	union{
		enum{B_INT, B_FLOAT} basic;
		struct { struct Type *element; int size;} array;
		struct StructContent *structure;
	};
} Type;


#endif

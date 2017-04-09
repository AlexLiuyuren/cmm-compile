CC := gcc
DIR_INC = ../include

all:
	bison -d syntax.y
	flex lexical.l
	$(CC) main.c syntax.tab.c syntax_tree.c -lfl -I $(DIR_INC) -o parser

#scanner : lex.yy.c main.c
#	$(CC) main.c lex.yy.c -lfl -I $(DIR_INC) -o scanner
#lex.yy.c : lexical.l syntax.tab.h
#	flex lexical.l
#syntax.tab.h : 
.PHONY: clean
clean : 
	rm lex.yy.c scanner

CC := gcc
DIR_INC = ../include

all:
	bison -d syntax.y
	flex lexical.l
	$(CC) main.c syntax.tab.c syntax_tree.c -lfl -I $(DIR_INC) -o parser

obj: 
	./parser ../test/1.1.cmm>../output/1.1.txt
	./parser ../test/1.2.cmm>../output/1.2.txt
	./parser ../test/1.3.cmm>../output/1.3.txt
	./parser ../test/1.4.cmm>../output/1.4.txt
#scanner : lex.yy.c main.c
#	$(CC) main.c lex.yy.c -lfl -I $(DIR_INC) -o scanner
#lex.yy.c : lexical.l syntax.tab.h
#	flex lexical.l
#syntax.tab.h : 
.PHONY: clean
clean : 
	rm lex.yy.c scanner

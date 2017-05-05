CC := gcc
DIR_INC = ../include

parser: syntax.tab.c main.c syntax_tree.c symbol_table.c symbol_table_toolfunction.c
	$(CC) syntax.tab.c main.c syntax_tree.c symbol_table.c symbol_table_toolfunction.c -lfl -I $(DIR_INC) -o ../parser

syntax.tab.c: syntax.y lex.yy.c
	bison -d syntax.y

lex.yy.c: lexical.l
	flex lexical.l

test: 
	./../parser ../test/1.1.cmm>../test_result/1.1.txt
	./../parser ../test/1.2.cmm>../test_result/1.2.txt
	./../parser ../test/1.3.cmm>../test_result/1.3.txt
	./../parser ../test/1.4.cmm>../test_result/1.4.txt
	./../parser ../test/1.5.cmm>../test_result/1.5.txt
	./../parser ../test/1.6.cmm>../test_result/1.6.txt
	./../parser ../test/1.7.cmm>../test_result/1.7.txt
	./../parser ../test/1.8.cmm>../test_result/1.8.txt
#scanner : lex.yy.c main.c
#	$(CC) main.c lex.yy.c -lfl -I $(DIR_INC) -o scanner
#lex.yy.c : lexical.l syntax.tab.h
#	flex lexical.l
#syntax.tab.h : 
.PHONY: clean
clean : 
	-rm syntax.tab.h syntax.tab.c lex.yy.c ../parser 

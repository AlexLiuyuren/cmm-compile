CC := gcc
DIR_INC = ../include

parser: syntax.tab.c main.c syntax_tree.c symbol_table.c symbol_table_toolfunction.c
	$(CC) -g syntax.tab.c main.c syntax_tree.c symbol_table.c symbol_table_toolfunction.c -lfl -I $(DIR_INC) -o ../parser

syntax.tab.c: syntax.y lex.yy.c
	bison -d syntax.y

lex.yy.c: lexical.l
	flex lexical.l

test1: 
	./../parser ../test/1.1.cmm>../test_result/1.1.txt
	./../parser ../test/1.2.cmm>../test_result/1.2.txt
	./../parser ../test/1.3.cmm>../test_result/1.3.txt
	./../parser ../test/1.4.cmm>../test_result/1.4.txt
	./../parser ../test/1.5.cmm>../test_result/1.5.txt
	./../parser ../test/1.6.cmm>../test_result/1.6.txt
	./../parser ../test/1.7.cmm>../test_result/1.7.txt
	./../parser ../test/1.8.cmm>../test_result/1.8.txt

test2:
	./../parser ../test/2.1.cmm>../test_result/2.1.txt
	./../parser ../test/2.2.cmm>../test_result/2.2.txt
	./../parser ../test/2.3.cmm>../test_result/2.3.txt
	./../parser ../test/2.4.cmm>../test_result/2.4.txt
	./../parser ../test/2.5.cmm>../test_result/2.5.txt
	./../parser ../test/2.6.cmm>../test_result/2.6.txt
	./../parser ../test/2.7.cmm>../test_result/2.7.txt
	./../parser ../test/2.8.cmm>../test_result/2.8.txt
	./../parser ../test/2.9.cmm>../test_result/2.9.txt
	./../parser ../test/2.10.cmm>../test_result/2.10.txt
	./../parser ../test/2.11.cmm>../test_result/2.11.txt
	./../parser ../test/2.12.cmm>../test_result/2.12.txt
#scanner : lex.yy.c main.c
#	$(CC) main.c lex.yy.c -lfl -I $(DIR_INC) -o scanner
#lex.yy.c : lexical.l syntax.tab.h
#	flex lexical.l
#syntax.tab.h : 
.PHONY: clean
clean : 
	-rm syntax.tab.h syntax.tab.c lex.yy.c ../parser 

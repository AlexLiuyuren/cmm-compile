CC := gcc
DIR_INC = include

parser: ./src/syntax.tab.c ./src/main.c ./src/syntax_tree.c
	$(CC) ./src/syntax.tab.c ./src/main.c ./src/syntax_tree.c -lfl -I $(DIR_INC) -o parser

src/syntax.tab.c src/syntax.tab.h: ./src/syntax.y ./src/lex.yy.c
	bison -d ./src/syntax.y

src/lex.yy.c: ./src/lexical.l
	flex ./src/lexical

obj: 
	./parser /test/1.1.cmm>test_result/1.1.txt
	./parser /test/1.2.cmm>test_result/1.2.txt
	./parser /test/1.3.cmm>test_result/1.3.txt
	./parser /test/1.4.cmm>test_result/1.4.txt
#scanner : lex.yy.c main.c
#	$(CC) main.c lex.yy.c -lfl -I $(DIR_INC) -o scanner
#lex.yy.c : lexical.l syntax.tab.h
#	flex lexical.l
#syntax.tab.h : 
.PHONY: clean
clean : 
	-rm syntax.tab.h syntax.tab.c

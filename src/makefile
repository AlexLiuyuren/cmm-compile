CC := gcc

scanner : lex.yy.c main.c
	$(CC) main.c lex.yy.c -lfl -o scanner
lex.yy.c : lexical.l
	flex lexical.l

.PHONY: clean
clean : 
	rm lex.yy.c scanner

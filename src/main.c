#include "stdio.h"
extern FILE* yyin;
int main(int argc, char **argv){
	if (argc <= 1) return 1;
	File *f = fopen(argv[i], "r");
	if (!f){
		perror(argv[1]);
		return 1;
	}
	yyrestart(f);
	yyparse();
	return 0;
}

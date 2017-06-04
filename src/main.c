#include "common.h"
#include "ir.h"

bool errorState;
extern FILE* yyin;
void yyrestart(FILE *);
void yyparse();
int main(int argc, char **argv){
	if (argc <= 2) return 1;
	FILE *f = fopen(argv[1], "r");
	if (!f){
		perror(argv[1]);
		return 1;
	}
	errorState = false;

	yyrestart(f);
	yyparse();

	FILE *ir = fopen(argv[2], "w+");
	if (!ir){
		perror(argv[2]);
		return 1;
	}
	if (errorState == false)
		printInterCode(ir);
	deleteInterCode();
	fclose(f);
	fclose(ir);
	return 0;
}

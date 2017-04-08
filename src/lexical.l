%{
	#include <stdlib.h>
	#include "syntax_tree.h"
	void procToken(char *symbol){
		TreeNode *p = createTreeNode();
		strcpy(p->symbol, symbol);
		strcpy(p->text, yytext);
		p->lineno = yylineno;
		if (strcmp(symbol, "INT") == 0){
			p->int_value = atoi(yytext); 
		}else if (strcmp(symbol, "INT_OCT") == 0){
			strcmp(p->symbol, "INT");
			p->int_value = strtol(yytext, NULL, 8);
		}else if (strcmp(symbol, "INT_HEX") == 0){
			strcmp(p->symbol, "INT");
			p->int_value = strtol(yytext, NULL, 16);
		}else if (strcmp(symbol, "FLOAT") == 0){
			p->float_value = atof(yytext);
		}else if (strcmp(symbol, "SCI") == 0){
			strcmp(p->symbol, "FLOAT");
			p->float_value = atof(yytext);
		}
		yylval = p;
	}
%}

TAB \t
SPACE " "
TYPE int|float

digit [0-9]
letter [_a-zA-Z]
INT 0|[1-9][0-9]*
INT_OCT 0[0-7]+
INT_HEX 0[xX][0-9a-fA-F]+
FLOAT {INT}\.[0-9]+
SCI [0-9]*\.[0-9]+[eE][+-]?{INT} 
ID [_a-zA-Z][_0-9a-zA-Z]*

SEMI ;
COMMA ,
ASSIGNOP =
RELOP >|<|>=|<=|==|!=
PLUS \+
MINUS -
STAR \*
DIV \/
AND &&
OR \|\|
DOT \.
NOT !

LP \(
RP \)
LB \[
RB \]
LC \{
RC \}

STRUCT struct
RETURN return
IF if
ELSE else
WHILE while
%%
{TAB} {printf("\t");}
{SPACE} {printf(" ");}
{TYPE} {printf("TYPE");}

{STRUCT} {printf("struct");}
{RETURN} {printf("return");}
{IF} {printf("if");}
{ELSE} {printf("else");}
{WHILE} {printf("while");}

{INT} {printf("INT");}
{INT_OCT} {printf("INT_OCT");}
{INT_HEX} {printf("INT_HEX");}
{FLOAT} { printf("FLOAT"); } 
{SCI} {printf("SCI");}
{ID} {printf("ID");}

{SEMI} {printf(";");}
{COMMA} {printf(",");}
{ASSIGNOP} {printf("=");}
{RELOP} {printf("RELOP");}
{PLUS} {printf("+");}
{MINUS} {printf("-");}
{STAR} {printf("*");}
{DIV} {printf("/");}
{AND} {printf("&&");}
{OR} {printf("||");}
{DOT} {printf(".");}
{NOT} {printf("!");}
{TYPE} {printf("TYPE");}

{LP} {printf("(");}
{RP} {printf(")");}
{LB} {printf("[");}
{RB} {printf("]");}
{LC} {printf("{");}
{RC} {printf("}");}

. {printf("Error type A at line %d: Mysterious character \'%s\'. [lexical error] \n", yylineno, yytext);}
%%


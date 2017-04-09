%{
	#include "common.h"
	#include "syntax_tree.h"
	#include "syntax.tab.h"
	int yycolumn = 1;
#define YY_USER_ACTION \
	yylloc.first_line = yylloc.last_line = yylineno; \
	yylloc.first_column = yycolumn; \
	yylloc.last_column = yycolumn + yyleng - 1; \
	yycolumn += yyleng;
	int errorState = false;
		void procToken(char *symbol){
		TreeNode *p = createTreeNode(0);
		strcpy(p->symbol, symbol);
		strcpy(p->text, yytext);
		p->lineno = yylineno;
		if (strcmp(symbol, "INT") == 0){
			p->int_value = atoi(yytext); 
		}else if (strcmp(symbol, "INT_OCT") == 0){
			strcpy(p->symbol, "INT");
			p->int_value = strtol(yytext, NULL, 8);
		}else if (strcmp(symbol, "INT_HEX") == 0){
			strcpy(p->symbol, "INT");
			p->int_value = strtol(yytext, NULL, 16);
		}else if (strcmp(symbol, "FLOAT") == 0){
			p->float_value = atof(yytext);
		}else if (strcmp(symbol, "SCI") == 0){
			strcpy(p->symbol, "FLOAT");
			p->float_value = atof(yytext);
		}
		yylval = p;
	}
%}
%option yylineno

TAB \t
SPACE " "
ENTER "\n"

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
{TAB} {;}
{SPACE} {;}
{ENTER} {yycolumn = 1;}

{TYPE} {procToken("TYPE"); return TYPE;}

{STRUCT} {procToken("STRUCT"); return STRUCT;}
{RETURN} {procToken("RETURN"); return RETURN;}
{IF} {procToken("IF"); return IF;}
{ELSE} {procToken("ELSE"); return ELSE;}
{WHILE} {procToken("WHILE"); return WHILE;}

{INT} {procToken("INT"); return INT;}
{INT_OCT} {procToken("INT_OCT"); return INT;}
{INT_HEX} {procToken("INT_HEX"); return INT;}
{FLOAT} {procToken("FLOAT"); return FLOAT;} 
{SCI} {procToken("SCI"); return FLOAT;}
{ID} {procToken("ID"); return ID;}

{SEMI} {procToken("SEMI"); return SEMI;}
{COMMA} {procToken("COMMA"); return COMMA;}
{ASSIGNOP} {procToken("ASSIGNOP"); return ASSIGNOP;}
{RELOP} {procToken("RELOP"); return RELOP;}
{PLUS} {procToken("PLUS"); return PLUS;}
{MINUS} {procToken("MINUS"); return MINUS;}
{STAR} {procToken("STAR"); return STAR;}
{DIV} {procToken("DIV"); return DIV;}
{AND} {procToken("AND"); return AND;}
{OR} {procToken("OR"); return OR;}
{DOT} {procToken("DOT"); return DOT;}
{NOT} {procToken("NOT"); return NOT;}

{LP} {procToken("LP"); return LP;}
{RP} {procToken("RP"); return RP;}
{LB} {procToken("LB"); return LB;}
{RB} {procToken("RB"); return RB;}
{LC} {procToken("LC"); return LC;}
{RC} {procToken("RC"); return RC;}

. {
	errorState = true;	
	printf("Error type A at line %d: Mysterious character \'%s\'. [lexical error] \n", yylineno, yytext);
}
%%

/*
{TAB} {printf("\t");}
{SPACE} {printf(" ");}
{ENTER} {printf("\n");}
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

{LP} {printf("(");}
{RP} {printf(")");}
{LB} {printf("[");}
{RB} {printf("]");}
{LC} {printf("{");}
{RC} {printf("}");}

. {printf("Error type A at line %d: Mysterious character \'%s\'. [lexical error] \n", yylineno, yytext);}
*/


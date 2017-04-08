%{
	#include <stdio.h>
%}
%right ASSIGNOP
%left OR
%left AND
%left RELOP
%left PLUS MINUS
%left STAR DIV
%right NOT
%left LP RP LB RB LC RC DOT

%nonassoc LOWER_THAN_ELSE
%nonassoc IF ELSE WHILE STRUCT RETURN
%nonassoc INT INT_OCT INT_HEX FLOAT SCI ID
%nonassoc SEMI COMMA

%%

%%


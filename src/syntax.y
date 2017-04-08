%{
	#include <stdio.h>
	extern int yylineno;
	TreeNode *procTreeNode(TreeNode *p, char *symbol){
		strcpy(p->symbol, symbol);
		strcpy(p->text, symbol);
		return p;
	}

	void yyerror(const char *msg){
		printf("Error type B at Line %d: %s. [syntax error]\n", yylineno, msg);
		errorState = true;
	}
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
/* High level Definition*/
Program : ExtDefList{};
ExtDefList : ExtDef ExtDefList | /*empty*/;
ExtDef : Specifier ExtDecList SEMI | Specifier SEMI | Specifier FunDec CompSt;
ExtDecList : VarDec COMMA ExtDecList | /*empty*/;

/*Specifiers*/
Specifier : TYPE | StructSpecifier;
StructSpecifier : STRUCT OptTag LC DefList RC | STRUCT Tag;
OptTag : ID | /*empty*/;
Tag : ID; 

/*Declarators*/
VarDec : ID | varDec LB INT RB;
FunDec : ID LP VarList RP;
VarList : ParamDec | ParamDec COMMA VarList;
ParamDec : Specifier VarDec;

/*Statements*/
CompSt : LC DefList StmtList RC;
StmtList : Stmt StmtList | /*empty*/;
Stmt : Exp SEMI | CompSt | RETURN Exp SEMI | IF LP Exp RP Stmt | IF LP Exp RP Stmt ELSE Stmt | WHILE LP Exp RP Stmt;

/*Local Definition*/
DefList : Def DefList | /*empty*/;
Def : Specifier DecList;
DecList : Dec | Dec COMMA DecList;
Dec : VarDec | VarDec ASSIGNOP Exp;

/*Expressions*/
Exp : Exp ASSIGNOP Exp
	| Exp AND Exp
	| Exp OR Exp
	| Exp RELOP Exp
	| Exp PLUS Exp
	| LP Exp RP
	| MINUS Exp
	| NOT Exp
	| ID LP Args RP
	| ID LP RP
	| Exp LB Exp RB
	| Exp DOT ID
	| INT
	| FLOAT
	| ID
	;
Args : Exp COMMA Args
	| Exp
	;
%%




%{
	#include "common.h"
	#include "syntax_tree.h"
	#define YYSTYPE TreeNode *
	extern int yylineno;
	TreeNode *procTreeNode(TreeNode *p, char *symbol){
		strcpy(p->symbol, symbol);
		strcpy(p->text, symbol);
		return p;
	}

%}
/*Declared tokens*/
/*%token TYPE*/
%locations
%define parse.error verbose
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
%nonassoc INT INT_OCT INT_HEX FLOAT SCI ID TYPE
%nonassoc SEMI COMMA

%%
/* High level Definition*/
Program : ExtDefList {
	$$ = procTreeNode(createTreeNode(1, $1), "Program");
	if (errorState == false){
		//printTree($$, 0);
		fflush(stdout);
		symbolTableMain($$);
		//deleteTreeNode($$);
		clearSymbolStack();
		clearStructTable();
	}
	};
ExtDefList : ExtDef ExtDefList {$$ = procTreeNode(createTreeNode(2, $1, $2), "ExtDefList");} 
	| /*empty*/ {$$ = NULL;}; 
ExtDef : Specifier ExtDecList SEMI {$$ = procTreeNode(createTreeNode(3, $1, $2, $3), "ExtDef");}
	| Specifier SEMI {$$ = procTreeNode(createTreeNode(2, $1, $2), "ExtDef");}
	| Specifier FunDec CompSt {$$ = procTreeNode(createTreeNode(3, $1, $2, $3), "ExtDef");};
ExtDecList : VarDec COMMA ExtDecList {$$ = procTreeNode(createTreeNode(3, $1, $2, $3), "ExtDecList");} 
	| VarDec {$$ = procTreeNode(createTreeNode(1, $1), "ExtDecList");}
	| error SEMI{
		yyerrok;
		errorState = true;
		$$ = procTreeNode(createTreeNode(0), "Stmt");
	}
	;

/*Specifiers*/
Specifier : TYPE {$$ = procTreeNode(createTreeNode(1, $1), "Specifier");}
	| StructSpecifier {$$ = procTreeNode(createTreeNode(1, $1), "Specifier" );};
StructSpecifier : STRUCT OptTag LC DefList RC {$$ = procTreeNode(createTreeNode(5, $1, $2, $3, $4, $5), "StructSpecifier");}
	| STRUCT Tag {$$ = procTreeNode(createTreeNode(2, $1, $2), "StructSpecifier");}; 
OptTag : ID {$$ = procTreeNode(createTreeNode(1, $1), "OptTag");}
	| /*empty*/ {$$ = NULL;};
Tag : ID {$$ = procTreeNode(createTreeNode(1, $1), "Tag");}; 

/*Declarators*/
VarDec : ID {$$ = procTreeNode(createTreeNode(1, $1), "VarDec");}
	| VarDec LB INT RB {$$ = procTreeNode(createTreeNode(4, $1, $2, $3, $4), "VarDec");};
FunDec : ID LP VarList RP {$$ = procTreeNode(createTreeNode(4, $1, $2, $3, $4), "FunDec");}
	| ID LP RP {$$ = procTreeNode(createTreeNode(3, $1, $2, $3), "FunDec");};
VarList : ParamDec {$$ = procTreeNode(createTreeNode(1, $1), "VarList");}
	| ParamDec COMMA VarList {$$ = procTreeNode(createTreeNode(3, $1, $2, $3), "VarList");};
ParamDec : Specifier VarDec {$$ = procTreeNode(createTreeNode(2, $1, $2), "ParamDec");};

/*Statements*/
CompSt : LC DefList StmtList RC{$$ = procTreeNode(createTreeNode(4, $1, $2, $3, $4), "CompSt");}
	| error RC{
		yyerrok;
		errorState = true;
		$$ = procTreeNode(createTreeNode(0), "CompSt");
	}
	;
StmtList : Stmt StmtList {$$ = procTreeNode(createTreeNode(2, $1, $2), "StmtList");}
	| /*empty*/ {$$ = NULL;};
Stmt : Exp SEMI {$$ = procTreeNode(createTreeNode(2, $1, $2), "Stmt");}
	| CompSt {$$ = procTreeNode(createTreeNode(1, $1), "Stmt");}
	| RETURN Exp SEMI {$$ = procTreeNode(createTreeNode(3, $1, $2, $3), "Stmt");}
	| IF LP Exp RP Stmt %prec LOWER_THAN_ELSE {$$ = procTreeNode(createTreeNode(5, $1, $2, $3, $4), "Stmt");}
	| IF LP Exp RP Stmt ELSE Stmt {$$ = procTreeNode(createTreeNode(7, $1, $2, $3, $4, $5, $6, $7), "Stmt");}
	| WHILE LP Exp RP Stmt{$$ = procTreeNode(createTreeNode(5, $1, $2, $3, $4, $5), "Stmt");}
	| error SEMI{
		yyerrok;
		errorState = true;
		$$ = procTreeNode(createTreeNode(0), "Stmt");
	}
	;

/*Local Definition*/
DefList : Def DefList {$$ = procTreeNode(createTreeNode(2, $1, $2), "DefList");}
	| /*empty*/ {$$ = NULL;};
Def : Specifier DecList SEMI {$$ = procTreeNode(createTreeNode(3, $1, $2, $3), "Def");};
DecList : Dec {$$ = procTreeNode(createTreeNode(1, $1), "DecList");} 
	| Dec COMMA DecList{$$ = procTreeNode(createTreeNode(3, $1, $2, $3), "DecList" );};
Dec : VarDec {$$ = procTreeNode(createTreeNode(1, $1), "Dec");}
	| VarDec ASSIGNOP Exp {$$ = procTreeNode(createTreeNode(3, $1, $2, $3), "Dec");};

/*Expressions*/
Exp : Exp ASSIGNOP Exp {$$ = procTreeNode(createTreeNode(3, $1, $2, $3), "Exp");}
	| Exp AND Exp {$$ = procTreeNode(createTreeNode(3, $1, $2, $3), "Exp");}
	| Exp OR Exp {$$ = procTreeNode(createTreeNode(3, $1, $2, $3), "Exp");}
	| Exp RELOP Exp {$$ = procTreeNode(createTreeNode(3, $1, $2, $3), "Exp");}
	| Exp PLUS Exp {$$ = procTreeNode(createTreeNode(3, $1, $2, $3), "Exp");}
	| Exp MINUS Exp {$$ = procTreeNode(createTreeNode(3, $1, $2, $3), "Exp");}
	| Exp STAR Exp {$$ = procTreeNode(createTreeNode(3, $1, $2, $3), "Exp");}
	| Exp DIV Exp {$$ = procTreeNode(createTreeNode(3, $1, $2, $3), "Exp");}
	| LP Exp RP {$$ = procTreeNode(createTreeNode(3, $1, $2, $3), "Exp");}
	| MINUS Exp {$$ = procTreeNode(createTreeNode(2, $1, $2), "Exp");}
	| NOT Exp {$$ = procTreeNode(createTreeNode(2, $1, $2), "Exp");}
	| ID LP Args RP {$$ = procTreeNode(createTreeNode(4, $1, $2, $3, $4), "Exp");}
	| ID LP RP {$$ = procTreeNode(createTreeNode(3, $1, $2, $3), "Exp");}
	| Exp LB Exp RB {$$ = procTreeNode(createTreeNode(4, $1, $2, $3, $4), "Exp");}
	| Exp DOT ID {$$ = procTreeNode(createTreeNode(3, $1, $2, $3), "Exp");}
	| INT {$$ = procTreeNode(createTreeNode(1, $1), "Exp");}
	| FLOAT {$$ = procTreeNode(createTreeNode(1, $1), "Exp");}
	| ID {$$ = procTreeNode(createTreeNode(1, $1), "Exp");}
	| error RP{
		yyerrok;
		errorState = true;
		$$ = procTreeNode(createTreeNode(0), "Exp");
	}
	;
	;
Args : Exp COMMA Args {$$ = procTreeNode(createTreeNode(3, $1, $2, $3), "Args");}
	| Exp {$$ = procTreeNode(createTreeNode(1, $1), "Args");}
	;
%%
#include "lex.yy.c"
//#include "syntax_tree.h"
	void yyerror(const char *msg){
		printf("Error type B at Line %d: %s. [syntax error]\n", yylloc.first_line, msg);
		errorState = true;
	}


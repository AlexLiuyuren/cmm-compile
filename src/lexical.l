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
PLUS +
MINUS -
STAR *
DIV /
AND &&
OR ||
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

{INT} {printf("INT");}
{INT_OCT} {printf("INT_OCT");}
{INT_HEX} {printf("INT_HEX");}
{FLOAT} {printf("FLOAT";)} 
{SCI} {printf("SCI";)}
{ID} {printf("ID";)}

{SEMI} {printf("SEMI");}
{COMMA} {printf("COMMA");}
{ASSIGNOP} {printf("ASSIGNOP");}
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

{STRUCT} {printf("struct");}
{RETURN} {printf("return");}
{IF} {printf("if");}
{ELSE} {printf("else");}
{WHILE} {printf("while");}
%%


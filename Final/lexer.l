%{
#include <stdio.h>
#include "tokens.h"
%}

%option yylineno

%%

[ \t\r]+                ;

\n                      { return TK_NL; }

"fun"                   { return TK_FUN; }
"if"                    { return TK_IF; }
"else"                  { return TK_ELSE; }
"end"                   { return TK_END; }
"while"                 { return TK_WHILE; }
"loop"                  { return TK_LOOP; }
"return"                { return TK_RETURN; }
"new"                   { return TK_NEW; }

"true"                  { return TK_TRUE; }
"false"                 { return TK_FALSE; }

"int"                   { return TK_INT; }
"bool"                  { return TK_BOOL; }
"char"                  { return TK_CHAR; }
"string"                { return TK_STRING; }

"and"                   { return TK_AND; }
"or"                    { return TK_OR; }
"not"                   { return TK_NOT; }

"=="                    { return TK_EQ; }
"<>"                    { return TK_NEQ; }
"<="                    { return TK_LE; }
">="                    { return TK_GE; }
"<"                     { return TK_LT; }
">"                     { return TK_GT; }

"="                     { return TK_ASSIGN; }
"+"                     { return TK_PLUS; }
"-"                     { return TK_MINUS; }
"*"                     { return TK_MUL; }
"/"                     { return TK_DIV; }

"("                     { return TK_LPAREN; }
")"                     { return TK_RPAREN; }
"["                     { return TK_LBRACKET; }
"]"                     { return TK_RBRACKET; }

":"                     { return TK_COLON; }
","                     { return TK_COMMA; }

[0-9]+                  { return TK_LITNUM; }

\"([^"\\]|\\.)*\"   { return TK_LITSTRING; }

[a-zA-Z_][a-zA-Z0-9_]*  { return TK_ID; }

.                       { return TK_ERROR; }

%%

int yywrap() { return 1; }
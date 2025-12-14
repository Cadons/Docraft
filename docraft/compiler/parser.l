%{
#include "bison/grammar.tab.h"
%}
%option c++
%option noyywrap
%option yylineno
%%

"program"       { return PROGRAM; }
"print"         { return PRINT; }
"{"             { return OPEN_BRACE; }
"}"             { return CLOSE_BRACE; }
"("             { return OPEN_PAREN; }
")"             { return CLOSE_PAREN; }
";"             { return SEMICOLON; }
\"[^\"]*\"      { yylval.str = strdup(yytext); return STRING_LITERAL; }
"if"            { return IF; }
"else"          { return ELSE; }
"=="            { return EQUALS; }
"!="            { return NOT_EQUALS; }
">"             { return GREATER; }
"<"             { return LESS; }
">="         { return GREATER_EQUALS; }
"<="         { return LESS_EQUALS; }
\<[a-zA-Z_]*\>          { return HTML_TAG_START; }
\<\/[a-zA-Z_]*\>       { return HTML_TAG_END; }
[a-zA-Z_][a-zA-Z0-9_]*  { return IDENTIFIER; }
[1-9][0-9]*       { yylval.str = strdup(yytext); return INTEGER_LITERAL; }
[0-9]+(\.[0-9]+)?  {
   yylval.str = strdup(yytext);
    return FLOAT_LITERAL;
}
"//".*          { /* ignora commenti su singola linea */ }
"/*"([^*]|\*+[^*/])*\*+"/"  { /* ignora commenti multi-linea */ }
[ \t\n]+        { /* ignora spazi bianchi */ }
.               { /* ignora caratteri sconosciuti */ }

%%

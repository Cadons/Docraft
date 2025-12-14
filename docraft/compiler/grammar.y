%{
   #include <iostream>
   #include <FlexLexer.h> // Include FlexLexer header
   #include <vector>
   void yyerror(const char* s);
   static yyFlexLexer* scanner;

   extern "C" int yylex()
   {
       return scanner->yylex();
   }
   void setScanner(yyFlexLexer* s) {
       scanner = s;
   }
    std::vector<std::string> commands;
enum ComparisonOp {
    OP_EQUALS = 1,
    OP_NOT_EQUALS,
    OP_LESS,
    OP_LESS_EQUALS,
    OP_GREATER,
    OP_GREATER_EQUALS
};
%}
%start program
%token PROGRAM
%token IDENTIFIER
%token OPEN_BRACE
%token CLOSE_BRACE
%token SEMICOLON
%token PRINT
%token OPEN_PAREN
%token CLOSE_PAREN
%token HTML_TAG_START
%token HTML_TAG_END
%token IF
%token ELSE
%token EQUALS
%token NOT_EQUALS
%token LESS
%token LESS_EQUALS
%token GREATER
%token GREATER_EQUALS
%union {
    char* str;
    int op;
}
%token <str> STRING_LITERAL
%token <str> INTEGER_LITERAL
%token <str> FLOAT_LITERAL
%type <op> comparison_operator
%type <str> value
%%

program:  PROGRAM IDENTIFIER OPEN_BRACE statement_list CLOSE_BRACE {
commands.push_back("Program");}
        | PROGRAM IDENTIFIER OPEN_BRACE CLOSE_BRACE {
commands.push_back("Program");}
        ;

statement_list:  statement {}
               | statement_list statement {}
               ;

statement:  PRINT OPEN_PAREN STRING_LITERAL CLOSE_PAREN SEMICOLON {
                std::cout << $3 << "\n";
                free($3);
                commands.push_back("Print");
            }
          | HTML_TAG_START html_content HTML_TAG_END {
                std::cout << "Html detected\n";
                commands.push_back("Html");
            }
          | HTML_TAG_START HTML_TAG_END SEMICOLON {
                commands.push_back("Html");
                std::cout << "Empty Html detected\n";
            }
         |  IF OPEN_PAREN STRING_LITERAL comparison_operator value CLOSE_PAREN OPEN_BRACE statement_list CLOSE_BRACE {
                std::cout << "If statement: " << $3;
                switch($4) {
                    case OP_EQUALS: std::cout << " == "; break;
                    case OP_NOT_EQUALS: std::cout << " != "; break;
                    case OP_LESS: std::cout << " < "; break;
                    case OP_LESS_EQUALS: std::cout << " <= "; break;
                    case OP_GREATER: std::cout << " > "; break;
                    case OP_GREATER_EQUALS: std::cout << " >= "; break;
                }
std::cout << $5 << "\n";
                free($3);
commands.push_back("If");
            }
         | IF OPEN_PAREN STRING_LITERAL comparison_operator value CLOSE_PAREN OPEN_BRACE statement_list CLOSE_BRACE ELSE OPEN_BRACE statement_list CLOSE_BRACE {
 std::cout << "If statement: " << $3;
                switch($4) {
                    case OP_EQUALS: std::cout << " == "; break;
                    case OP_NOT_EQUALS: std::cout << " != "; break;
                    case OP_LESS: std::cout << " < "; break;
                    case OP_LESS_EQUALS: std::cout << " <= "; break;
                    case OP_GREATER: std::cout << " > "; break;
                    case OP_GREATER_EQUALS: std::cout << " >= "; break;
                }
std::cout << $5 << "\n";
                std::cout << "If-Else statement\n";
                free($3);
                free($5);
                commands.push_back("If-Else");
            }
          ;
html_content: /* empty */ {}
            | html_content STRING_LITERAL { std::cout << $2 << "\n"; free($2); }
            | html_content statement {}
            ;
comparison_operator: EQUALS { $$ = OP_EQUALS; }//assigns the value of the token to $$ (yacc variable for the left-hand side)
                   | NOT_EQUALS { $$ = OP_NOT_EQUALS; }
                   | LESS { $$ = OP_LESS; }
                   | LESS_EQUALS { $$ = OP_LESS_EQUALS; }
                   | GREATER { $$ = OP_GREATER; }
                   | GREATER_EQUALS { $$ = OP_GREATER_EQUALS; }
                   ;
value: INTEGER_LITERAL { $$ = $1; }
     | FLOAT_LITERAL { $$ = $1; }
     | STRING_LITERAL { $$ = $1; }
     ;
%%
void yyerror(const char* s) {
    std::cerr << "Error: " <<  scanner->YYText() << " at line: " << scanner->lineno() << std::endl;
}

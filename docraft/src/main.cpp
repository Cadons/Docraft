#include <FlexLexer.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "bison/grammar.tab.h"
extern void setScanner(yyFlexLexer* s); // Dichiara la funzione
extern std::vector<std::string> commands;
int main() {
    //requere file path
    std::string filePath;
    std::cout << "Enter file path: ";
    std::cin >> filePath;
    std::string fileContent;
    //read file content
    std::ifstream file(filePath);
    //set input for lexer
    auto lexer = new yyFlexLexer(&file);// Instantiate the lexer
    lexer->set_debug(true);
    setScanner(lexer); // Set the lexer for Bison
    yyparse();// Start parsing
    for (const auto& cmd : commands) {
        std::cout << "Command: " << cmd << std::endl;
    }

    delete lexer;// Clean up
    return 0;
}

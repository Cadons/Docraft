#include <iostream>
#include <string>

#include "docraft_document.h"
#include "craft/docraft_craft_language_parser.h"

int main() {
    try {
        std::cout << "DOCRAFT generator" << std::endl;
        docraft::craft::DocraftCraftLanguageParser parser;
        parser.load_from_file("test.craft");
        parser.get_document()->set_document_title("test");
        parser.get_document()->render();
        return 0;
    } catch (const std::exception &ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
        return 1;
    }
}

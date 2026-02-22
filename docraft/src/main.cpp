#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "templating/docraft_template_engine.h"

#include "docraft_document.h"
#include "craft/docraft_craft_language_parser.h"
int main() {
    try {
        std::cout << "DOCRAFT generator" << std::endl;
        docraft::craft::DocraftCraftLanguageParser parser;
        auto template_engine = std::make_shared<docraft::templating::DocraftTemplateEngine>();
template_engine->add_template_variable("test","[{\"name\": \"Alice\", \"role\": \"Developer\"}, {\"name\": \"Bob\", \"role\": \"Designer\"}]");
        parser.load_from_file("test.craft");
        parser.get_document()->set_document_template_engine(template_engine);
        parser.get_document()->set_document_title("test");
        docraft::DocraftDocumentMetadata metadata;
        metadata.set_title("Test Document");
        metadata.set_author("Docraft");
        metadata.set_subject("Testing Docraft Craft Language Parser");
        parser.get_document()->set_document_metadata(metadata);
        parser.get_document()->enable_auto_keywords();
        parser.get_document()->set_document_path("..");
        auto img = parser.get_document()->get_by_name("myimg");
        parser.get_document()->render();
        return 0;
    } catch (const std::exception &ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
        return 1;
    }
}

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
        // Base64 2x1 RGB image: black then white.
        //read a file raw file and put in image data
        std::string path = "test_raw.raw";
        std::vector<unsigned char> raw_data;
        try {
            std::ifstream file(path, std::ios::binary);
            if (!file) {                std::cerr << "Failed to open file: " << path << std::endl;
                return 1;
            }
            file.seekg(0, std::ios::end);
            std::streamsize size = file.tellg();
            file.seekg(0, std::ios::beg);
            raw_data.resize(size);
            if (!file.read(reinterpret_cast<char*>(raw_data.data()), size)) {
                std::cerr << "Failed to read file: " << path << std::endl;
                return 1;
            }
        } catch (const std::exception &e) {
            std::cerr << "Error reading file: " << e.what() << std::endl;
            return 1;
        }
        template_engine->add_image_data("raw_image", raw_data, 1024, 1024);

        parser.load_from_file("test.craft");
        parser.get_document()->set_document_template_engine(template_engine);
        parser.get_document()->set_document_title("test");
        auto img = parser.get_document()->get_by_name("myimg");
        parser.get_document()->render();
        return 0;
    } catch (const std::exception &ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
        return 1;
    }
}

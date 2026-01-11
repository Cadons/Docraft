#pragma once
#include <pugixml.hpp>
#include <string>

#include "docraft_document.h"
#include "i_docraft_parser.h"
#include <memory>
#include <unordered_map>

namespace docraft::craft {
    class DocraftCraftLanguageParser {
    public:
        DocraftCraftLanguageParser();

        ~DocraftCraftLanguageParser() = default;

        void parse(const std::string &craft_language_source);

        void load_from_file(const std::string &file_path);

        std::shared_ptr<DocraftDocument> get_document() const;

    private:
        void load_document();

        std::shared_ptr<model::DocraftNode> parse_node(const pugi::xml_node &xml_node);

        /**
         * @brief Debug function to print the XML node structure
         * @param node node to debug
         * @param indent indentation level
         */
        static void debug_node(const pugi::xml_node &node, int indent = 0);

        /**
         * @brief Print the XML tree structure for debugging
         * @param node root node
         * @param indent indentation level
         */
        static void print_xml_tree(const pugi::xml_node &node, int indent = 0);
        static std::string tag_formatter(const std::string &tag_name);

        pugi::xml_document xml_doc_;
        std::shared_ptr<DocraftDocument> document_ = nullptr;
        std::unordered_map<std::string, std::unique_ptr<IDocraftParser>> parsers_;
    };
} // docraft

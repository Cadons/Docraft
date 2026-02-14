#pragma once
#include <pugixml.hpp>
#include <string>

#include "docraft_document.h"
#include "i_docraft_parser.h"
#include <memory>
#include <unordered_map>

namespace docraft::craft {
    /**
     * @brief Parses the Docraft Craft Language into a DocraftDocument.
     *
     * Uses an XML parser (pugixml) and a registry of node parsers to
     * build the document tree.
     */
    class DocraftCraftLanguageParser {
    public:
        /**
         * @brief Constructs a parser and registers default node parsers.
         */
        DocraftCraftLanguageParser();

        ~DocraftCraftLanguageParser() = default;

        /**
         * @brief Parses craft language source as a string.
         * @param craft_language_source XML source as string.
         */
        void parse(const std::string &craft_language_source);

        /**
         * @brief Loads and parses craft language source from file.
         * @param file_path Path to XML file.
         */
        void load_from_file(const std::string &file_path);

        /**
         * @brief Returns the parsed document.
         * @return Parsed document or nullptr if not parsed.
         */
        std::shared_ptr<DocraftDocument> get_document() const;

    private:
        /**
         * @brief Constructs the document from the parsed XML.
         */
        void load_document();

        /**
         * @brief Parses a single XML node into a Docraft node.
         * @param xml_node XML node.
         * @return Parsed Docraft node.
         */
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
        /**
         * @brief Normalizes a tag name for parser lookup.
         * @param tag_name Raw tag name.
         * @return Normalized tag name.
         */
        static std::string tag_formatter(const std::string &tag_name);

        pugi::xml_document xml_doc_;
        std::shared_ptr<DocraftDocument> document_ = nullptr;
        std::unordered_map<std::string, std::unique_ptr<IDocraftParser>> parsers_;
    };
} // docraft

/*
 * Copyright 2026 Matteo Cadoni (https://github.com/cadons)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include "docraft/docraft_lib.h"
#include <pugixml.hpp>
#include <string>

#include "docraft/docraft_document.h"
#include "docraft/craft/i_docraft_parser.h"
#include <memory>
#include <unordered_map>

namespace docraft::craft {
    /**
     * @brief Parses the Docraft Craft Language into a DocraftDocument.
     *
     * Uses an XML parser (pugixml) and a registry of node parsers to
     * build the document tree.
     */
    class DOCRAFT_LIB DocraftCraftLanguageParser {
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

        /**
         * @brief Parses a single XML node into a Docraft node.
         * @param xml_node XML node.
         * @return Parsed Docraft node.
         */
        std::shared_ptr<model::DocraftNode> parse_node(const pugi::xml_node &xml_node);

    private:
        /**
         * @brief Constructs the document from the parsed XML.
         */
        void load_document();

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

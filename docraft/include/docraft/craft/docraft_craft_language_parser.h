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

#include <memory>
#include <string>
#include <unordered_map>

#include "docraft/docraft_lib.h"
#include <pugixml.hpp>

#include "docraft/craft/docraft_craft_parsed_element.h"
#include "docraft/craft/i_docraft_parser.h"

namespace docraft::craft {
    /**
     * @brief Parses the Craft Language into a generic `DocraftParsedElement` tree.
     *
     * Fully engine-agnostic: zero dependency on any rendering/layout engine (in
     * particular, no `docraft::loom` include anywhere). For each XML element, the
     * registered `IDocraftParser` produces the tag-specific payload, common attributes
     * are parsed once generically, and children are recursed into -- except `<Table>`,
     * whose row/cell structure is parsed entirely inside `DocraftTableParser` itself (see
     * `ParsedTableData`), so it is not recursed into generically.
     *
     * Turning a parsed tree into actual loom nodes is the job of the separate
     * `docraft::loom::craft::DocraftLoomTreeBuilder`, not this class.
     */
    class DOCRAFT_LIB DocraftCraftLanguageParser {
    public:
        /**
         * @brief Constructs a parser and registers the default tag parsers.
         */
        DocraftCraftLanguageParser();

        ~DocraftCraftLanguageParser() = default;

        DocraftCraftLanguageParser(const DocraftCraftLanguageParser&) = delete;
        DocraftCraftLanguageParser& operator=(const DocraftCraftLanguageParser&) = delete;

        /**
         * @brief Parses craft language source (a single root element) as a string.
         * @param craft_language_source XML source as string.
         * @return The parsed tree rooted at the document's root element.
         */
        std::shared_ptr<DocraftParsedElement> parse(const std::string& craft_language_source);

        /**
         * @brief Loads and parses craft language source from file.
         * @param file_path Path to XML file.
         * @return The parsed tree rooted at the document's root element.
         */
        std::shared_ptr<DocraftParsedElement> load_from_file(const std::string& file_path);

        /**
         * @brief Parses a single XML element (and, generically, its subtree) into a
         * `DocraftParsedElement`.
         * @param xml_node XML node.
         * @return The parsed element.
         * @throws docraft::exception::DataFormatException if no parser is registered for
         * the element's tag name.
         * @throws docraft::exception::InvalidInputException on structurally invalid
         * nesting (e.g. a Text-like tag containing another Text-like child, or a non-Text
         * child of a List).
         */
        std::shared_ptr<DocraftParsedElement> parse_node(const pugi::xml_node& xml_node);

    private:
        pugi::xml_document xml_doc_;
        std::unordered_map<std::string, std::unique_ptr<IDocraftParser>> parsers_;
    };
} // namespace docraft::craft

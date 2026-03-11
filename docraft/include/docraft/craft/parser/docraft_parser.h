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
#include "docraft/craft/docraft_craft_language_parser.h"
#include "docraft/craft/i_docraft_parser.h"
#include "docraft/craft/parser/docraft_circle_parser.h"
#include "docraft/craft/parser/docraft_line_parser.h"
#include "docraft/craft/parser/docraft_polygon_parser.h"
#include "docraft/craft/parser/docraft_triangle_parser.h"
#include "docraft/model/docraft_rectangle.h"
#include "docraft/model/docraft_settings.h"

namespace docraft::craft::parser {
    /**
     * @brief Base parser for rectangle-like nodes.
     */
    class DOCRAFT_LIB DocraftRectangleParser : public IDocraftParser {
    public:
        /**
         * @brief Parses a rectangle XML node into a DocraftRectangle.
         * @param craft_language_source XML node.
         * @return Parsed rectangle node.
         */
        std::shared_ptr<model::DocraftNode> parse(const pugi::xml_node &craft_language_source) override;
        /**
         * @brief Copies rectangle attributes from one node to another.
         * @param from Source rectangle.
         * @param to Destination rectangle.
         */
        static void apply_attributes_to(const std::shared_ptr<model::DocraftRectangle>& from, const std::shared_ptr<model::DocraftRectangle>& to);
    };
    /**
     * @brief Parser for header sections.
     */
    class DOCRAFT_LIB DocraftHeaderParser : public DocraftRectangleParser {
    public:
        /**
         * @brief Parses a header XML node.
         * @param craft_language_source XML node.
         * @return Parsed header node.
         */
        std::shared_ptr<model::DocraftNode> parse(const pugi::xml_node &craft_language_source) override;
    };

    /**
     * @brief Parser for body sections.
     */
    class DOCRAFT_LIB DocraftBodyParser : public DocraftRectangleParser {
    public:
        /**
         * @brief Parses a body XML node.
         * @param craft_language_source XML node.
         * @return Parsed body node.
         */
        std::shared_ptr<model::DocraftNode> parse(const pugi::xml_node &craft_language_source) override;
    };

    /**
     * @brief Parser for footer sections.
     */
    class DOCRAFT_LIB DocraftFooterParser : public DocraftRectangleParser {
    public:
        /**
         * @brief Parses a footer XML node.
         * @param craft_language_source XML node.
         * @return Parsed footer node.
         */
        std::shared_ptr<model::DocraftNode> parse(const pugi::xml_node &craft_language_source) override;
    };
    /**
     * @brief Parser for layout nodes.
     */
    class DOCRAFT_LIB DocraftLayoutParser : public IDocraftParser {
    public:
        /**
         * @brief Parses a layout XML node.
         * @param craft_language_source XML node.
         * @return Parsed layout node.
         */
        std::shared_ptr<model::DocraftNode> parse(const pugi::xml_node &craft_language_source) override;
    };
    /**
     * @brief Parser for text nodes.
     */
    class DOCRAFT_LIB DocraftTextParser : public IDocraftParser {
    public:
        /**
         * @brief Parses a text XML node.
         * @param craft_language_source XML node.
         * @return Parsed text node.
         */
        std::shared_ptr<model::DocraftNode> parse(const pugi::xml_node &craft_language_source) override;
    };
    /**
     * @brief Parser for page number nodes.
     */
    class DOCRAFT_LIB DocraftPageNumberParser : public IDocraftParser {
    public:
        /**
         * @brief Parses a page number XML node.
         * @param craft_language_source XML node.
         * @return Parsed page number node.
         */
        std::shared_ptr<model::DocraftNode> parse(const pugi::xml_node &craft_language_source) override;
    };
    /**
     * @brief Parser for image nodes.
     */
    class DOCRAFT_LIB DocraftImageParser : public IDocraftParser {
    public:
        /**
         * @brief Parses an image XML node.
         * @param craft_language_source XML node.
         * @return Parsed image node.
         */
        std::shared_ptr<model::DocraftNode> parse(const pugi::xml_node &craft_language_source) override;
    };
    /**
     * @brief Parser for table nodes.
     */
    class DOCRAFT_LIB DocraftTableParser : public IDocraftParser {
    public:
        /**
         * @brief Parses a table XML node.
         * @param craft_language_source XML node.
         * @return Parsed table node.
         */
        std::shared_ptr<model::DocraftNode> parse(const pugi::xml_node &craft_language_source) override;
    };
    /**
     * @brief Parser for list nodes.
     */
    class DOCRAFT_LIB DocraftListParser : public IDocraftParser {
    public:
        /**
         * @brief Parses a list XML node.
         * @param craft_language_source XML node.
         * @return Parsed list node.
         */
        std::shared_ptr<model::DocraftNode> parse(const pugi::xml_node &craft_language_source) override;
    };
    /**
     * @brief Parser for unordered list nodes.
     */
    class DOCRAFT_LIB DocraftUListParser : public IDocraftParser {
    public:
        /**
         * @brief Parses an unordered list XML node.
         * @param craft_language_source XML node.
         * @return Parsed unordered list node.
         */
        std::shared_ptr<model::DocraftNode> parse(const pugi::xml_node &craft_language_source) override;
    };

    /**
     * @brief Parser for blank line nodes.
     */
    class DOCRAFT_LIB DocraftBlackLineParser : public IDocraftParser {
    public:
        /**
         * @brief Parses a blank line XML node.
         * @param craft_language_source XML node.
         * @return Parsed blank line node.
         */
        std::shared_ptr<model::DocraftNode> parse(const pugi::xml_node &craft_language_source) override;
    };
    /**
     * @brief Parser for manual page break nodes.
     */
    class DOCRAFT_LIB DocraftNewPageParser : public IDocraftParser {
    public:
        /**
         * @brief Parses a NewPage XML node.
         * @param craft_language_source XML node.
         * @return Parsed NewPage node.
         */
        std::shared_ptr<model::DocraftNode> parse(const pugi::xml_node &craft_language_source) override;
    };
    /**
     * @brief Parser for settings nodes.
     */
    class DOCRAFT_LIB DocraftSettingsParser : public IDocraftParser {
    public:
        /**
         * @brief Parses a settings XML node.
         * @param craft_language_source XML node.
         * @return Parsed settings node.
         */
        std::shared_ptr<model::DocraftNode> parse(const pugi::xml_node &craft_language_source) override;
    };
    /**
     * @brief Parser for foreach nodes.
     * @param craft_language_source XML node.
     * @return Parsed foreach node.
     */
    class DOCRAFT_LIB DocraftForeachParser : public IDocraftParser {
    public:
        DocraftForeachParser(DocraftCraftLanguageParser* craft_language_parser);
        /**
         * @brief Parses a foreach XML node.
         * @param craft_language_source XML node.
         * @return Parsed foreach node.
         */
        std::shared_ptr<model::DocraftNode> parse(const pugi::xml_node &craft_language_source) override;
    private:
        DocraftCraftLanguageParser* craft_language_parser_;
    };

} // docraft

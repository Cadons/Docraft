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

#include "docraft/craft/docraft_craft_language_parser.h"

#include "docraft/craft/docraft_craft_language_tokens.h"
#include "docraft/craft/parser/docraft_circle_parser.h"
#include "docraft/craft/parser/docraft_foreach_parser.h"
#include "docraft/craft/parser/docraft_line_parser.h"
#include "docraft/craft/parser/docraft_paragraph_parser.h"
#include "docraft/craft/parser/docraft_parser.h"
#include "docraft/craft/parser/docraft_parser_helpers.h"
#include "docraft/craft/parser/docraft_polygon_parser.h"
#include "docraft/craft/parser/docraft_section_parsers.h"
#include "docraft/craft/parser/docraft_triangle_parser.h"
#include "docraft/exception/docraft_exceptions.h"

namespace docraft::craft {

DocraftCraftLanguageParser::DocraftCraftLanguageParser() {
    parsers_[std::string{elements::kRectangle}] = std::make_unique<parser::DocraftRectangleParser>();
    parsers_[std::string{elements::kCanvas}] = std::make_unique<parser::DocraftRectangleParser>();
    parsers_[std::string{elements::kCircle}] = std::make_unique<parser::DocraftCircleParser>();
    parsers_[std::string{elements::kTriangle}] = std::make_unique<parser::DocraftTriangleParser>();
    parsers_[std::string{elements::kLine}] = std::make_unique<parser::DocraftLineParser>();
    parsers_[std::string{elements::kPolygon}] = std::make_unique<parser::DocraftPolygonParser>();
    parsers_[std::string{elements::kText}] = std::make_unique<parser::DocraftTextParser>();
    parsers_[std::string{elements::kTitle}] = std::make_unique<parser::DocraftTextParser>();
    parsers_[std::string{elements::kSubtitle}] = std::make_unique<parser::DocraftTextParser>();
    parsers_[std::string{elements::kPageNumber}] = std::make_unique<parser::DocraftPageNumberParser>();
    parsers_[std::string{elements::kImage}] = std::make_unique<parser::DocraftImageParser>();
    parsers_[std::string{elements::kTable}] = std::make_unique<parser::DocraftTableParser>();
    parsers_[std::string{elements::kList}] = std::make_unique<parser::DocraftListParser>();
    parsers_[std::string{elements::kUList}] = std::make_unique<parser::DocraftUListParser>();
    parsers_[std::string{elements::kBlankLine}] = std::make_unique<parser::DocraftBlackLineParser>();
    parsers_[std::string{elements::kLayout}] = std::make_unique<parser::DocraftLayoutParser>();
    parsers_[std::string{elements::kParagraph}] = std::make_unique<parser::DocraftParagraphParser>();
    parsers_[std::string{section::kHeader}] = std::make_unique<parser::DocraftSectionParser>();
    parsers_[std::string{section::kBody}] = std::make_unique<parser::DocraftSectionParser>();
    parsers_[std::string{section::kFooter}] = std::make_unique<parser::DocraftSectionParser>();
    parsers_[std::string{elements::kNewPage}] = std::make_unique<parser::DocraftNewPageParser>();
    parsers_[std::string{elements::templating::kForeach}] = std::make_unique<parser::DocraftForeachParser>();
    // Settings/Metadata/Document itself are not registered here -- Settings is Phase 4,
    // and Document is walked directly (via pugixml, not through this per-tag registry) by
    // docraft::craft::DocraftLoomCraftLanguageParser, which picks out only its
    // Header/Body/Footer children and ignores everything else at that top level.
}

std::shared_ptr<DocraftParsedElement> DocraftCraftLanguageParser::parse(const std::string& craft_language_source)
{
    xml_doc_ = pugi::xml_document();
    const pugi::xml_parse_result result = xml_doc_.load_string(craft_language_source.c_str());
    if (!result) {
        throw docraft::exception::DataFormatException(
            "Error parsing .craft content: " + std::string(result.description()));
    }
    const pugi::xml_node root = xml_doc_.first_child();
    if (!root)
    {
        throw docraft::exception::DataFormatException("Invalid .craft content: no root element");
    }
    return parse_node(root);
}

std::shared_ptr<DocraftParsedElement> DocraftCraftLanguageParser::load_from_file(const std::string& file_path)
{
    xml_doc_ = pugi::xml_document();
    const pugi::xml_parse_result result = xml_doc_.load_file(file_path.c_str());
    if (!result) {
        throw docraft::exception::DataFormatException(
            "Error loading .craft file: " + std::string(result.description()));
    }
    const pugi::xml_node root = xml_doc_.first_child();
    if (!root)
    {
        throw docraft::exception::DataFormatException("Invalid .craft file: no root element");
    }
    return parse_node(root);
}

std::shared_ptr<DocraftParsedElement> DocraftCraftLanguageParser::parse_node(const pugi::xml_node& xml_node)
{
    const std::string node_name = xml_node.name();
    const auto it = parsers_.find(node_name);
    if (it == parsers_.end()) {
        throw docraft::exception::DataFormatException("No parser registered for node: " + node_name);
    }

    auto element = std::make_shared<DocraftParsedElement>();
    element->tag_name = node_name;
    element->common = parser::detail::parse_common_node_attributes(xml_node);
    element->data = it->second->parse(xml_node);

    // Table owns its row/cell structure internally (see ParsedTableData) -- it is not
    // recursed into generically.
    if (node_name == std::string{elements::kTable})
    {
        return element;
    }

    for (pugi::xml_node child : xml_node.children())
    {
        if (child.type() != pugi::node_element)
        {
            continue;
        }
        const std::string child_name = child.name();

        if (node_name == std::string{elements::kList} || node_name == std::string{elements::kUList})
        {
            if (child_name != std::string{elements::kText})
            {
                throw docraft::exception::InvalidInputException(child_name + " cannot be placed in a list");
            }
        }

        // Text/Title/Subtitle/PageNumber are leaf nodes: they cannot contain other
        // Text-like children.
        if (node_name == std::string{elements::kText} || node_name == std::string{elements::kTitle} ||
            node_name == std::string{elements::kSubtitle} || node_name == std::string{elements::kPageNumber})
        {
            if (child_name == std::string{elements::kText} || child_name == std::string{elements::kTitle} ||
                child_name == std::string{elements::kSubtitle} || child_name == std::string{elements::kPageNumber})
            {
                throw docraft::exception::InvalidInputException(
                    "Text nodes cannot contain child <" + child_name + "> nodes. " +
                    "Text is a leaf node and only accepts text content. " +
                    "Use <layout> as a container for multiple text elements instead.");
            }
        }

        element->children.push_back(parse_node(child));
    }
    return element;
}

} // namespace docraft::craft

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

#include "docraft/craft/docraft_loom_craft_language_parser.h"

#include <any>

#include <pugixml.hpp>

#include "docraft/craft/docraft_craft_language_parser.h"
#include "docraft/craft/docraft_craft_language_tokens.h"
#include "docraft/craft/parser/docraft_section_parsers.h"
#include "docraft/exception/docraft_exceptions.h"
#include "docraft/loom/craft/docraft_loom_tree_builder.h"

namespace docraft::craft {
    namespace {
        loom::DocraftLoomPdfCreator::Margins to_margins(const parser::ParsedSectionData& data)
        {
            loom::DocraftLoomPdfCreator::Margins margins;
            if (data.margin_top)
            {
                margins.top = *data.margin_top;
            }
            if (data.margin_bottom)
            {
                margins.bottom = *data.margin_bottom;
            }
            if (data.margin_left)
            {
                margins.left = *data.margin_left;
            }
            if (data.margin_right)
            {
                margins.right = *data.margin_right;
            }
            return margins;
        }
    } // namespace

    void DocraftLoomCraftLanguageParser::parse(const std::string& xml_string)
    {
        pugi::xml_document doc;
        const pugi::xml_parse_result result = doc.load_string(xml_string.c_str());
        if (!result)
        {
            throw docraft::exception::DataFormatException(
                "Error parsing .craft content: " + std::string(result.description()));
        }
        build_from_document(doc.first_child());
    }

    void DocraftLoomCraftLanguageParser::load_from_file(const std::filesystem::path& path)
    {
        pugi::xml_document doc;
        const pugi::xml_parse_result result = doc.load_file(path.c_str());
        if (!result)
        {
            throw docraft::exception::DataFormatException(
                "Error loading .craft file: " + std::string(result.description()));
        }
        build_from_document(doc.first_child());
    }

    std::shared_ptr<loom::DocraftLoomPdfCreator> DocraftLoomCraftLanguageParser::edit_creator()
    {
        return creator_;
    }

    void DocraftLoomCraftLanguageParser::build_from_document(const pugi::xml_node& document_node)
    {
        if (!document_node || std::string{document_node.name()} != std::string{section::kDocument})
        {
            throw docraft::exception::DataFormatException(
                "Invalid .craft content: missing required <" + std::string{section::kDocument} + "> root element");
        }

        DocraftCraftLanguageParser craft_parser;
        loom::craft::DocraftLoomTreeBuilder tree_builder;

        std::shared_ptr<DocraftParsedElement> header_element;
        std::shared_ptr<DocraftParsedElement> body_element;
        std::shared_ptr<DocraftParsedElement> footer_element;

        // Only Header/Body/Footer are recognized at this top level -- Settings/Metadata/
        // Foreach/NewPage/anything else are deliberately skipped rather than parsed (they
        // have no registered per-tag parser and are out of scope for this phase; see the
        // class-level doc comment).
        for (pugi::xml_node child : document_node.children())
        {
            if (child.type() != pugi::node_element)
            {
                continue;
            }
            const std::string tag = child.name();
            if (tag == std::string{section::kHeader})
            {
                header_element = craft_parser.parse_node(child);
            }
            else if (tag == std::string{section::kBody})
            {
                body_element = craft_parser.parse_node(child);
            }
            else if (tag == std::string{section::kFooter})
            {
                footer_element = craft_parser.parse_node(child);
            }
        }

        if (!body_element)
        {
            throw docraft::exception::DataFormatException(
                "Invalid .craft document: missing required <" + std::string{section::kBody} + "> element");
        }

        auto body_node = tree_builder.build(body_element);
        creator_ = std::make_shared<loom::DocraftLoomPdfCreator>(body_node);
        creator_->set_body_margins(
            to_margins(std::any_cast<const parser::ParsedSectionData&>(body_element->data)));

        if (header_element)
        {
            creator_->set_header(tree_builder.build(header_element));
            creator_->set_header_margins(
                to_margins(std::any_cast<const parser::ParsedSectionData&>(header_element->data)));
        }
        if (footer_element)
        {
            creator_->set_footer(tree_builder.build(footer_element));
            creator_->set_footer_margins(
                to_margins(std::any_cast<const parser::ParsedSectionData&>(footer_element->data)));
        }
    }
} // namespace docraft::craft

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

#include "docraft/craft/parser/docraft_parser.h"

#include "docraft/craft/parser/docraft_parser_helpers.h"
#include "docraft/exception/docraft_exceptions.h"

namespace docraft::craft::parser {
    std::any DocraftListParser::parse(const pugi::xml_node& craft_language_source)
    {
        ParsedListData data;
        data.kind = ParsedListKind::kOrdered;
        if (auto style_attr = craft_language_source.attribute(elements::list::attribute::kStyle.data())) {
            const std::string style_str = style_attr.as_string();
            if (style_str == std::string{elements::list::style::kNumber}) {
                data.ordered_style = ParsedOrderedListStyle::kNumber;
            } else if (style_str == std::string{elements::list::style::kRoman}) {
                data.ordered_style = ParsedOrderedListStyle::kRoman;
            } else {
                throw docraft::exception::InvalidInputException("Invalid list style: " + style_str);
            }
        }
        return data;
    }

    std::vector<std::string_view> DocraftListParser::accepted_attributes() const
    {
        return {
            elements::list::attribute::kStyle
        };
    }

    std::any DocraftUListParser::parse(const pugi::xml_node& craft_language_source)
    {
        ParsedListData data;
        data.kind = ParsedListKind::kUnordered;
        if (auto dot_attr = craft_language_source.attribute(elements::ulist::attribute::kDot.data())) {
            const std::string dot_str = dot_attr.as_string();
            if (dot_str == std::string{elements::ulist::dot::kDash}) {
                data.unordered_dot = ParsedUnorderedListDot::kDash;
            } else if (dot_str == std::string{elements::ulist::dot::kStar}) {
                data.unordered_dot = ParsedUnorderedListDot::kStar;
            } else if (dot_str == std::string{elements::ulist::dot::kCircle}) {
                data.unordered_dot = ParsedUnorderedListDot::kCircle;
            } else if (dot_str == std::string{elements::ulist::dot::kBox}) {
                data.unordered_dot = ParsedUnorderedListDot::kBox;
            } else {
                throw docraft::exception::InvalidInputException("Invalid unordered list dot: " + dot_str);
            }
        }
        return data;
    }

    std::vector<std::string_view> DocraftUListParser::accepted_attributes() const
    {
        return {
            elements::ulist::attribute::kDot
        };
    }
} // namespace docraft::craft::parser

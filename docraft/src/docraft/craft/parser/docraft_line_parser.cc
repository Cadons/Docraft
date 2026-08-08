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

#include "docraft/craft/parser/docraft_line_parser.h"

#include "docraft/craft/docraft_craft_language_tokens.h"
#include "docraft/craft/parser/docraft_parser_helpers.h"

namespace docraft::craft::parser {
    std::any DocraftLineParser::parse(const pugi::xml_node& craft_language_source)
    {
        ParsedLineData data;
        if (auto x1_attr = craft_language_source.attribute(elements::line::attribute::kX1.data()))
        {
            data.x1 = x1_attr.as_float();
        }
        if (auto y1_attr = craft_language_source.attribute(elements::line::attribute::kY1.data()))
        {
            data.y1 = y1_attr.as_float();
        }
        if (auto x2_attr = craft_language_source.attribute(elements::line::attribute::kX2.data()))
        {
            data.x2 = x2_attr.as_float();
        }
        if (auto y2_attr = craft_language_source.attribute(elements::line::attribute::kY2.data()))
        {
            data.y2 = y2_attr.as_float();
        }
        if (auto border_color_attr = craft_language_source.attribute(
                elements::line::attribute::kBorderColor.data()))
        {
            data.border_color = detail::get_color_attribute_raw(border_color_attr);
        }
        if (auto border_width_attr = craft_language_source.attribute(
                elements::line::attribute::kBorderWidth.data()))
        {
            data.border_width = border_width_attr.as_float();
        }
        return data;
    }

    std::vector<std::string_view> DocraftLineParser::accepted_attributes() const
    {
        return {
            elements::line::attribute::kX1,
            elements::line::attribute::kY1,
            elements::line::attribute::kX2,
            elements::line::attribute::kY2,
            elements::line::attribute::kBorderColor,
            elements::line::attribute::kBorderWidth
        };
    }
} // namespace docraft::craft::parser

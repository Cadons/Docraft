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

#include "docraft/craft/parser/docraft_polygon_parser.h"

#include "docraft/craft/docraft_craft_language_tokens.h"
#include "docraft/craft/parser/docraft_parser_helpers.h"
#include "docraft/craft/parser/docraft_shape_parser_utils.h"

namespace docraft::craft::parser {
    std::any DocraftPolygonParser::parse(const pugi::xml_node& craft_language_source)
    {
        ParsedPolygonData data;
        if (auto background_color_attr = craft_language_source.attribute(
                elements::polygon::attribute::kBackgroundColor.data()))
        {
            data.background_color = detail::get_color_attribute_raw(background_color_attr);
        }
        if (auto border_color_attr = craft_language_source.attribute(
                elements::polygon::attribute::kBorderColor.data()))
        {
            data.border_color = detail::get_color_attribute_raw(border_color_attr);
        }
        if (auto border_width_attr = craft_language_source.attribute(
                elements::polygon::attribute::kBorderWidth.data()))
        {
            data.border_width = border_width_attr.as_float();
        }
        if (auto border_style_attr = craft_language_source.attribute(
                elements::polygon::attribute::kBorderStyle.data()))
        {
            data.border_style = detail::parse_line_style(border_style_attr.as_string());
        }

        auto points = detail::parse_points_attribute(craft_language_source,
                                                     elements::polygon::attribute::kPoints.data());
        if (!points.empty())
        {
            data.points = std::move(points);
        }

        return data;
    }

    std::vector<std::string_view> DocraftPolygonParser::accepted_attributes() const
    {
        return {
            elements::polygon::attribute::kPoints,
            elements::polygon::attribute::kBackgroundColor,
            elements::polygon::attribute::kBorderColor,
            elements::polygon::attribute::kBorderWidth,
            elements::polygon::attribute::kBorderStyle
        };
    }
} // namespace docraft::craft::parser

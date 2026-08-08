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

#include "docraft/craft/parser/docraft_triangle_parser.h"

#include "docraft/craft/docraft_craft_language_tokens.h"
#include "docraft/craft/parser/docraft_parser_helpers.h"
#include "docraft/craft/parser/docraft_shape_parser_utils.h"
#include "docraft/exception/docraft_exceptions.h"

namespace docraft::craft::parser {
    std::any DocraftTriangleParser::parse(const pugi::xml_node& craft_language_source)
    {
        ParsedTriangleData data;
        if (auto background_color_attr = craft_language_source.attribute(
                elements::triangle::attribute::kBackgroundColor.data())) {
            data.background_color = detail::get_color_attribute_raw(background_color_attr);
        }
        if (auto border_color_attr = craft_language_source.attribute(
                elements::triangle::attribute::kBorderColor.data())) {
            data.border_color = detail::get_color_attribute_raw(border_color_attr);
        }
        if (auto border_width_attr = craft_language_source.attribute(
                elements::triangle::attribute::kBorderWidth.data())) {
            data.border_width = border_width_attr.as_float();
        }

        auto points = detail::parse_points_attribute(craft_language_source,
                                                     elements::triangle::attribute::kPoints.data());
        if (!points.empty()) {
            if (points.size() != 3U) {
                throw docraft::exception::InvalidInputException("Triangle requires exactly 3 points");
            }
            data.points = std::move(points);
        }

        return data;
    }

    std::vector<std::string_view> DocraftTriangleParser::accepted_attributes() const
    {
        return {
            elements::triangle::attribute::kPoints,
            elements::triangle::attribute::kBackgroundColor,
            elements::triangle::attribute::kBorderColor,
            elements::triangle::attribute::kBorderWidth
        };
    }
} // namespace docraft::craft::parser

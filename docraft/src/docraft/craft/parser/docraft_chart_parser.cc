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

namespace docraft::craft::parser {
    std::any DocraftChartParser::parse(const pugi::xml_node& craft_language_source)
    {
        ParsedChartData data;
        if (auto style_attr = craft_language_source.attribute(elements::chart::attribute::kStyle.data()))
        {
            data.style = style_attr.as_string();
        }
        if (auto axis_position_attr = craft_language_source.attribute(
                elements::chart::attribute::kAxisPosition.data()))
        {
            data.axis_position = axis_position_attr.as_string();
        }
        if (auto title_attr = craft_language_source.attribute(elements::chart::attribute::kTitle.data()))
        {
            data.title = title_attr.as_string();
        }
        if (auto x_label_attr = craft_language_source.attribute(elements::chart::attribute::kXLabel.data()))
        {
            data.x_label = x_label_attr.as_string();
        }
        if (auto y_label_attr = craft_language_source.attribute(elements::chart::attribute::kYLabel.data()))
        {
            data.y_label = y_label_attr.as_string();
        }
        if (auto background_color_attr = craft_language_source.attribute(
                elements::chart::attribute::kBackgroundColor.data()))
        {
            data.background_color = detail::get_color_attribute_raw(background_color_attr);
        }
        if (auto border_color_attr = craft_language_source.attribute(
                elements::chart::attribute::kBorderColor.data()))
        {
            data.border_color = detail::get_color_attribute_raw(border_color_attr);
        }
        if (auto border_width_attr = craft_language_source.attribute(
                elements::chart::attribute::kBorderWidth.data()))
        {
            data.border_width = border_width_attr.as_float();
        }
        return data;
    }
} // namespace docraft::craft::parser

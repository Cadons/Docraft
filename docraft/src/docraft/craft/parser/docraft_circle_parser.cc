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

#include "docraft/craft/parser/docraft_circle_parser.h"

#include "docraft/craft/docraft_craft_language_tokens.h"
#include "docraft/craft/parser/docraft_parser_helpers.h"

namespace docraft::craft::parser {
    std::any DocraftCircleParser::parse(const pugi::xml_node& craft_language_source)
    {
        ParsedCircleData data;
        if (auto background_color_attr = craft_language_source.attribute(
                elements::circle::attribute::kBackgroundColor.data())) {
            data.background_color = detail::get_color_attribute_raw(background_color_attr);
        }
        if (auto border_color_attr = craft_language_source.attribute(
                elements::circle::attribute::kBorderColor.data())) {
            data.border_color = detail::get_color_attribute_raw(border_color_attr);
        }
        if (auto border_width_attr = craft_language_source.attribute(
                elements::circle::attribute::kBorderWidth.data())) {
            data.border_width = border_width_attr.as_float();
        }
        if (auto radius_attr = craft_language_source.attribute(elements::circle::attribute::kRadius.data()))
        {
            data.radius = radius_attr.as_float();
        }
        if (auto attr = craft_language_source.attribute(elements::circle::attribute::kStartX.data()))
        {
            data.start_x = attr.as_float();
        }
        if (auto attr = craft_language_source.attribute(elements::circle::attribute::kStartY.data()))
        {
            data.start_y = attr.as_float();
        }
        if (auto attr = craft_language_source.attribute(elements::circle::attribute::kFinishX.data()))
        {
            data.finish_x = attr.as_float();
        }
        if (auto attr = craft_language_source.attribute(elements::circle::attribute::kFinishY.data()))
        {
            data.finish_y = attr.as_float();
        }
        return data;
    }
} // namespace docraft::craft::parser

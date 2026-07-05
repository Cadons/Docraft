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

#include "docraft/craft/parser/docraft_section_parsers.h"

#include "docraft/craft/docraft_craft_language_tokens.h"
#include "docraft/craft/parser/docraft_parser_helpers.h"

namespace docraft::craft::parser {
    std::any DocraftSectionParser::parse(const pugi::xml_node& craft_language_source)
    {
        ParsedSectionData data;
        // Reuses Rectangle's own attribute names -- a section's background/border are
        // expressed identically to a plain <Rectangle>'s.
        if (auto background_color_attr = craft_language_source.attribute(
            elements::rectangle::attribute::kBackgroundColor.data()))
        {
            data.background_color = detail::get_docraft_color(background_color_attr);
        }
        if (auto border_color_attr = craft_language_source.attribute(
            elements::rectangle::attribute::kBorderColor.data()))
        {
            data.border_color = detail::get_docraft_color(border_color_attr);
        }
        if (auto border_width_attr = craft_language_source.attribute(
            elements::rectangle::attribute::kBorderWidth.data()))
        {
            data.border_width = border_width_attr.as_float();
        }
        if (auto margin_top_attr = craft_language_source.attribute(section::attribute::kMarginTop.data()))
        {
            data.margin_top = margin_top_attr.as_float();
        }
        if (auto margin_bottom_attr = craft_language_source.attribute(section::attribute::kMarginBottom.data()))
        {
            data.margin_bottom = margin_bottom_attr.as_float();
        }
        if (auto margin_left_attr = craft_language_source.attribute(section::attribute::kMarginLeft.data()))
        {
            data.margin_left = margin_left_attr.as_float();
        }
        if (auto margin_right_attr = craft_language_source.attribute(section::attribute::kMarginRight.data()))
        {
            data.margin_right = margin_right_attr.as_float();
        }
        return data;
    }
} // namespace docraft::craft::parser

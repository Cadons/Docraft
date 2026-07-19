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
    namespace {
        ParsedTextStyle parse_style(const std::string& style_str)
        {
            if (style_str == std::string{style::kBold})
            {
                return ParsedTextStyle::kBold;
            }
            if (style_str == std::string{style::kItalic})
            {
                return ParsedTextStyle::kItalic;
            }
            if (style_str == std::string{style::kBoldItalic})
            {
                return ParsedTextStyle::kBoldItalic;
            }
            if (style_str == std::string{style::kNormal})
            {
                return ParsedTextStyle::kNormal;
            }
            throw docraft::exception::InvalidInputException("Invalid text style: " + style_str);
        }

        ParsedTextAlignment parse_alignment(const std::string& alignment_str)
        {
            if (alignment_str == std::string{alignment::kCenter})
            {
                return ParsedTextAlignment::kCenter;
            }
            if (alignment_str == std::string{alignment::kRight})
            {
                return ParsedTextAlignment::kRight;
            }
            if (alignment_str == std::string{alignment::kJustified})
            {
                return ParsedTextAlignment::kJustified;
            }
            if (alignment_str == std::string{alignment::kLeft})
            {
                return ParsedTextAlignment::kLeft;
            }
            throw docraft::exception::InvalidInputException("Invalid text alignment: " + alignment_str);
        }
    } // namespace

    std::any DocraftPageNumberParser::parse(const pugi::xml_node& craft_language_source)
    {
        ParsedPageNumberData data;

        if (auto font_size_attr = craft_language_source.attribute(elements::text::attribute::kFontSize.data())) {
            data.font_size = font_size_attr.as_float();
        }
        if (auto font_name_attr = craft_language_source.attribute(elements::text::attribute::kFontName.data())) {
            data.font_name = font_name_attr.as_string();
        }
        if (auto color_attr = craft_language_source.attribute(basic::attribute::kColor.data())) {
            data.color = detail::get_color_attribute_raw(color_attr);
        }
        if (auto style_attr = craft_language_source.attribute(elements::text::attribute::kStyle.data())) {
            data.style = parse_style(style_attr.as_string());
        }
        if (auto alignment_attr = craft_language_source.attribute(elements::text::attribute::kAlignment.data())) {
            data.alignment = parse_alignment(alignment_attr.as_string());
        }
        if (auto underline_attr = craft_language_source.attribute(elements::text::attribute::kUnderline.data())) {
            data.underline = underline_attr.as_bool();
        }
        if (auto strikeout_attr = craft_language_source.attribute(elements::text::attribute::kStrikeout.data())) {
            data.strikeout = strikeout_attr.as_bool();
        }

        return data;
    }
} // namespace docraft::craft::parser

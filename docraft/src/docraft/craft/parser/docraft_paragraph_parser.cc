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

#include "docraft/craft/parser/docraft_paragraph_parser.h"

#include "docraft/craft/parser/docraft_parser_helpers.h"
#include "docraft/exception/docraft_exceptions.h"

namespace docraft::craft::parser {
    namespace {
        // Duplicated from docraft_text_parser.cc's own local helper of the same name --
        // both are small, tag-local anonymous-namespace helpers, consistent with the
        // existing per-parser-file convention rather than a shared exported utility.
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

        // Duplicated from docraft_text_parser.cc's own local helper of the same name --
        // see the comment on parse_alignment above for why this file duplicates rather
        // than shares these small helpers.
        std::string trim_whitespace(const std::string& text)
        {
            constexpr auto whitespace = " \t\n\r\f\v";

            const auto first = text.find_first_not_of(whitespace);
            if (first == std::string::npos)
            {
                return {};
            }

            const auto last = text.find_last_not_of(whitespace);
            return text.substr(first, last - first + 1);
        }
    } // namespace

    std::any DocraftParagraphParser::parse(const pugi::xml_node& craft_language_source)
    {
        ParsedParagraphData data;
        data.text = trim_whitespace(craft_language_source.child_value());
        if (auto line_spacing_attr = craft_language_source.attribute(
            elements::paragraph::attribute::kLineSpacing.data()))
        {
            data.line_spacing = line_spacing_attr.as_float();
        }
        if (auto space_before_attr = craft_language_source.attribute(
            elements::paragraph::attribute::kSpaceBefore.data()))
        {
            data.space_before = space_before_attr.as_float();
        }
        if (auto space_after_attr = craft_language_source.attribute(
            elements::paragraph::attribute::kSpaceAfter.data()))
        {
            data.space_after = space_after_attr.as_float();
        }
        if (auto alignment_attr = craft_language_source.attribute(elements::text::attribute::kAlignment.data()))
        {
            data.alignment = parse_alignment(alignment_attr.as_string());
        }
        return data;
    }
} // namespace docraft::craft::parser

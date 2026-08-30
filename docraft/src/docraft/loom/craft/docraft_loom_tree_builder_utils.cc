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

#include "docraft/loom/craft/docraft_loom_tree_builder_utils.h"

#include <cctype>

namespace docraft::loom::craft {
    namespace parser = docraft::craft::parser;

    void apply_style(nodes::DocraftLoomText& node, parser::ParsedTextStyle style)
    {
        switch (style)
        {
        case parser::ParsedTextStyle::kBold:
            node.set_bold(true);
            node.set_italic(false);
            break;
        case parser::ParsedTextStyle::kItalic:
            node.set_bold(false);
            node.set_italic(true);
            break;
        case parser::ParsedTextStyle::kBoldItalic:
            node.set_bold(true);
            node.set_italic(true);
            break;
        case parser::ParsedTextStyle::kNormal:
        default:
            node.set_bold(false);
            node.set_italic(false);
            break;
        }
    }

    nodes::TextAlignment to_loom_alignment(parser::ParsedTextAlignment alignment)
    {
        switch (alignment)
        {
        case parser::ParsedTextAlignment::kLeft:
            return nodes::TextAlignment::kLeft;
        case parser::ParsedTextAlignment::kRight:
            return nodes::TextAlignment::kRight;
        case parser::ParsedTextAlignment::kJustified:
            return nodes::TextAlignment::kJustified;
        case parser::ParsedTextAlignment::kCenter:
        default:
            return nodes::TextAlignment::kCenter;
        }
    }

    namespace {
        bool is_word_char(char ch)
        {
            const auto uch = static_cast<unsigned char>(ch);
            return std::isalnum(uch) != 0 || ch == '_';
        }
    } // namespace

    std::string normalize_single_quoted_json(std::string_view input)
    {
        std::string output;
        output.reserve(input.size());
        bool in_single = false;
        bool in_double = false;
        for (std::size_t i = 0; i < input.size(); ++i)
        {
            const char ch = input[i];
            if (ch == '\\' && (i + 1) < input.size())
            {
                const char next = input[i + 1];
                if (in_single && (next == '\\' || next == '\''))
                {
                    // Escaped backslash/single-quote inside a single-quoted literal --
                    // drop the backslash, since the bare escaped char is fine once the
                    // delimiters below are converted to double quotes.
                    output.push_back(next);
                    ++i;
                    continue;
                }
                if (in_single || in_double)
                {
                    // Any other escape inside a string (`\"`/`\\` while in_double, or a
                    // JSON escape like `\n` in either) must be consumed as a unit so the
                    // escaped character -- notably an escaped quote -- can't be read on
                    // its own as a delimiter and toggle in_single/in_double.
                    output.push_back(ch);
                    output.push_back(next);
                    ++i;
                    continue;
                }
                output.push_back(ch);
                continue;
            }
            if (ch == '\'' && !in_double)
            {
                if (in_single)
                {
                    const char prev = (i > 0) ? input[i - 1] : '\0';
                    const char next = ((i + 1) < input.size()) ? input[i + 1] : '\0';
                    if (is_word_char(prev) && is_word_char(next))
                    {
                        output.push_back('\'');
                        continue;
                    }
                }
                in_single = !in_single;
                output.push_back('"');
                continue;
            }
            if (ch == '"' && !in_single)
            {
                in_double = !in_double;
                output.push_back(ch);
                continue;
            }
            output.push_back(ch);
        }
        return output;
    }
} // namespace docraft::loom::craft

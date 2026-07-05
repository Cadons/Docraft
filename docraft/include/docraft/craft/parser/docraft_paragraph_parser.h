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

#pragma once

#include <optional>

#include "docraft/docraft_lib.h"

#include "docraft/craft/i_docraft_parser.h"
#include "docraft/craft/parser/docraft_parser.h"

namespace docraft::craft::parser {
    /**
     * @brief Tag-specific payload parsed from a `<Paragraph>` element. `<Paragraph>`
     * groups child text/inline content (recursed into generically by
     * `DocraftCraftLanguageParser`, exactly like `<layout>`) -- this struct only carries
     * the tag's own typographic attributes.
     */
    struct ParsedParagraphData
    {
        std::optional<float> line_spacing;
        std::optional<float> space_before;
        std::optional<float> space_after;
        std::optional<ParsedTextAlignment> alignment;
    };

    /**
     * @brief Parser for paragraph nodes.
     */
    class DOCRAFT_LIB DocraftParagraphParser : public IDocraftParser
    {
    public:
        /**
         * @brief Parses a paragraph XML node.
         * @param craft_language_source XML node.
         * @return `ParsedParagraphData`.
         */
        std::any parse(const pugi::xml_node& craft_language_source) override;
    };
} // namespace docraft::craft::parser

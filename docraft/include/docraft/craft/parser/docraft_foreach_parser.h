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

#include <any>
#include <optional>
#include <string>

#include "docraft/docraft_lib.h"

#include "docraft/craft/i_docraft_parser.h"

namespace docraft::craft::parser {
    /**
     * @brief Tag-specific payload parsed from a `<Foreach>` element. Exactly one of
     * `model`/`count` is set -- `<Foreach>`'s own children (the repeat template) are
     * parsed once, generically, into the owning `DocraftParsedElement::children` by
     * `DocraftCraftLanguageParser`, same as any other container tag; expanding them
     * (once per `model` item, or `count` times verbatim) is
     * `docraft::loom::craft::DocraftLoomTreeBuilder`'s job, not this parser's.
     */
    struct ParsedForeachData
    {
        // Raw attribute value, unresolved and unnormalized: may be a JSON array literal
        // (possibly single-quoted), a `${variable}`/`${data(...)}` expression that
        // resolves to one, or a mix of both. DocraftLoomTreeBuilder::expand_foreach()
        // resolves template expressions first, then normalizes single quotes, then parses.
        std::optional<std::string> model;
        std::optional<int> count; // "n" attribute -- repeat count when no model is given
    };

    /**
     * @brief Parser for `<Foreach>` elements.
     */
    class DOCRAFT_LIB DocraftForeachParser : public IDocraftParser
    {
    public:
        /**
         * @brief Parses a `<Foreach>` XML node.
         * @param craft_language_source XML node.
         * @return `ParsedForeachData`.
         * @throws docraft::exception::InvalidInputException if both or neither of
         * `model`/`n` are present, or if `n` is negative.
         */
        std::any parse(const pugi::xml_node& craft_language_source) override;

        /**
         * @brief Attribute names this parser reads, beyond the common set.
         */
        std::vector<std::string_view> accepted_attributes() const override;
    };
} // namespace docraft::craft::parser

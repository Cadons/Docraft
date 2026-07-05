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

#include "docraft/docraft_color.h"
#include "docraft/docraft_lib.h"

#include "docraft/craft/i_docraft_parser.h"

namespace docraft::craft::parser {
    /**
     * @brief Tag-specific payload parsed from a `<Header>`/`<Body>`/`<Footer>` element.
     *
     * These are the top-level document sections: `DocraftLoomTreeBuilder` turns one into
     * a `DocraftLoomRectangle` stacking its children top-to-bottom (background/border are
     * applied to the rectangle's style; `padding` flows through the generic
     * `DocraftCommonAttributes::padding` instead of being duplicated here). Margins are
     * NOT a rectangle concept -- they are consumed directly by
     * `docraft::craft::DocraftLoomCraftLanguageParser` to build a
     * `docraft::loom::DocraftLoomPdfCreator::Margins` for `set_header_margins()` /
     * `set_body_margins()` / `set_footer_margins()`.
     */
    struct ParsedSectionData
    {
        std::optional<DocraftColor> background_color;
        std::optional<DocraftColor> border_color;
        std::optional<float> border_width;
        std::optional<float> margin_top;
        std::optional<float> margin_bottom;
        std::optional<float> margin_left;
        std::optional<float> margin_right;
    };

    /**
     * @brief Parser for `<Header>`, `<Body>` and `<Footer>` elements (all produce a
     * `ParsedSectionData` -- there is no tag-based default distinction between them,
     * unlike `Text`/`Title`/`Subtitle`).
     */
    class DOCRAFT_LIB DocraftSectionParser : public IDocraftParser
    {
    public:
        /**
         * @brief Parses a section-like XML node.
         * @param craft_language_source XML node.
         * @return `ParsedSectionData`.
         */
        std::any parse(const pugi::xml_node& craft_language_source) override;
    };
} // namespace docraft::craft::parser

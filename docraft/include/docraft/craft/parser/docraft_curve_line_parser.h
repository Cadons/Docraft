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
#include <string>
#include <string_view>
#include <vector>

#include "docraft/docraft_lib.h"

#include "docraft/craft/i_docraft_parser.h"
#include "docraft/craft/parser/docraft_parsed_line_style.h"
#include "docraft/docraft_position.h"

namespace docraft::craft::parser {
    /**
     * @brief Tag-specific payload parsed from a `<CurveLine>` element.
     *
     * No background color: an open curve has no interior to fill, so unlike
     * `<Polygon>` this element is stroke-only by construction rather than by
     * convention.
     */
    struct ParsedCurveLineData : ParsedStrokeStyleData
    {
        std::vector<docraft::Position> points;
    };

    /**
     * @brief Parser for curve line nodes.
     */
    class DOCRAFT_LIB DocraftCurveLineParser : public IDocraftParser {
    public:
        /**
         * @brief Parses a curve line XML node.
         * @param craft_language_source XML node.
         * @return `ParsedCurveLineData`.
         */
        std::any parse(const pugi::xml_node& craft_language_source) override;

        /**
         * @brief Attributes accepted on a `<CurveLine>` element.
         * @return List of accepted attribute names.
         */
        std::vector<std::string_view> accepted_attributes() const override;
    };
} // namespace docraft::craft::parser

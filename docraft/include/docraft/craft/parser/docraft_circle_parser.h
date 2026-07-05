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
     * @brief Tag-specific payload parsed from a `<Circle>` element. Circle has its own
     * `radius` attribute -- the generic `common.width`/`common.height` attributes are
     * deliberately not supported/used for sizing a circle (unlike the legacy shape, which
     * used a width/height bounding box); `docraft::loom::nodes::DocraftLoomCircle` only
     * ever has a single radius.
     */
    struct ParsedCircleData
    {
        std::optional<DocraftColor> background_color;
        std::optional<DocraftColor> border_color;
        std::optional<float> border_width;
        std::optional<float> radius;
    };

    /**
     * @brief Parser for circle nodes.
     */
    class DOCRAFT_LIB DocraftCircleParser : public IDocraftParser {
    public:
        /**
         * @brief Parses a circle XML node.
         * @param craft_language_source XML node.
         * @return `ParsedCircleData`.
         */
        std::any parse(const pugi::xml_node& craft_language_source) override;
    };
} // namespace docraft::craft::parser

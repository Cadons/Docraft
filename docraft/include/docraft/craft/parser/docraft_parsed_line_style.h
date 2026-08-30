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

namespace docraft::craft::parser {
    /**
     * @brief Stroke style for line-like elements (`<Line>`/`<CurveLine>`) and shape
     * borders (`<Rectangle>`/`<Circle>`/`<Triangle>`/`<Polygon>`), duplicated from
     * (rather than shared with) loom's own `docraft::loom::nodes::DocraftLineStyle` --
     * see `ParsedTextStyle` for the same "duplicate a tiny enum and translate"
     * rationale.
     */
    enum class ParsedLineStyle
    {
        kSolid,
        kDashed
    };

    /**
     * @brief Border styling shared by every stroke-only, line-like element
     * (`<Line>`/`<CurveLine>`). Base struct rather than duplicating the same three
     * fields on each `Parsed*Data`.
     */
    struct ParsedStrokeStyleData
    {
        std::optional<std::string> border_color;
        std::optional<float> border_width;
        std::optional<ParsedLineStyle> border_style;
    };

    /**
     * @brief Background+border styling shared by every fillable shape element
     * (`<Rectangle>`/`<Circle>`/`<Triangle>`/`<Polygon>`). Extends
     * `ParsedStrokeStyleData` with the fill color those elements additionally support.
     */
    struct ParsedShapeStyleData : ParsedStrokeStyleData
    {
        std::optional<std::string> background_color;
    };
} // namespace docraft::craft::parser

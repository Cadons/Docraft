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
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace docraft::craft {
    /**
     * @brief Block vs. absolute positioning for a parsed element.
     *
     * Deliberately duplicated from `docraft::loom::nodes::DocraftPositionType` rather than
     * included from it: `docraft::craft` must have zero dependency on the loom layout
     * engine (or any other future rendering target), so this tiny enum is translated by
     * `docraft::loom::craft::DocraftLoomTreeBuilder` instead of shared by include.
     */
    enum class PositionMode
    {
        kBlock,
        kAbsolute
    };

    /**
     * @brief Universal attributes any Craft-language tag can carry (name/x/y/width/
     * height/padding/margin/weight/z_index/visible/position). Parsed once, generically,
     * by `DocraftCraftLanguageParser` -- independent of what `DocraftParsedElement::data`
     * holds or which engine eventually consumes the parsed tree. `weight` is the odd one
     * out: it is only accepted on a direct child of `<Layout>` (validated at parse time),
     * since it has no meaning anywhere else.
     */
    struct DocraftCommonAttributes
    {
        std::optional<std::string> name;
        std::optional<float> x;
        std::optional<float> y;
        std::optional<float> width;
        std::optional<float> height;
        std::optional<float> padding;
        std::optional<float> margin;
        std::optional<float> margin_top;
        std::optional<float> margin_right;
        std::optional<float> margin_bottom;
        std::optional<float> margin_left;
        std::optional<float> weight;
        std::optional<int> z_index;
        std::optional<bool> visible;
        std::optional<PositionMode> position_mode;
    };

    /**
     * @brief A single parsed Craft-language element.
     *
     * `data` is a type-erased tag-specific payload -- one concrete `Parsed<Tag>Data`
     * struct per tag (e.g. `ParsedRectangleData`, `ParsedTextData`), produced by the
     * `IDocraftParser` registered for `tag_name`. This struct itself, and everything in
     * `docraft::craft`, has no notion of any rendering/layout engine; see
     * `docraft::loom::craft::DocraftLoomTreeBuilder` for the layer that turns a tree of
     * these into actual loom nodes.
     */
    struct DocraftParsedElement
    {
        std::string tag_name;
        DocraftCommonAttributes common;
        std::any data;
        std::vector<std::shared_ptr<DocraftParsedElement>> children;
    };
} // namespace docraft::craft

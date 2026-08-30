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

#include <string>
#include <string_view>

#include "docraft/craft/docraft_craft_parsed_element.h"
#include "docraft/craft/parser/docraft_parser.h"
#include "docraft/docraft_lib.h"
#include "docraft/loom/nodes/docraft_loom_line_style.h"
#include "docraft/loom/nodes/docraft_loom_node.h"
#include "docraft/loom/nodes/docraft_loom_text.h"

namespace docraft::loom::craft {
    /**
     * @brief Applies the style enum a Text/Title/Subtitle/PageNumber/table-title-cell's
     * parsed data may carry onto `node`'s bold/italic flags.
     */
    DOCRAFT_LIB void apply_style(nodes::DocraftLoomText& node, docraft::craft::parser::ParsedTextStyle style);

    DOCRAFT_LIB nodes::TextAlignment to_loom_alignment(docraft::craft::parser::ParsedTextAlignment alignment);

    /**
     * @brief Translates the parser's `ParsedLineStyle` into loom's own
     * `DocraftLineStyle`, for the same "duplicate a tiny enum and translate" reason as
     * to_loom_alignment().
     */
    DOCRAFT_LIB nodes::DocraftLineStyle to_loom_line_style(docraft::craft::parser::ParsedLineStyle style);

    /**
     * @brief Normalizes a JSON string that may use single quotes for strings instead of
     * double quotes (the `.craft` convention for embedding JSON in an XML attribute, e.g.
     * `model='[{"a":1}]'` vs `model="[{'a':1}]"`), while preserving apostrophes that are
     * part of a word (e.g. "don't") rather than string delimiters. Apply after
     * `${variable}`/`${data(...)}` resolution, since a resolved variable's value may itself
     * use this convention.
     */
    DOCRAFT_LIB std::string normalize_single_quoted_json(std::string_view input);

    /**
     * @brief Applies the common attributes (name/z_index/padding/margin/width/height/
     * position/x/y) every loom node shares, straight from a parsed element's
     * `DocraftCommonAttributes`.
     * @details `width`/`height` are only set if `NodeT` exposes `set_width`/`set_height`
     * (some node types, e.g. `DocraftLoomCircle`, size themselves through other
     * attributes instead).
     */
    template <typename NodeT>
    void apply_common_attributes(NodeT& node, const docraft::craft::DocraftCommonAttributes& common)
    {
        if (common.name.has_value())
        {
            node.set_name(*common.name);
        }
        if (common.z_index.has_value())
        {
            node.set_z_index(*common.z_index);
        }
        if (common.padding.has_value())
        {
            node.set_padding(*common.padding);
        }
        if (common.margin.has_value())
        {
            node.set_margin(*common.margin);
        }
        if (common.margin_top.has_value() || common.margin_right.has_value() || common.margin_bottom.has_value()
            || common.margin_left.has_value())
        {
            const auto& current = node.margin();
            node.set_margin(common.margin_top.value_or(current.top), common.margin_right.value_or(current.right),
                            common.margin_bottom.value_or(current.bottom),
                            common.margin_left.value_or(current.left));
        }

        if constexpr (requires(NodeT& n, float v) { n.set_width(v); })
        {
            if (common.width.has_value())
            {
                node.set_width(*common.width);
            }
        }
        if constexpr (requires(NodeT& n, float v) { n.set_height(v); })
        {
            if (common.height.has_value())
            {
                node.set_height(*common.height);
            }
        }

        if (common.position_mode.has_value())
        {
            node.set_position_mode(*common.position_mode == docraft::craft::PositionMode::kAbsolute
                                       ? nodes::DocraftPositionType::kAbsolute
                                       : nodes::DocraftPositionType::kBlock);
        }
        if (common.x.has_value() || common.y.has_value())
        {
            nodes::Position pos = node.explicit_position();
            if (common.x.has_value())
            {
                pos.x = *common.x;
            }
            if (common.y.has_value())
            {
                pos.y = *common.y;
            }
            node.set_explicit_position(pos);
        }
    }

    /**
     * @brief Applies the background_color/border_color/border_width fields shared by every
     * shape's parsed data (Rectangle/Circle/Triangle/Polygon/Canvas/Chart/Section) onto
     * node's own composed `DocraftLoomShapeStyle`.
     * @details Templated on the resolver rather than taking a `DocraftLoomTreeBuilderContext&`
     * directly so callers can pass e.g. `[&context](const std::string& c) { return
     * context.resolve_color(c); }`.
     */
    template <typename NodeT, typename ShapeDataT, typename ColorResolver>
    void apply_shape_style(NodeT& node, const ShapeDataT& data, const ColorResolver& resolve_color)
    {
        if (data.background_color.has_value())
        {
            node.edit_style().background_color = resolve_color(*data.background_color);
        }
        if (data.border_color.has_value())
        {
            node.edit_style().border_color = resolve_color(*data.border_color);
        }
        if (data.border_width.has_value())
        {
            node.edit_style().border_width = *data.border_width;
        }
        // Not every ShapeDataT carries a border_style (e.g. Canvas/Section/Chart's parsed
        // data doesn't) -- if constexpr keeps this a no-op for those instead of a hard
        // compile error, the same idiom apply_common_attributes() uses for width/height.
        if constexpr (requires { data.border_style; })
        {
            if (data.border_style.has_value())
            {
                node.edit_style().border_style = to_loom_line_style(*data.border_style);
            }
        }
    }
} // namespace docraft::loom::craft

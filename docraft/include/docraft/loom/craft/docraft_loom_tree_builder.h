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

#include <memory>
#include <vector>

#include "docraft/docraft_lib.h"

#include "docraft/craft/docraft_craft_parsed_element.h"
#include "docraft/loom/nodes/docraft_loom_blank_line.h"
#include "docraft/loom/nodes/docraft_loom_circle.h"
#include "docraft/loom/nodes/docraft_loom_image.h"
#include "docraft/loom/nodes/docraft_loom_line.h"
#include "docraft/loom/nodes/docraft_loom_list.h"
#include "docraft/loom/nodes/docraft_loom_node.h"
#include "docraft/loom/nodes/docraft_loom_page_number.h"
#include "docraft/loom/nodes/docraft_loom_polygon.h"
#include "docraft/loom/nodes/docraft_loom_rectangle.h"
#include "docraft/loom/nodes/docraft_loom_table.h"
#include "docraft/loom/nodes/docraft_loom_text.h"
#include "docraft/loom/nodes/docraft_loom_triangle.h"

namespace docraft::loom::craft {
    /**
     * @brief Translates a generic `docraft::craft::DocraftParsedElement` tree (produced by
     * `docraft::craft::DocraftCraftLanguageParser`, which has zero knowledge of loom) into
     * an actual tree of `docraft::loom::nodes::DocraftLoomNode`.
     *
     * This is the *only* component depending on both `docraft::craft` and
     * `docraft::loom` -- keeping the craft parser itself fully engine-agnostic (see the
     * plan's "Craft parser: generic + a separate loom builder layer").
     */
    class DOCRAFT_LIB DocraftLoomTreeBuilder
    {
    public:
        /**
         * @brief Builds a loom node (and, recursively, its subtree) from a parsed element.
         * @param element The parsed element to translate. May be null (returns nullptr).
         * @return The built node, or nullptr if `element` is null or
         * `element->common.visible` is explicitly `false` (the subtree is simply not
         * constructed, per the plan's "visible is resolved by the builder" design).
         * @throws docraft::exception::DataFormatException if `element->tag_name` is not a
         * tag this builder recognizes.
         */
        std::shared_ptr<nodes::DocraftLoomNode> build(
            const std::shared_ptr<docraft::craft::DocraftParsedElement>& element);

    private:
        using ParsedElement = docraft::craft::DocraftParsedElement;

        std::shared_ptr<nodes::DocraftLoomRectangle> build_rectangle(const ParsedElement& element);
        std::shared_ptr<nodes::DocraftLoomCircle> build_circle(const ParsedElement& element);
        std::shared_ptr<nodes::DocraftLoomTriangle> build_triangle(const ParsedElement& element);
        std::shared_ptr<nodes::DocraftLoomPolygon> build_polygon(const ParsedElement& element);
        std::shared_ptr<nodes::DocraftLoomLine> build_line(const ParsedElement& element);
        std::shared_ptr<nodes::DocraftLoomText> build_text(const ParsedElement& element);
        std::shared_ptr<nodes::DocraftLoomPageNumber> build_page_number(const ParsedElement& element);
        std::shared_ptr<nodes::DocraftLoomImage> build_image(const ParsedElement& element);
        std::shared_ptr<nodes::DocraftLoomBlankLine> build_blank_line(const ParsedElement& element);
        std::shared_ptr<nodes::DocraftLoomList> build_list(const ParsedElement& element);
        std::shared_ptr<nodes::DocraftLoomTable> build_table(const ParsedElement& element);
        std::shared_ptr<nodes::DocraftLoomNode> build_layout(const ParsedElement& element);

        /**
         * @brief Recursively builds each of `children` and appends the non-null results
         * to `parent`.
         */
        void add_children(const std::shared_ptr<nodes::DocraftLoomNode>& parent,
                          const std::vector<std::shared_ptr<ParsedElement>>& children);

        /**
         * @brief Applies the generic `DocraftCommonAttributes` (name/x/y/width/height/
         * padding/weight/z_index/position) to `node`. Position mode/explicit position/
         * name/z_index are always available (they live on `DocraftLoomNode` itself);
         * width/height/padding/weight are applied only if `NodeT` exposes the matching
         * setter (checked via a `requires` expression), so e.g. a `DocraftLoomCircle`
         * (no set_width) simply skips `common.width` rather than failing to compile or
         * throwing. `common.visible` is handled by the caller (build()), not here.
         */
        template <typename NodeT>
        static void apply_common_attributes(NodeT& node, const docraft::craft::DocraftCommonAttributes& common);
    };
} // namespace docraft::loom::craft

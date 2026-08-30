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
#include <optional>
#include <string>
#include <vector>

#include <nlohmann/json_fwd.hpp>

#include "docraft/docraft_color.h"
#include "docraft/docraft_lib.h"

#include "docraft/craft/docraft_craft_parsed_element.h"
#include "docraft/templating/docraft_template_engine.h"
#include "docraft/loom/craft/docraft_loom_tree_builder_context.h"
#include "docraft/loom/nodes/docraft_loom_blank_line.h"
#include "docraft/loom/nodes/docraft_loom_circle.h"
#include "docraft/loom/nodes/docraft_loom_image.h"
#include "docraft/loom/nodes/docraft_loom_line.h"
#include "docraft/loom/nodes/docraft_loom_list.h"
#include "docraft/loom/nodes/docraft_loom_new_page.h"
#include "docraft/loom/nodes/docraft_loom_node.h"
#include "docraft/loom/nodes/docraft_loom_page_number.h"
#include "docraft/loom/nodes/docraft_loom_paragraph.h"
#include "docraft/loom/nodes/docraft_loom_curve_line.h"
#include "docraft/loom/nodes/docraft_loom_polygon.h"
#include "docraft/loom/charts/docraft_chart_types.h"
#include "docraft/loom/nodes/docraft_loom_canvas.h"
#include "docraft/loom/nodes/docraft_loom_rectangle.h"
#include "docraft/loom/nodes/docraft_loom_table.h"
#include "docraft/loom/nodes/docraft_loom_text.h"
#include "docraft/loom/nodes/docraft_loom_subtitle.h"
#include "docraft/loom/nodes/docraft_loom_title.h"
#include "docraft/loom/nodes/docraft_loom_triangle.h"
#include "docraft/loom/nodes/docraft_loom_vstack.h"

namespace docraft::craft::parser {
    struct ParsedTextData;
    struct ParsedPageNumberData;
    struct ParsedImageData;
} // namespace docraft::craft::parser

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
    class DOCRAFT_LIB DocraftLoomTreeBuilder : public DocraftLoomTableHandlerContext
    {
    public:
        /**
         * @brief Constructs the builder.
         * @param template_engine Resolves `${variable}`/`${data("field")}` in Text/Title/
         * Subtitle content, Image src, and `<Foreach>`'s own `model` attribute. Defaults to
         * a fresh, empty engine (no variables registered) if not given, so `${...}`
         * expressions simply pass through unresolved rather than requiring a caller to
         * always supply one.
         */
        explicit DocraftLoomTreeBuilder(
            std::shared_ptr<docraft::templating::DocraftTemplateEngine> template_engine = nullptr);

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
            const std::shared_ptr<docraft::craft::DocraftParsedElement>& element) override;

        /**
         * @brief Sets the font family applied to Text/Title/Subtitle/PageNumber/table-cell
         * nodes built afterwards that don't carry their own `font_name` attribute (from
         * `<Settings><Fonts default="...">`). Unset by default, in which case such nodes
         * keep DocraftLoomText's own hardcoded fallback.
         */
        void set_default_font_family(const std::string& font_family);

    private:
        using ParsedElement = docraft::craft::DocraftParsedElement;

        /**
         * @brief Resolves a raw color attribute (hex, named color, or a `${...}` template
         * expression) against this builder's template engine and current Foreach item, then
         * interprets it. Color parsing itself happens too early (during the engine-agnostic
         * craft parse, before templating) to resolve `${...}` there, so the raw text is
         * carried in the parsed data until this point.
         * @param raw The raw color attribute text.
         * @throws docraft::exception::InvalidInputException if the resolved string is not a
         * valid hex code or a recognized named color.
         */
        DocraftColor resolve_color(const std::string& raw) const override;

        /**
         * @brief Renders `text` against this builder's template engine, using
         * current_foreach_item_ if one is in scope (per-Foreach-item `${data("field")}`
         * resolution) or plain `${variable}` substitution otherwise.
         */
        std::string render_template_text(const std::string& text) const override;

        void fill_text_node(nodes::DocraftLoomText& node,
                            const docraft::craft::parser::ParsedTextData& data) const override;
        void fill_page_number_node(nodes::DocraftLoomPageNumber& node,
                                   const docraft::craft::parser::ParsedPageNumberData& data) const override;
        void fill_image_node(nodes::DocraftLoomImage& node,
                             const docraft::craft::parser::ParsedImageData& data) const override;

        std::optional<std::string> default_font_family() const override { return default_font_family_; }

        docraft::templating::DocraftTemplateEngine& template_engine() const override { return *template_engine_; }
        const nlohmann::json* current_foreach_item() const override { return current_foreach_item_; }
        void set_current_foreach_item(const nlohmann::json* item) override { current_foreach_item_ = item; }

        /**
         * @brief Recursively builds each of `children` and appends the non-null results
         * to `parent`. A `<Foreach>`-tagged child is not itself a node: it is expanded in
         * place via expand_foreach() instead, so its own children (the repeat template)
         * end up as direct siblings of the surrounding content in `parent`.
         */
        void add_children(const std::shared_ptr<nodes::DocraftLoomNode>& parent,
                          const std::vector<std::shared_ptr<ParsedElement>>& children) override;

        /**
         * @brief Expands a `<Foreach>` element's children (its repeat template) into
         * `parent`, either once per item of its JSON array `model` (with `${data("field")}`
         * substituted into any Text-bearing descendant's text, via current_foreach_item_)
         * or, if no model was given, `n` times verbatim with no substitution.
         * @throws docraft::exception::DataFormatException if `model` isn't valid JSON or
         * doesn't resolve to a JSON array.
         */
        void expand_foreach(const std::shared_ptr<nodes::DocraftLoomNode>& parent,
                            const ParsedElement& foreach_element);

        /**
         * @brief The JSON item of the `<Foreach model="...">` iteration currently being
         * expanded, consulted by fill_text_node() to resolve `${data("field")}` in text
         * content; nullptr outside of a model-based Foreach expansion (including inside an
         * `n`-based one, unless nested within an outer model-based Foreach).
         */
        const nlohmann::json* current_foreach_item_ = nullptr;

        std::shared_ptr<docraft::templating::DocraftTemplateEngine> template_engine_;
        std::optional<std::string> default_font_family_;
    };
} // namespace docraft::loom::craft

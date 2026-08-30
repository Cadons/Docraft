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

namespace docraft::templating {
    class DocraftTemplateEngine;
} // namespace docraft::templating

namespace docraft::craft {
    struct DocraftParsedElement;
    namespace parser {
        struct ParsedTextData;
        struct ParsedPageNumberData;
        struct ParsedImageData;
    } // namespace parser
} // namespace docraft::craft

namespace docraft::loom::nodes {
    class DocraftLoomNode;
    class DocraftLoomText;
    class DocraftLoomPageNumber;
    class DocraftLoomImage;
} // namespace docraft::loom::nodes

namespace docraft::loom::craft {
    /**
     * @brief The narrow set of `DocraftLoomTreeBuilder` primitives a per-tag
     * `IDocraftLoomTagHandler` needs: color/template resolution, recursing back into the
     * builder for children, and the shared Text/PageNumber/Image field-filling logic.
     * @details Implemented by `DocraftLoomTreeBuilder` itself -- handlers receive a
     * reference to the builder through this interface instead of being `friend`ed into
     * all of its internals (namely `current_foreach_item_` and the Table-only helpers,
     * see `DocraftLoomTableHandlerContext`).
     */
    class DOCRAFT_LIB DocraftLoomTreeBuilderContext
    {
    public:
        virtual ~DocraftLoomTreeBuilderContext() = default;

        /**
         * @brief Resolves a raw color attribute (hex, named color, or a `${...}` template
         * expression) against the current template engine and Foreach item.
         */
        virtual DocraftColor resolve_color(const std::string& raw) const = 0;

        /**
         * @brief Renders `text` against the current template engine and Foreach item (if
         * any is in scope).
         */
        virtual std::string render_template_text(const std::string& text) const = 0;

        /**
         * @brief Recursively builds a loom node (and its subtree) from a parsed element,
         * dispatching back through the same tag-handler registry every top-level `build()`
         * call uses.
         */
        virtual std::shared_ptr<nodes::DocraftLoomNode> build(
            const std::shared_ptr<docraft::craft::DocraftParsedElement>& element) = 0;

        /**
         * @brief Recursively builds each of `children` and appends the non-null results to
         * `parent`, expanding any `<Foreach>` child in place.
         */
        virtual void add_children(
            const std::shared_ptr<nodes::DocraftLoomNode>& parent,
            const std::vector<std::shared_ptr<docraft::craft::DocraftParsedElement>>& children) = 0;

        /**
         * @brief The document's `<Settings><Fonts default="...">` family, if set.
         */
        virtual std::optional<std::string> default_font_family() const = 0;

        virtual void fill_text_node(nodes::DocraftLoomText& node,
                                     const docraft::craft::parser::ParsedTextData& data) const = 0;
        virtual void fill_page_number_node(nodes::DocraftLoomPageNumber& node,
                                            const docraft::craft::parser::ParsedPageNumberData& data) const = 0;
        virtual void fill_image_node(nodes::DocraftLoomImage& node,
                                      const docraft::craft::parser::ParsedImageData& data) const = 0;
    };

    /**
     * @brief Widens `DocraftLoomTreeBuilderContext` with the few extra primitives only
     * `<Table>`'s handler needs: direct template-engine access (a title cell resolves only
     * plain `${variable}` text, never `${data("field")}`, unlike everywhere else) and the
     * ability to scope the current Foreach item itself (a JSON-object `model` row template
     * binds each element to `current_foreach_item_` while building that element's rows).
     * @details `IDocraftLoomTagHandler::build()` takes this wider interface uniformly for
     * every tag rather than introducing a second handler-interface hierarchy --
     * `DocraftLoomTreeBuilder` is the only implementer of either interface, so no handler
     * pays a real cost for the 3 extra methods it doesn't call.
     */
    class DOCRAFT_LIB DocraftLoomTableHandlerContext : public DocraftLoomTreeBuilderContext
    {
    public:
        virtual docraft::templating::DocraftTemplateEngine& template_engine() const = 0;

        /**
         * @brief The JSON item of the innermost model-based iteration currently being
         * expanded (a `<Foreach model="...">` or a `<Table model="...">` row template's
         * own per-object binding); nullptr outside of one.
         */
        virtual const nlohmann::json* current_foreach_item() const = 0;

        /**
         * @brief Overrides the value `current_foreach_item()` returns; the caller is
         * responsible for restoring the previous value (see `resolve_table_model_json`'s
         * per-row-template use, an RAII scope guard local to the Table handler).
         */
        virtual void set_current_foreach_item(const nlohmann::json* item) = 0;
    };
} // namespace docraft::loom::craft

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

#include "docraft/docraft_lib.h"
#include "docraft/loom/craft/docraft_loom_tree_builder_context.h"

namespace docraft::craft {
    struct DocraftParsedElement;
} // namespace docraft::craft

namespace docraft::loom::nodes {
    class DocraftLoomNode;
} // namespace docraft::loom::nodes

namespace docraft::loom::craft {
    /**
     * @brief One handler per Craft-language tag: translates a parsed `<Tag>` element into
     * its loom node (and, recursively, its subtree).
     * @details Registered by tag name in `DocraftLoomTagHandlerRegistry`; adding a new tag
     * means adding one handler class plus one registration line, instead of a new branch
     * and method on `DocraftLoomTreeBuilder` itself. Mirrors the `docraft::craft::IDocraftParser`
     * registry that already exists one stage earlier (XML -> generic `DocraftParsedElement`).
     */
    class DOCRAFT_LIB IDocraftLoomTagHandler
    {
    public:
        virtual ~IDocraftLoomTagHandler() = default;

        /**
         * @brief Builds the loom node for `element`.
         * @param element The parsed element for this handler's tag (never null; the
         * "visible" and unrecognized-tag checks happen before dispatch, in
         * `DocraftLoomTreeBuilder::build()`).
         * @param context The builder-side primitives this handler needs -- see
         * `DocraftLoomTableHandlerContext`'s class doc for why every handler receives the
         * wider interface uniformly.
         */
        virtual std::shared_ptr<nodes::DocraftLoomNode> build(
            const docraft::craft::DocraftParsedElement& element,
            DocraftLoomTableHandlerContext& context) = 0;
    };
} // namespace docraft::loom::craft

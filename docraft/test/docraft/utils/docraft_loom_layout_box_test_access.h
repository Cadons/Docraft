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

#include "docraft/loom/nodes/docraft_loom_node.h"

namespace docraft::test::utils {
    /**
     * @brief The only way to fabricate a `docraft::loom::nodes::LayoutBox::LayoutProof`
     * or `PageIndexProof` outside of `DocraftLoomLayoutProcessor`/
     * `DocraftLoomPaginationProcessor` themselves.
     * @details Exists so unit tests can construct a `LayoutBox` state directly (a
     * frame/page_index without actually running the real pipeline) without the
     * production `LayoutBox` header exposing any public escape hatch of its own.
     * Gets there the same way the real processors do -- by privately inheriting both
     * `docraft::loom::nodes::DocraftLoomLayoutBoxLayoutAccessor` and
     * `DocraftLoomLayoutBoxPaginationAccessor` (kept separate there too, so a real
     * processor inheriting only one never gains the other's capability) -- rather than
     * being named as a friend inside `LayoutBox` itself; `LayoutBox` has no knowledge
     * this class (or any test code) exists. Include this header only from test (.cc)
     * files, never from anything under docraft/include or docraft/src.
     */
    class LayoutBoxTestAccess : private docraft::loom::nodes::DocraftLoomLayoutBoxLayoutAccessor,
                                 private docraft::loom::nodes::DocraftLoomLayoutBoxPaginationAccessor
    {
    public:
        using DocraftLoomLayoutBoxLayoutAccessor::make_layout_proof;

        static docraft::loom::nodes::LayoutBox::PageIndexProof make_page_index_proof()
        {
            return DocraftLoomLayoutBoxPaginationAccessor::make_page_index_proof(make_layout_proof());
        }
    };
} // namespace docraft::test::utils

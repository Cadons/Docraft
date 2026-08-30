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

#include "docraft/exception/docraft_exceptions.h"
#include "docraft/loom/nodes/docraft_loom_node.h"

namespace docraft::loom::nodes {
    /**
     * @brief Returns the LayoutProof `node`'s own LayoutBox was sealed with, for code
     * that needs to reuse it -- e.g. to seal a *different*, freshly-created node's
     * frame derived from `node` (a pagination-time table-split remainder/blank cell is
     * "the same already-laid-out region, reshaped", not a node Layout itself visited).
     * @throws docraft::exception::InvalidInputException if Layout hasn't sealed `node`
     * yet.
     */
    inline LayoutBox::LayoutProof layout_proof_or_throw(const DocraftLoomNode& node)
    {
        const auto proof = node.layout_box().layout_proof();
        if (!proof)
        {
            throw docraft::exception::InvalidInputException(
                "LayoutBox::frame read for a node DocraftLoomLayoutProcessor hasn't laid out yet");
        }
        return *proof;
    }

    /**
     * @brief Reads `node`'s already-sealed frame, for code that runs after
     * DocraftLoomLayoutProcessor (Pagination/Rendering/Table geometry helpers) and
     * therefore can't mint its own LayoutBox::LayoutProof.
     * @throws docraft::exception::InvalidInputException if Layout hasn't sealed a
     * frame for this node yet -- an internal pipeline-ordering bug, not a user input
     * error, but this codebase has no dedicated "internal invariant violated"
     * exception type.
     */
    inline const Rect& sealed_frame(const DocraftLoomNode& node)
    {
        return node.layout_box().frame(layout_proof_or_throw(node));
    }

    /**
     * @brief Mutable counterpart of sealed_frame(): reuses `node`'s own already-sealed
     * proof to read/write its frame -- for post-Layout code (Pagination reflow, Table
     * row/cell splitting) that legitimately adjusts an already-laid-out node's
     * geometry without re-running real Layout.
     * @throws docraft::exception::InvalidInputException if Layout hasn't sealed a
     * frame for this node yet.
     */
    inline Rect& sealed_edit_frame(DocraftLoomNode& node)
    {
        return node.edit_layout_box().edit_frame(layout_proof_or_throw(node));
    }
} // namespace docraft::loom::nodes

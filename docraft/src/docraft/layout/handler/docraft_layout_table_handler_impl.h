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

/**
 * @file docraft_layout_table_handler_impl.h
 * @brief Internal shared helpers and input structs for table layout handlers.
 *
 * NOT part of the public API. Include only from table handler .cc files.
 */

#pragma once

#include <algorithm>
#include <memory>
#include <numeric>
#include <vector>

#include "docraft/docraft_cursor.h"
#include "docraft/docraft_document_context.h"
#include "docraft/layout/docraft_layout_engine.h"
#include "docraft/model/docraft_table.h"
#include "docraft/model/docraft_text.h"

namespace docraft::layout::handler::table_impl {
    // ── Shared padding constants ───────────────────────────────────────────────

    constexpr float kHorizontalCellPaddingY = 2.5F;
    constexpr float kHorizontalCellPaddingX = 2.5F;
    constexpr float kVerticalCellPaddingY = 2.0F;
    constexpr float kVerticalCellPaddingX = 2.0F;

    // ── Input structs ─────────────────────────────────────────────────────────

    /**
     * @brief Input for computing the layout of a single table cell.
     */
    struct CellLayoutInput {
        const std::shared_ptr<model::DocraftNode> &node;
        docraft::layout::DocraftLayoutEngine &engine;
        const std::shared_ptr<DocraftDocumentContext> &context;
        DocraftCursor &table_cursor;
        float inner_width;
        float x;
        float y;
    };

    // ── Shared helper functions ───────────────────────────────────────────────

    /**
     * @brief Creates synthetic DocraftText title nodes for columns that have no node yet.
     */
    inline void ensure_title_nodes(const std::shared_ptr<model::DocraftTable> &node) {
        const auto &titles = node->titles();
        if (node->title_nodes().size() < titles.size()) {
            for (std::size_t i = node->title_nodes().size(); i < titles.size(); ++i) {
                auto title_node = std::make_shared<model::DocraftText>();
                title_node->set_text(titles[i]);
                node->add_title_node(title_node);
            }
        }
    }

    /**
     * @brief Flattens the 2-D content grid into a single sequential vector of nodes.
     */
    inline std::vector<std::shared_ptr<model::DocraftNode> > flatten_content_nodes(
        const std::shared_ptr<model::DocraftTable> &node) {
        std::vector<std::shared_ptr<model::DocraftNode> > flat;
        for (const auto &row: node->content_nodes()) {
            for (const auto &cell: row) {
                if (cell) {
                    flat.emplace_back(cell);
                }
            }
        }
        return flat;
    }

    /**
     * @brief Returns the usable width for the table, respecting auto_fill, explicit width, and fallback.
     */
    inline float available_width_for(const std::shared_ptr<model::DocraftTable> &node,
                                     const std::shared_ptr<DocraftDocumentContext> &context,
                                     float fallback) {
        if (node->auto_fill_width()) {
            return std::max(0.0F, context->layout().available_space());
        }
        const float w = node->width();
        if (w > 0.0F) return std::max(0.0F, w);
        return std::max(0.0F, fallback);
    }

    /**
     * @brief Moves the cursor to the node's declared position when in absolute mode;
     *        does nothing for block-flow nodes (cursor is already correct).
     */
    inline void configure_cursor_position(const std::shared_ptr<model::DocraftTable> &node,
                                          DocraftCursor &cursor) {
        if (node->position_mode() != model::DocraftPositionType::kBlock) {
            cursor.move_to(node->position().x, node->position().y);
        }
    }

    /**
     * @brief Vertically centers a text node within a row by adjusting its y-for-children offset.
     *
     * Has no effect on non-text nodes.
     */
    inline void center_text_in_row(const std::shared_ptr<model::DocraftNode> &node,
                                   const float row_top_y,
                                   const float row_height,
                                   const float baseline_offset) {
        const auto text_node = std::dynamic_pointer_cast<model::DocraftText>(node);
        if (!text_node) return;
        const float current_center =
                (text_node->anchors().top_left.y + text_node->anchors().bottom_left.y) * 0.5F;
        const float desired_center =
                row_top_y - (row_height * 0.5F) + (baseline_offset * text_node->font_size());
        const float delta = current_center - desired_center;
        if (delta != 0.0F) text_node->set_y_for_children(delta);
    }

    /**
     * @brief Runs the layout engine on a single cell, temporarily repositioning the cursor.
     *
     * Saves and restores the cursor position so surrounding cells are not affected.
     */
    inline void compute_cell_layout(const CellLayoutInput &input) {
        auto &layout_service = input.context->edit_layout();
        const float saved_x = input.table_cursor.x();
        const float saved_y = input.table_cursor.y();
        layout_service.set_current_rect_width(input.inner_width);
        input.table_cursor.move_to(input.x, input.y);
        (void) input.engine.compute_layout(input.node, input.table_cursor);
        input.table_cursor.move_to(saved_x, saved_y);
    }

    /**
     * @brief Builds the left-edge x-coordinates for each column given a starting x and per-column widths.
     */
    inline std::vector<float> build_column_lefts(const float fixed_x, const std::vector<float> &col_widths) {
        std::vector<float> col_lefts(col_widths.size(), fixed_x);
        float x = fixed_x;
        for (std::size_t i = 0; i < col_widths.size(); ++i) {
            col_lefts[i] = x;
            x += col_widths[i];
        }
        return col_lefts;
    }

    /**
     * @brief Returns a weight vector of size @p cols, normalised so that no zero-sum occurs.
     *
     * If the source vector has a wrong size or sums to zero, equal weights are used.
     */
    inline std::vector<float> assign_weights(const std::vector<float> &source, const std::size_t cols) {
        std::vector<float> weights = source;
        if (weights.size() != cols) weights.assign(cols, 1.0F);
        const float total = std::accumulate(weights.begin(), weights.end(), 0.0F);
        if (total <= 0.0F) weights.assign(cols, 1.0F);
        return weights;
    }
} // namespace docraft::layout::handler::table_impl


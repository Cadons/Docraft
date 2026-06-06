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

#include "docraft/layout/handler/docraft_layout_vertical_table_handler.h"

#include <algorithm>
#include <fmt/format.h>

#include "docraft_layout_table_handler_impl.h"
#include "docraft/layout/docraft_layout_engine.h"
#include "docraft/model/docraft_text.h"
#include "docraft/utils/docraft_logger.h"

namespace docraft::layout::handler {
    void DocraftLayoutVerticalTableHandler::compute(const std::shared_ptr<model::DocraftTable> &node,
                                                    model::DocraftTransform *box,
                                                    DocraftCursor &cursor) {
        auto &layout_service = edit_context()->edit_layout();
        DocraftCursor table_cursor = cursor;
        table_impl::configure_cursor_position(node, table_cursor);
        const float fixed_x = table_cursor.x();
        const float fixed_y = table_cursor.y();

        docraft::layout::DocraftLayoutEngine engine(edit_context(), false);
        const float saved_available_space = layout_service.available_space();
        const float kCellPaddingY = 2.0F;
        const float kCellPaddingX = 2.0F;
        const float baseline_offset = node->baseline_offset();

        auto center_text = [baseline_offset](const std::shared_ptr<model::DocraftText> &text_node,
                                             const float row_top_y,
                                             const float row_height) {
            if (!text_node) {
                return;
            }
            const float current_center =
                    (text_node->anchors().top_left.y + text_node->anchors().bottom_left.y) * 0.5F;
            const float desired_center = row_top_y - (row_height * 0.5F)
                                         + (baseline_offset * text_node->font_size());
            const float delta = current_center - desired_center;
            if (delta != 0.0F) {
                text_node->set_y_for_children(delta);
            }
        };

        const std::size_t rows = node->titles().empty() ? node->title_nodes().size() : node->titles().size();
        table_impl::ensure_title_nodes(node);

        const std::size_t value_cols = static_cast<std::size_t>(std::max(1, node->content_cols()));
        const auto flat = table_impl::flatten_content_nodes(node);

        float title_col_natural_width = 0.0F;

        for (std::size_t r = 0; r < rows; ++r) {
            const auto &title_node = node->title_nodes()[r];

            const float saved_x = table_cursor.x();
            const float saved_y = table_cursor.y();
            layout_service.set_current_rect_width(layout_service.available_space());
            table_cursor.move_to(fixed_x, layout_service.page_height());
            (void) engine.compute_layout(title_node, table_cursor);
            table_cursor.move_to(saved_x, saved_y);

            title_col_natural_width = std::max(title_col_natural_width, title_node->width());
        }

        const float available_width = table_impl::available_width_for(node, edit_context(), title_col_natural_width);

        std::vector<float> weights = node->column_weights();
        if (weights.size() < 2) {
            weights.assign(2, 1.0F);
        }
        float total_weight = weights[0] + weights[1];
        if (total_weight <= 0.0F) {
            weights = {.5F, .5F};
            total_weight = 1.0F;
        }

        const float title_col_width = std::max(title_col_natural_width,
                                               available_width * (weights[0] / total_weight));
        const float values_block_width = std::max(0.0F, available_width - title_col_width);
        float value_col_width = (value_cols > 0)
                                    ? (values_block_width / static_cast<float>(value_cols))
                                    : values_block_width;

        float explicit_value_col_width = 0.0F;
        for (const auto &cell: flat) {
            if (cell && cell->width() > 0.0F) {
                explicit_value_col_width = std::max(explicit_value_col_width, cell->width());
            }
        }
        if (explicit_value_col_width > 0.0F) {
            value_col_width = explicit_value_col_width;
        }

        float y = fixed_y;
        float total_height = 0.0F;

        const float padding_x = 2.0F * kCellPaddingX;
        const float padding_y = 2.0F * kCellPaddingY;

        float header_row_height = 0.0F;
        if (!node->htitle_nodes().empty()) {
            for (std::size_t c = 0; c < std::min(value_cols, node->htitle_nodes().size()); ++c) {
                const auto &htitle_node = node->htitle_nodes()[c];
                const float saved_x = table_cursor.x();
                const float saved_y = table_cursor.y();
                const float inner_width = std::max(0.0F, value_col_width - padding_x);
                layout_service.set_current_rect_width(inner_width);
                table_cursor.move_to(
                    fixed_x + title_col_width + (static_cast<float>(c) * value_col_width) + kCellPaddingX,
                    y - kCellPaddingY);
                (void) engine.compute_layout(htitle_node, table_cursor);
                table_cursor.move_to(saved_x, saved_y);
                header_row_height = std::max(header_row_height, htitle_node->height() + padding_y);
            }

            for (std::size_t c = 0; c < std::min(value_cols, node->htitle_nodes().size()); ++c) {
                const auto &htitle_node = node->htitle_nodes()[c];
                if (auto text_node = std::dynamic_pointer_cast<model::DocraftText>(htitle_node)) {
                    center_text(text_node, y, header_row_height);
                }
                htitle_node->set_position({
                    .x = fixed_x + title_col_width + (static_cast<float>(c) * value_col_width), .y = y
                });
                htitle_node->set_width(value_col_width);
                htitle_node->set_height(header_row_height);
            }

            total_height += header_row_height;
            y -= header_row_height;
        }

        for (std::size_t r = 0; r < rows; ++r) {
            float row_height = 0.0F;

            {
                const auto &title_node = node->title_nodes()[r];
                const float saved_x = table_cursor.x();
                const float saved_y = table_cursor.y();
                const float inner_width = std::max(0.0F, title_col_width - padding_x);
                layout_service.set_current_rect_width(inner_width);
                table_cursor.move_to(fixed_x + kCellPaddingX, y - kCellPaddingY);
                (void) engine.compute_layout(title_node, table_cursor);
                table_cursor.move_to(saved_x, saved_y);
                row_height = std::max(row_height, title_node->height() + padding_y);
            }

            const std::size_t row_first = r * value_cols;
            for (std::size_t vc = 0; vc < value_cols; ++vc) {
                const std::size_t idx = row_first + vc;
                if (idx >= flat.size()) {
                    break;
                }
                const auto &cell = flat[idx];

                const float saved_x = table_cursor.x();
                const float saved_y = table_cursor.y();
                const float inner_width = std::max(0.0F, value_col_width - padding_x);
                layout_service.set_current_rect_width(inner_width);
                table_cursor.move_to(
                    fixed_x + title_col_width + (static_cast<float>(vc) * value_col_width) + kCellPaddingX,
                    y - kCellPaddingY);
                (void) engine.compute_layout(cell, table_cursor);
                table_cursor.move_to(saved_x, saved_y);

                row_height = std::max(row_height, cell->height() + padding_y);
            }

            {
                const auto &title_node = node->title_nodes()[r];
                if (auto text_node = std::dynamic_pointer_cast<model::DocraftText>(title_node)) {
                    center_text(text_node, y, row_height);
                }
                title_node->set_position({.x = fixed_x, .y = y});
                title_node->set_width(title_col_width);
                title_node->set_height(row_height);
            }

            for (std::size_t vc = 0; vc < value_cols; ++vc) {
                const std::size_t idx = row_first + vc;
                if (idx >= flat.size()) {
                    break;
                }
                const auto &cell = flat[idx];
                if (auto text_cell = std::dynamic_pointer_cast<model::DocraftText>(cell)) {
                    center_text(text_cell, y, row_height);
                }
                cell->set_position({
                    .x = fixed_x + title_col_width + (static_cast<float>(vc) * value_col_width), .y = y
                });
                cell->set_width(value_col_width);
                cell->set_height(row_height);
            }

            total_height += row_height;
            y -= row_height;
        }

        node->set_position({.x = fixed_x, .y = fixed_y});
        node->set_width(title_col_width + (value_col_width * static_cast<float>(value_cols)));
        node->set_height(total_height);

        if (box) {
            box->set_position(node->position());
            box->set_width(node->width());
            box->set_height(node->height());
        }
        layout_service.set_current_rect_width(saved_available_space);

        for (const auto &row: node->content_nodes()) {
            for (const auto &cell: row) {
                if (cell) {
                    LOG_DEBUG(fmt::format("Cell at ({}, {}) with size ({}, {})", cell->position().x,
                        cell->position().y, cell->width(), cell->height()));
                }
            }
        }
    }
} // namespace docraft::layout::handler

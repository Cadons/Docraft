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

#include "docraft/layout/handler/docraft_layout_horizontal_table_handler.h"

#include <algorithm>
#include <fmt/format.h>
#include <numeric>
#include <vector>

#include "docraft_layout_table_handler_impl.h"
#include "docraft/layout/docraft_layout_engine.h"
#include "docraft/model/docraft_text.h"
#include "docraft/utils/docraft_logger.h"

namespace docraft::layout::handler {
    void DocraftLayoutHorizontalTableHandler::compute(const std::shared_ptr<model::DocraftTable> &node,
                                                      model::DocraftTransform *box,
                                                      DocraftCursor &cursor) {
        auto &layout_service = edit_context()->edit_layout();
        DocraftCursor table_cursor = cursor;
        table_impl::configure_cursor_position(node, table_cursor);
        const float fixed_x = table_cursor.x();
        const float fixed_y = table_cursor.y() - node->padding();

        DocraftLayoutEngine engine(edit_context(), false);
        const float saved_available_space = layout_service.available_space();
        constexpr float kCellPaddingY = 2.5F;
        constexpr float kCellPaddingX = 2.5F;
        const float baseline_offset = node->baseline_offset();

        auto center_text_vertically = [baseline_offset](const std::shared_ptr<model::DocraftText> &text_node,
                                                        const float row_top_y,
                                                        const float row_height) {
            if (!text_node) {
                return;
            }
            const float current_center = (text_node->anchors().top_left.y + text_node->anchors().bottom_left.y) * 0.5F;
            const float desired_center = row_top_y - (row_height * 0.5F) + (baseline_offset * text_node->font_size());
            const float delta = current_center - desired_center;
            if (delta != 0.0F) {
                text_node->set_y_for_children(delta);
            }
        };

        const auto &titles = node->titles();
        const std::size_t cols = titles.size();
        table_impl::ensure_title_nodes(node);

        std::vector<float> natural_widths(cols, 0.0F);
        float title_row_height = 0.0F;

        for (std::size_t i = 0; i < cols; ++i) {
            const auto &title_node = node->title_nodes()[i];
            const float saved_x = table_cursor.x();
            const float saved_y = table_cursor.y();
            table_cursor.move_to(0.0F, layout_service.page_height());
            (void) engine.compute_layout(title_node, table_cursor);
            //Measure title nodes with an unconstrained width to get their natural widths, then restore cursor.
            table_cursor.move_to(saved_x, saved_y);

            natural_widths[i] = title_node->width();
            title_row_height = std::max(title_row_height, title_node->height());
        }

        const float natural_sum = std::accumulate(natural_widths.begin(), natural_widths.end(), 0.0F);
        const float available_width = table_impl::available_width_for(node, edit_context(), natural_sum);

        std::vector<float> explicit_col_widths(cols, 0.0F);
        //Compute explicit column widths and check if any are set, which affects scaling behavior.
        const auto grid = node->content_nodes();
        for (const auto &row: grid) {
            for (std::size_t c = 0; c < std::min(row.size(), cols); ++c) {
                const auto &cell = row[c];
                if (!cell) {
                    continue;
                }
                if (cell->width() > 0.0F) {
                    explicit_col_widths[c] = std::max(explicit_col_widths[c], cell->width());
                }
            }
        }
        // If any explicit column widths are set, they take priority and scaling only applies to non-explicit columns.
        // If no explicit widths are set, all columns scale proportionally.
        const bool has_explicit_col_width =
                std::ranges::any_of(explicit_col_widths, [](const float w) { return w > 0.0F; });

        std::vector<float> weights = node->column_weights();
        if (weights.size() != cols) {
            weights.assign(cols, 1.0F);
        }
        float total_weight = std::accumulate(weights.begin(), weights.end(), 0.0F);
        if (total_weight <= 0.0F) {
            weights.assign(cols, 1.0F);
            total_weight = static_cast<float>(cols);
        }

        std::vector<float> col_widths(cols, 0.0F);
        float widths_sum = 0.0F;
        // First assign widths based on natural size or weighted target, then scale if needed to fit available width.
        for (std::size_t i = 0; i < cols; ++i) {
            if (explicit_col_widths[i] > 0.0F) {
                //Use the explicit
                col_widths[i] = explicit_col_widths[i];
            } else {
                //Compute proportionally based on weight, but never less than natural width.
                const float target = available_width * (weights[i] / total_weight);
                col_widths[i] = std::max(natural_widths[i], target);
            }
            widths_sum += col_widths[i];
        }

        //If the total width exceeds available width, we need to scale down some columns. The scaling behavior depends on whether any explicit widths are set:
        if (!has_explicit_col_width && available_width > 0.0F && widths_sum > 0.0F && widths_sum != available_width) {
            const float scale = available_width / widths_sum;
            for (auto &w: col_widths) {
                w *= scale;
            }
            widths_sum = available_width;
        }

        // Build the left-edge x-coordinates for each column based on the final widths.
        std::vector<float> col_lefts(cols, fixed_x);
        {
            float x = fixed_x;
            for (std::size_t i = 0; i < cols; ++i) {
                col_lefts[i] = x;
                x += col_widths[i];
            }
        }

        // Layout title nodes and compute title row height.
        // Title nodes are always sized to fill the column width,
        // but their height is determined by their content (plus padding) up to the maximum title row height.
        std::vector<float> title_heights(cols, 0.0F);
        title_row_height = 0.0F;
        const float kOffsetY = 2.0F * kCellPaddingY;
        const float kOffsetX = 2.0F * kCellPaddingX;
        for (std::size_t i = 0; i < cols; ++i) {
            const auto &title_node = node->title_nodes()[i];

            const float saved_x = table_cursor.x();
            const float saved_y = table_cursor.y();
            const float inner_width = std::max(0.0F, col_widths[i] - kOffsetX);
            layout_service.set_current_rect_width(inner_width);
            table_cursor.move_to(col_lefts[i] + kCellPaddingX, fixed_y - kCellPaddingY);
            (void) engine.compute_layout(title_node, table_cursor);
            table_cursor.move_to(saved_x, saved_y);

            title_heights[i] = title_node->height();
            title_row_height = std::max(title_row_height, title_heights[i]);

            if (auto text_node = std::dynamic_pointer_cast<model::DocraftText>(title_node)) {
                center_text_vertically(text_node, fixed_y, title_row_height + 4 * kCellPaddingY);
            }
            title_node->set_position({.x = col_lefts[i], .y = fixed_y});
            title_node->set_width(col_widths[i]);
            title_node->set_height(title_row_height);
        }

        // Layout content cells row by row, starting below the title row. Each cell's height is determined by its content (plus padding) up to the maximum height of its row.
        float y = fixed_y - title_row_height;
        float total_content_height = 0.0F;
        float max_content_height = 0.0F;
        for (const auto &row: grid) {
            float row_height = 20.0F + kOffsetX;

            float max_column_height = 0.0F;
            for (std::size_t c = 0; c < std::min(row.size(), cols); ++c) {
                const auto &cell = row[c];
                if (!cell) {
                    continue;
                }

                const float saved_x = table_cursor.x();
                const float saved_y = table_cursor.y();
                const float inner_width = std::max(0.0F, col_widths[c] - kOffsetX);
                layout_service.set_current_rect_width(inner_width);
                table_cursor.move_to(col_lefts[c] + kCellPaddingX, y - kCellPaddingY);
                (void) engine.compute_layout(cell, table_cursor);
                table_cursor.move_to(saved_x, saved_y);

                row_height = std::max(cell->height(), row_height);
                max_column_height = std::max(max_column_height, cell->height());
                if (auto text_cell = std::dynamic_pointer_cast<model::DocraftText>(cell)) {
                    center_text_vertically(text_cell, y, row_height + 4 *kCellPaddingY);
                }
                cell->set_position({.x = col_lefts[c], .y = y});
                cell->set_width(col_widths[c]);
                cell->set_height(row_height);
            }
            // After computing the height of the tallest cell in the row, ensure all cells in the row have the same height to maintain a consistent grid appearance.
            for (std::size_t c = 0; c < std::min(row.size(), cols); ++c) {
                const auto &cell = row[c];
                if (cell->height() < max_column_height) {
                    cell->set_height(max_column_height);
                }
            }
            max_content_height = std::max(max_content_height, max_column_height);
            total_content_height += row_height;
            y -= row_height;
        }


        node->set_position({.x = fixed_x, .y = fixed_y});
        node->set_width(widths_sum);
        node->set_height(title_row_height + total_content_height);

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

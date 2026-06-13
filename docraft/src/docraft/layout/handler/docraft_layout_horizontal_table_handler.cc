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
#include <numeric>
#include <vector>

#include "docraft/layout/docraft_layout_engine.h"

namespace docraft::layout::handler {
    void DocraftLayoutHorizontalTableHandler::setup_compute_state(const std::shared_ptr<model::DocraftTable> &node,
                                                                  DocraftCursor &cursor) {
        initialize_base_state(node, cursor, kHorizontalCellPaddingX, kHorizontalCellPaddingY);
    }

    DocraftLayoutHorizontalTableHandler::TableContent DocraftLayoutHorizontalTableHandler::collect_table_content(
        const std::shared_ptr<model::DocraftTable> &node) {
        ensure_title_nodes(node);
        return TableContent{
            .title_nodes = node->title_nodes(),
            .rows = node->content_nodes()
        };
    }

    DocraftLayoutHorizontalTableHandler::WidthPlan DocraftLayoutHorizontalTableHandler::compute_width_plan(
        const std::shared_ptr<model::DocraftTable> &node,
        const TableContent &content) {
        const std::size_t cols = content.title_nodes.size();
        std::vector<float> natural_widths(cols, 0.0F);

        auto &layout_service = edit_context()->edit_layout();
        auto &cursor = get_table_cursor();
        auto &engine = get_engine();
        for (std::size_t i = 0; i < cols; ++i) {
            const auto &title_node = content.title_nodes[i];
            const float saved_x = cursor.x();
            const float saved_y = cursor.y();
            cursor.move_to(0.0F, layout_service.page_height());
            (void) engine.compute_layout(title_node, cursor);
            cursor.move_to(saved_x, saved_y);

            natural_widths[i] = title_node->width();
        }

        const float natural_sum = std::accumulate(natural_widths.begin(), natural_widths.end(), 0.0F);
        const float available_width = available_width_for(node, natural_sum);

        std::vector<float> explicit_col_widths(cols, 0.0F);
        for (const auto &row: content.rows) {
            for (std::size_t c = 0; c < std::min(row.size(), cols); ++c) {
                const auto &cell = row[c];
                if (cell && cell->width() > 0.0F) {
                    explicit_col_widths[c] = std::max(explicit_col_widths[c], cell->width());
                }
            }
        }

        const bool has_explicit_col_width = std::ranges::any_of(explicit_col_widths, [](const float w) {
            return w > 0.0F;
        });

        const std::vector<float> weights = assign_weights(node->column_weights(), cols);
        const float total_weight = std::accumulate(weights.begin(), weights.end(), 0.0F);

        std::vector<float> col_widths(cols, 0.0F);
        float widths_sum = 0.0F;
        for (std::size_t i = 0; i < cols; ++i) {
            if (explicit_col_widths[i] > 0.0F) {
                col_widths[i] = explicit_col_widths[i];
            } else {
                const float target = available_width * (weights[i] / total_weight);
                col_widths[i] = std::max(natural_widths[i], target);
            }
            widths_sum += col_widths[i];
        }

        if (!has_explicit_col_width && available_width > 0.0F && widths_sum > 0.0F && widths_sum != available_width) {
            const float scale = available_width / widths_sum;
            for (auto &w: col_widths) {
                w *= scale;
            }
            widths_sum = available_width;
        }

        return WidthPlan(widths_sum, col_widths);
    }

    void DocraftLayoutHorizontalTableHandler::apply_width_plan(const WidthPlan &plan) {
        col_widths_.assign(plan.col_widths.begin(), plan.col_widths.end());
        const auto lefts = build_column_lefts(get_fixed_x(), col_widths_);
        col_lefts_.assign(lefts.begin(), lefts.end());
        cols_ = col_widths_.size();
    }

    float DocraftLayoutHorizontalTableHandler::layout_body_rows(const TableContent &content,
                                                                const float start_y,
                                                                const float min_row_height) {
        float y = start_y;
        float total_content_height = 0.0F;
        for (const auto &row: content.rows) {
            // Compute the maximum height of the row based on its cells
            const float row_height = layout_row_band(build_row_band(row), y, min_row_height);
            total_content_height += row_height; // Update the y position for the next row
            y -= row_height; // Update the y position for the next row
        }
        return total_content_height;
    }

    DocraftLayoutTableHandler::RowBand DocraftLayoutHorizontalTableHandler::build_row_band(
        const std::vector<std::shared_ptr<model::DocraftNode> > &row_nodes) const {
        RowBand band;
        const std::size_t cells = std::min(row_nodes.size(), get_cols());
        band.nodes.reserve(cells);
        band.lefts.reserve(cells);
        band.widths.reserve(cells);

        for (std::size_t c = 0; c < cells; ++c) {
            band.nodes.emplace_back(row_nodes[c]);
            band.lefts.emplace_back(get_col_lefts()[c]);
            band.widths.emplace_back(get_col_widths()[c]);
        }
        return band;
    }

    void DocraftLayoutHorizontalTableHandler::compute(const std::shared_ptr<model::DocraftTable> &node,
                                                      model::DocraftTransform *box,
                                                      DocraftCursor &cursor) {
        setup_compute_state(node, cursor); // Collect table content
        const TableContent content = collect_table_content(node); // Compute width plan
        const WidthPlan width_plan = compute_width_plan(node, content); // Apply width plan
        apply_width_plan(width_plan); // Layout title nodes

        const float min_row_height = std::max(0.0F, 2.0F * node->padding());
        // Minimum row height to ensure padding is respected
        const float title_row_height = layout_row_band(build_row_band(content.title_nodes), get_fixed_y(),
                                                       min_row_height);
        const float body_start_y = get_fixed_y() - title_row_height; // Layout body rows
        const float body_height = layout_body_rows(content, body_start_y, min_row_height);
        // Total content height including title and body

        // Apply final size and position to the table box
        finalize_output(node, box, width_plan.table_width, title_row_height + body_height);
        log_cells(node);
        clear_compute_state();
    }


    std::size_t DocraftLayoutHorizontalTableHandler::get_cols() const {
        return cols_;
    }

    const std::vector<float> &DocraftLayoutHorizontalTableHandler::get_col_widths() const {
        return col_widths_;
    }

    const std::vector<float> &DocraftLayoutHorizontalTableHandler::get_col_lefts() const {
        return col_lefts_;
    }

} // namespace docraft::layout::handler

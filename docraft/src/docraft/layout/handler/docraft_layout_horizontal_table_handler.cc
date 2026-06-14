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
    void DocraftLayoutHorizontalTableHandler::setup_pipeline_state(const std::shared_ptr<model::DocraftTable> &node,
                                                                   DocraftCursor &cursor) {
        initialize_base_state(node, cursor, DocraftLayoutTableHandler::kHorizontalCellPaddingX,
                              DocraftLayoutTableHandler::kHorizontalCellPaddingY);
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
        // Compute width of the cells using titles nodes
        for (std::size_t i = 0; i < cols; ++i) {
            const auto &title_node = content.title_nodes[i]; /// get title
            //save cursor position before computing title layout
            const float saved_x = cursor.x();
            const float saved_y = cursor.y();
            // Move cursor to the bottom of the page to avoid affecting current layout state,
            // since we're only interested in measuring the title node's width, not its position
            cursor.move_to(0.0F, layout_service.page_height());
            //compute title node layout
            (void) engine.compute_layout(title_node, cursor);
            cursor.move_to(saved_x, saved_y); //reset cursor to saved position before computing title layout
            //save natural width of the title node
            natural_widths[i] = title_node->width();
        }

        //sum the natural widths of the title nodes to get the total natural width of the table,
        //which serves as a baseline for column width allocation
        const float natural_sum = std::accumulate(natural_widths.begin(), natural_widths.end(), 0.0F);
        /*
         * an available width for the table is the maximum width that can be allocated to the table based on the current
         * layout context, minus any horizontal padding specified by the table node.
         * This is the width that will be distributed among
         * the columns after accounting for their natural widths and any explicit widths specified on the cells.
         */
        const float available_width = available_width_for(node, natural_sum);

        std::vector<float> explicit_col_widths(cols, 0.0F);
        //explicit_col_widths are the widths specified directly on the cells, which take precedence over natural widths and weight-based allocation
        for (const auto &row: content.rows) {
            // Iterate over each cell in the row, up to the number of columns, to check for explicit width specifications.
            for (std::size_t c = 0; c < std::min(row.size(), cols); ++c) {
                const auto &cell = row[c];
                if (cell && cell->width() > 0.0F) {
                    // The explicit width is the maximum between the cell width and the explicit width already recorded
                    // for the column, ensuring that if multiple cells in the same column specify widths,
                    // the widest one is used as the explicit width for that column.
                    explicit_col_widths[c] = std::max(explicit_col_widths[c], cell->width());
                }
            }
        }
        // check if any explicit column widths were specified, which will affect how the remaining available width is allocated among the columns
        const bool has_explicit_col_width = std::ranges::any_of(explicit_col_widths, [](const float w) {
            return w > 0.0F;
        });
        // weights are used to distribute the remaining available width among the columns after accounting for natural widths and explicit widths
        const std::vector<float> weights = assign_weights(node->column_weights(), cols);
        const float total_weight = std::accumulate(weights.begin(), weights.end(), 0.0F);

        std::vector<float> col_widths(cols, 0.0F);
        float widths_sum = 0.0F;
        // For each column, determine its final width based on the following precedence:
        // - If an explicit width is specified for the column, use that.
        // - Otherwise, calculate a target width based on the column's weight relative to the total
        for (std::size_t i = 0; i < cols; ++i) {
            if (explicit_col_widths[i] > 0.0F) {
                col_widths[i] = explicit_col_widths[i];
            } else {
                //compute width based on weight allocation
                const float target = available_width * (weights[i] / total_weight);
                col_widths[i] = std::max(natural_widths[i], target);
            }
            widths_sum += col_widths[i]; // sum the width
        }

        // If there are no explicit column widths and the total of the computed column widths does not match the
        // available width, scale all column widths proportionally to fit the available width.
        if (!has_explicit_col_width && available_width > 0.0F && widths_sum > 0.0F && widths_sum != available_width) {
            const float scale = available_width / widths_sum;
            for (auto &w: col_widths) {
                w *= scale;
            }
            widths_sum = available_width;
        }
        //return width plan
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
            RowBand band = build_row_band(row);
            band.min_row_height = min_row_height;
            const float row_height = layout_row_band(band, y);
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
        compute_with_pipeline(node, box, cursor);
    }

    void DocraftLayoutHorizontalTableHandler::prepare_table_layout(const std::shared_ptr<model::DocraftTable> &node) {
        content_ = collect_table_content(node); // collect title and content nodes
        width_plan_ = compute_width_plan(node, content_); // compute column widths and total table width
        apply_width_plan(width_plan_); // apply computed widths to class state for use in row layout
    }

    float DocraftLayoutHorizontalTableHandler::layout_table_content(const std::shared_ptr<model::DocraftTable> &node) {
        const float min_row_height = std::max(0.0F, 2.0F * node->padding());

        RowBand title_band = build_row_band(content_.title_nodes);
        title_band.min_row_height = min_row_height;
        const float title_row_height = layout_row_band(title_band, get_fixed_y());

        const float body_start_y = get_fixed_y() - title_row_height;
        const float body_height = layout_body_rows(content_, body_start_y, min_row_height);
        return title_row_height + body_height;
    }

    float DocraftLayoutHorizontalTableHandler::resolve_table_width() const {
        return width_plan_.table_width;
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

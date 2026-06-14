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
#include <vector>
#include <fmt/format.h>

#include "docraft/layout/docraft_layout_engine.h"

namespace docraft::layout::handler {
    void DocraftLayoutVerticalTableHandler::setup_pipeline_state(const std::shared_ptr<model::DocraftTable> &node,
                                                                 DocraftCursor &cursor) {
        initialize_base_state(node, cursor, DocraftLayoutTableHandler::kVerticalCellPaddingX,
                              DocraftLayoutTableHandler::kVerticalCellPaddingY);
    }

    DocraftLayoutVerticalTableHandler::TableData DocraftLayoutVerticalTableHandler::collect_table_data(
        const std::shared_ptr<model::DocraftTable> &node) {
        ensure_title_nodes(node);

        const std::size_t row_count = node->titles().empty()
                                          ? node->title_nodes().size()
                                          : std::max(node->titles().size(), node->title_nodes().size());
        const std::size_t value_cols = static_cast<std::size_t>(std::max(1, node->content_cols()));

        if (row_count > node->title_nodes().size()) {
            throw docraft::exception::InvalidInputException(
                fmt::format("table node has more titles ({}) than title nodes ({})", node->titles().size(),
                            node->title_nodes().size()));
        }
        return TableData{
            .title_nodes = node->title_nodes(),
            .flat_values = flatten_content_nodes(node),
            .row_count = row_count,
            .value_cols = value_cols
        };
    }

    float DocraftLayoutVerticalTableHandler::compute_title_natural_width(const TableData &data) {
        auto &layout_service = edit_context()->edit_layout();
        auto &cursor = get_table_cursor();
        auto &engine = get_engine();
        float title_col_natural_width = 0.0F;
        // Foreach title node, compute its layout to determine its width, and keep track of the maximum width among all title nodes
        for (std::size_t r = 0; r < data.row_count; ++r) {
            const auto &title_node = data.title_nodes[r];
            const float saved_x = cursor.x();
            const float saved_y = cursor.y();
            // Move cursor the bottom of the page to avoid affecting current layout state, since we're only interested
            // in measuring the title node's width, not its position
            layout_service.set_current_rect_width(layout_service.available_space());
            cursor.move_to(get_fixed_x(), layout_service.page_height());
            //compute title node layout
            (void) engine.compute_layout(title_node, cursor);
            cursor.move_to(saved_x, saved_y); //reset cursor
            // title column natural width is the maximum width among all title nodes, which serves as a baseline for
            // allocating space to the title column in the final layout
            title_col_natural_width = std::max(title_col_natural_width, title_node->width());
        }

        return title_col_natural_width;
    }

    DocraftLayoutVerticalTableHandler::ColumnPlan DocraftLayoutVerticalTableHandler::resolve_column_plan(
        const std::shared_ptr<model::DocraftTable> &node,
        const TableData &data,
        const float title_natural_width) const {
        // compute the available width
        const float available_width = available_width_for(node, title_natural_width);
        // compute the weights for the title and value columns based on the node's column_weights property
        const std::vector<float> weights = assign_weights(node->column_weights(), 2);
        // the total weight is the sum of the title column weight and the value columns weight,
        // which is used to determine how to distribute the available width between the title and value columns
        const float total_weight = weights[0] + weights[1];
        // the title column width is determined by taking the maximum of the natural width of the title column and the
        // portion of the available width allocated to it based on its weight.
        const float title_col_width = std::max(title_natural_width, available_width * (weights[0] / total_weight));
        // The values block width is the remaining width after allocating space for the title column. The value
        // column width is determined by dividing the value block width by the number of value columns,
        // unless there are explicit widths specified on the cells, in which case the maximum explicit width is used
        // for the value column width.
        const float values_block_width = std::max(0.0F, available_width - title_col_width);
        float value_col_width = (data.value_cols > 0)
                                    ? (values_block_width / static_cast<float>(data.value_cols))
                                    : values_block_width;

        float explicit_value_col_width = 0.0F;
        // Foreach cell check if it has an explicit width specified and get the max between cell width and the current explicit value column width
        for (const auto &cell: data.flat_values) {
            if (cell && cell->width() > 0.0F) {
                explicit_value_col_width = std::max(explicit_value_col_width, cell->width());
            }
        }
        //if there is an explicit value column width specified on any cell,
        //it takes precedence over the computed value column width based on weights and available space,
        //ensuring that the layout respects explicit width specifications from the content.
        if (explicit_value_col_width > 0.0F) {
            value_col_width = explicit_value_col_width;
        }
        // return the cols plan
        return ColumnPlan{
            .title_col_width = title_col_width,
            .value_col_width = value_col_width,
            .value_cols = data.value_cols
        };
    }

    float DocraftLayoutVerticalTableHandler::layout_header_row(const std::shared_ptr<model::DocraftTable> &node,
                                                               const ColumnPlan &plan,
                                                               const float row_top_y,
                                                               const float min_row_height) {
        if (node->htitle_nodes().empty()) {
            return 0.0F;
        }

        RowBand band = build_header_band(node, plan);
        band.min_row_height = min_row_height;
        return layout_row_band(band, row_top_y);
    }

    float DocraftLayoutVerticalTableHandler::layout_body_rows(const TableData &data,
                                                              const ColumnPlan &plan,
                                                              const float row_top_y,
                                                              const float min_row_height) {
        float y = row_top_y;
        float total_height = 0.0F;
        // For each row, we need to build a band that includes the title cell and the value cells for that row.
        for (std::size_t r = 0; r < data.row_count; ++r) {
            // Build a row band for this row, which includes the title node and the value nodes for this row,
            // with their corresponding left positions and widths based on the column plan.
            RowBand band = build_body_band(data, plan, r);
            band.min_row_height = min_row_height;
            const float row_height = layout_row_band(band, y);
            total_height += row_height;
            y -= row_height;
        }
        return total_height;
    }

    DocraftLayoutTableHandler::RowBand DocraftLayoutVerticalTableHandler::build_header_band(
        const std::shared_ptr<model::DocraftTable> &node,
        const ColumnPlan &plan) const {
        RowBand band;
        // The header cols is the min between the number of value columns and the number of horizontal title nodes,
        // since we can't have more header cells than value columns, and we can't use more header nodes than we have.
        const std::size_t header_cols = std::min(plan.value_cols, node->htitle_nodes().size());
        band.nodes.reserve(header_cols);
        band.lefts.reserve(header_cols);
        band.widths.reserve(header_cols);
        // The header row only includes the horizontal title nodes
        for (std::size_t c = 0; c < header_cols; ++c) {
            // add the horizontal title node for this column as a cell in the band
            band.nodes.emplace_back(node->htitle_nodes()[c]);
            // The horizontal title cells are positioned in the value columns, so we calculate their left positions accordingly
            band.lefts.emplace_back(
                get_fixed_x() + plan.title_col_width + (static_cast<float>(c) * plan.value_col_width));
            // The width of each horizontal title cell is the same as the value column width
            band.widths.emplace_back(plan.value_col_width);
        }
        return band;
    }

    DocraftLayoutTableHandler::RowBand DocraftLayoutVerticalTableHandler::build_body_band(
        const TableData &data,
        const ColumnPlan &plan,
        const std::size_t row_index) const {
        RowBand band;
        band.nodes.reserve(plan.value_cols + 1); // reserve space for title + value cells
        band.lefts.reserve(plan.value_cols + 1); // reserve space for title + value cells
        band.widths.reserve(plan.value_cols + 1); // reserve space for title + value cells

        band.nodes.emplace_back(data.title_nodes[row_index]);
        // add the title node for this row as the first cell in the band
        band.lefts.emplace_back(get_fixed_x()); // title column starts at the fixed x position
        band.widths.emplace_back(plan.title_col_width); // the plan determines title column width

        const std::size_t row_first = row_index * plan.value_cols;
        // add value cells for this row, using the flat values vector and the plan's value column width
        for (std::size_t vc = 0; vc < plan.value_cols; ++vc) {
            const std::size_t idx = row_first + vc;
            if (idx >= data.flat_values.size()) {
                break;
            }
            band.nodes.emplace_back(data.flat_values[idx]);
            band.lefts.emplace_back(
                get_fixed_x() + plan.title_col_width + (static_cast<float>(vc) * plan.value_col_width));
            band.widths.emplace_back(plan.value_col_width);
        }

        return band;
    }

    void DocraftLayoutVerticalTableHandler::compute(const std::shared_ptr<model::DocraftTable> &node,
                                                    model::DocraftTransform *box,
                                                    DocraftCursor &cursor) {
        compute_with_pipeline(node, box, cursor);
    }

    void DocraftLayoutVerticalTableHandler::prepare_table_layout(const std::shared_ptr<model::DocraftTable> &node) {
        data_ = collect_table_data(node); // collect title and value nodes into a structured format
        // compute the natural width required by the title column based on title nodes
        const float title_natural_width = compute_title_natural_width(data_);
        // resolve the final column widths for the title and value columns, taking into account weights and explicit widths
        plan_ = resolve_column_plan(node, data_, title_natural_width);
        // compute the total table width based on the resolved column widths and number of value columns
        table_width_ = plan_.title_col_width + (plan_.value_col_width * static_cast<float>(plan_.value_cols));
    }

    float DocraftLayoutVerticalTableHandler::layout_table_content(const std::shared_ptr<model::DocraftTable> &node) {
        const float min_row_height = std::max(0.0F, 2.0F * node->padding()); // match horizontal table behavior
        //2*padding means that the row height will be at least enough to accommodate the padding on both top and bottom of the cell content
        // layout header row and get its height
        const float header_height = layout_header_row(node, plan_, get_fixed_y(), min_row_height);
        const float body_top_y = get_fixed_y() - header_height; // compute the starting y position for the body rows
        // layout body rows and get their total height
        const float body_height = layout_body_rows(data_, plan_, body_top_y, min_row_height);
        return header_height + body_height; // return the total height consumed by the table (header + body)
    }

    float DocraftLayoutVerticalTableHandler::resolve_table_width() const {
        return table_width_;
    }
} // namespace docraft::layout::handler

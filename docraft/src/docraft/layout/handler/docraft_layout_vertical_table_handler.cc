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
    void DocraftLayoutVerticalTableHandler::setup_compute_state(const std::shared_ptr<model::DocraftTable> &node,
                                                                DocraftCursor &cursor) {
        initialize_base_state(node, cursor, kVerticalCellPaddingX, kVerticalCellPaddingY);
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
                fmt::format("table node has more titles ({}) than title nodes ({})", node->titles().size(), node->title_nodes().size()));
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

        for (std::size_t r = 0; r < data.row_count; ++r) {
            const auto &title_node = data.title_nodes[r];
            const float saved_x = cursor.x();
            const float saved_y = cursor.y();
            layout_service.set_current_rect_width(layout_service.available_space());
            cursor.move_to(get_fixed_x(), layout_service.page_height());
            (void) engine.compute_layout(title_node, cursor);
            cursor.move_to(saved_x, saved_y);

            title_col_natural_width = std::max(title_col_natural_width, title_node->width());
        }

        return title_col_natural_width;
    }

    DocraftLayoutVerticalTableHandler::ColumnPlan DocraftLayoutVerticalTableHandler::resolve_column_plan(
        const std::shared_ptr<model::DocraftTable> &node,
        const TableData &data,
        const float title_natural_width) const {
        const float available_width = available_width_for(node, title_natural_width);

        const std::vector<float> weights = assign_weights(node->column_weights(), 2);
        const float total_weight = weights[0] + weights[1];

        const float title_col_width = std::max(title_natural_width, available_width * (weights[0] / total_weight));
        const float values_block_width = std::max(0.0F, available_width - title_col_width);
        float value_col_width = (data.value_cols > 0)
                                    ? (values_block_width / static_cast<float>(data.value_cols))
                                    : values_block_width;

        float explicit_value_col_width = 0.0F;
        for (const auto &cell: data.flat_values) {
            if (cell && cell->width() > 0.0F) {
                explicit_value_col_width = std::max(explicit_value_col_width, cell->width());
            }
        }
        if (explicit_value_col_width > 0.0F) {
            value_col_width = explicit_value_col_width;
        }

        return ColumnPlan{
            .title_col_width = title_col_width,
            .value_col_width = value_col_width,
            .value_cols = data.value_cols
        };
    }

    float DocraftLayoutVerticalTableHandler::layout_header_row(const std::shared_ptr<model::DocraftTable> &node,
                                                               const ColumnPlan &plan,
                                                               const float row_top_y) {
        if (node->htitle_nodes().empty()) {
            return 0.0F;
        }

        return layout_row_band(build_header_band(node, plan), row_top_y, 0.0F);
    }

    float DocraftLayoutVerticalTableHandler::layout_body_rows(const TableData &data,
                                                              const ColumnPlan &plan,
                                                              const float row_top_y) {
        float y = row_top_y;
        float total_height = 0.0F;

        for (std::size_t r = 0; r < data.row_count; ++r) {
            // For each row, we need to build a band that includes the title cell and the value cells for that row.
            const float row_height = layout_row_band(build_body_band(data, plan, r), y, 0.0F);
            total_height += row_height;
            y -= row_height;
        }
        return total_height;
    }

    DocraftLayoutTableHandler::RowBand DocraftLayoutVerticalTableHandler::build_header_band(
        const std::shared_ptr<model::DocraftTable> &node,
        const ColumnPlan &plan) const {
        RowBand band;
        const std::size_t header_cols = std::min(plan.value_cols, node->htitle_nodes().size());
        band.nodes.reserve(header_cols);
        band.lefts.reserve(header_cols);
        band.widths.reserve(header_cols);

        for (std::size_t c = 0; c < header_cols; ++c) {
            band.nodes.emplace_back(node->htitle_nodes()[c]);
            band.lefts.emplace_back(
                get_fixed_x() + plan.title_col_width + (static_cast<float>(c) * plan.value_col_width));
            band.widths.emplace_back(plan.value_col_width);
        }
        return band;
    }

    DocraftLayoutTableHandler::RowBand DocraftLayoutVerticalTableHandler::build_body_band(
        const TableData &data,
        const ColumnPlan &plan,
        const std::size_t row_index) const {
        RowBand band;
        band.nodes.reserve(plan.value_cols + 1);
        band.lefts.reserve(plan.value_cols + 1);
        band.widths.reserve(plan.value_cols + 1);

        band.nodes.emplace_back(data.title_nodes[row_index]);
        band.lefts.emplace_back(get_fixed_x());
        band.widths.emplace_back(plan.title_col_width);

        const std::size_t row_first = row_index * plan.value_cols;
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
        setup_compute_state(node, cursor);
        const TableData data = collect_table_data(node);
        const float title_natural_width = compute_title_natural_width(data);
        const ColumnPlan plan = resolve_column_plan(node, data, title_natural_width);

        const float header_height = layout_header_row(node, plan, get_fixed_y());
        const float body_top_y = get_fixed_y() - header_height;
        const float body_height = layout_body_rows(data, plan, body_top_y);

        const float table_width = plan.title_col_width + (plan.value_col_width * static_cast<float>(plan.value_cols));
        finalize_output(node, box, table_width, header_height + body_height);
        log_cells(node);
        clear_compute_state();
    }

} // namespace docraft::layout::handler

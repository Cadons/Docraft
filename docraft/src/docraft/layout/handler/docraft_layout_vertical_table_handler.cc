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

#include "docraft/layout/docraft_layout_engine.h"

namespace docraft::layout::handler {
    void DocraftLayoutVerticalTableHandler::setup_compute_state(const std::shared_ptr<model::DocraftTable> &node,
                                                                DocraftCursor &cursor) {
        initialize_base_state(node, cursor, kVerticalCellPaddingX, kVerticalCellPaddingY);
        padding_x_ = 2.0F * kVerticalCellPaddingX;
        padding_y_ = 2.0F * kVerticalCellPaddingY;
    }

    DocraftLayoutVerticalTableHandler::TableData DocraftLayoutVerticalTableHandler::collect_table_data(
        const std::shared_ptr<model::DocraftTable> &node) {
        ensure_title_nodes(node);

        const std::size_t row_count = node->titles().empty() ? node->title_nodes().size() : node->titles().size();
        const std::size_t value_cols = static_cast<std::size_t>(std::max(1, node->content_cols()));

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

        const std::size_t header_cols = std::min(plan.value_cols, node->htitle_nodes().size());
        std::vector<std::shared_ptr<model::DocraftNode> > header_nodes;
        std::vector<float> header_lefts;
        std::vector<float> header_widths;
        //Allocate header nodes and compute their widths
        header_nodes.reserve(header_cols);
        header_lefts.reserve(header_cols);
        header_widths.reserve(header_cols);

        for (std::size_t c = 0; c < header_cols; ++c) {
            header_nodes.emplace_back(node->htitle_nodes()[c]);
            header_lefts.emplace_back(
                get_fixed_x() + plan.title_col_width + (static_cast<float>(c) * plan.value_col_width));
            header_widths.emplace_back(plan.value_col_width);
        }

        const BandView header_band{.nodes = &header_nodes, .lefts = &header_lefts, .widths = &header_widths};
        const float header_row_height = compute_band_height(header_band, row_top_y);
        place_band(header_band, row_top_y, header_row_height);
        return header_row_height;
    }

    float DocraftLayoutVerticalTableHandler::layout_body_rows(const TableData &data,
                                                              const ColumnPlan &plan,
                                                              const float row_top_y) {
        float y = row_top_y;
        float total_height = 0.0F;

        for (std::size_t r = 0; r < data.row_count; ++r) {
            std::vector<std::shared_ptr<model::DocraftNode> > row_nodes;
            std::vector<float> row_lefts;
            std::vector<float> row_widths;
            row_nodes.reserve(plan.value_cols + 1);
            row_lefts.reserve(plan.value_cols + 1);
            row_widths.reserve(plan.value_cols + 1);

            row_nodes.emplace_back(data.title_nodes[r]);
            row_lefts.emplace_back(get_fixed_x());
            row_widths.emplace_back(plan.title_col_width);

            const std::size_t row_first = r * plan.value_cols;
            for (std::size_t vc = 0; vc < plan.value_cols; ++vc) {
                const std::size_t idx = row_first + vc;
                if (idx >= data.flat_values.size()) {
                    break;
                }
                row_nodes.emplace_back(data.flat_values[idx]);
                row_lefts.emplace_back(
                    get_fixed_x() + plan.title_col_width + (static_cast<float>(vc) * plan.value_col_width));
                row_widths.emplace_back(plan.value_col_width);
            }

            const BandView row_band{.nodes = &row_nodes, .lefts = &row_lefts, .widths = &row_widths};
            // Compute height using the actual y position where content will be placed
            // This ensures page break calculations are consistent
            const float cell_y = y; // Actual y position for cells
            const float row_height = compute_band_height(row_band, cell_y);
            place_band(row_band, cell_y, row_height);

            total_height += row_height;
            y -= row_height;
        }

        return total_height;
    }

    float DocraftLayoutVerticalTableHandler::compute_band_height(const BandView &band,
                                                                 const float row_top_y) {
        float row_height = 0.0F;
        for (std::size_t i = 0; i < band.nodes->size(); ++i) {
            const auto &cell_node = (*band.nodes)[i];
            if (!cell_node) {
                continue;
            }
            // Layout the cell with proper width accounting for padding
            compute_cell_layout(cell_node,
                                std::max(0.0F, (*band.widths)[i] - get_padding_x()),
                                (*band.lefts)[i] + get_cell_padding_x(),
                                row_top_y - get_cell_padding_y());
            // Height includes both the content height and vertical padding
            row_height = std::max(row_height, cell_node->height() + get_padding_y());
        }
        return row_height;
    }

    void DocraftLayoutVerticalTableHandler::place_band(const BandView &band,
                                                       const float row_top_y,
                                                       const float row_height) const {
        for (std::size_t i = 0; i < band.nodes->size(); ++i) {
            const auto &cell_node = (*band.nodes)[i];
            if (!cell_node) {
                continue;
            }
            // Use VERTICAL padding for alignment and account for full cell height with padding
            auto vertical_padding_offset = 2.0F * kVerticalCellPaddingY;
            center_text_in_row(cell_node, row_top_y, row_height + (2.0F * vertical_padding_offset),
                               get_baseline_offset());
            cell_node->set_position({.x = (*band.lefts)[i], .y = row_top_y});
            cell_node->set_width((*band.widths)[i]);
            cell_node->set_height(row_height);
        }
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


    float DocraftLayoutVerticalTableHandler::get_padding_x() const {
        return padding_x_;
    }

    float DocraftLayoutVerticalTableHandler::get_padding_y() const {
        return padding_y_;
    }

} // namespace docraft::layout::handler

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
#include <vector>

#include "docraft_layout_table_handler_impl.h"
#include "docraft/layout/docraft_layout_engine.h"
#include "docraft/utils/docraft_logger.h"

namespace docraft::layout::handler {
    void DocraftLayoutVerticalTableHandler::setup_compute_state(const std::shared_ptr<model::DocraftTable> &node,
                                                                DocraftCursor &cursor) {
        auto &layout_service = edit_context()->edit_layout();
        table_cursor_ = cursor;
        table_impl::configure_cursor_position(node, table_cursor_);

        fixed_x_ = table_cursor_.x();
        fixed_y_ = table_cursor_.y();
        saved_available_space_ = layout_service.available_space();

        context_ = edit_context();
        engine_.emplace(context_, false);

        constexpr float kCellPaddingY = table_impl::kVerticalCellPaddingY;
        constexpr float kCellPaddingX = table_impl::kVerticalCellPaddingX;
        baseline_offset_ = node->baseline_offset();
        cell_padding_x_ = kCellPaddingX;
        cell_padding_y_ = kCellPaddingY;
        padding_x_ = 2.0F * kCellPaddingX;
        padding_y_ = 2.0F * kCellPaddingY;
    }

    DocraftLayoutVerticalTableHandler::TableData DocraftLayoutVerticalTableHandler::collect_table_data(
        const std::shared_ptr<model::DocraftTable> &node) {
        table_impl::ensure_title_nodes(node);

        const std::size_t row_count = node->titles().empty() ? node->title_nodes().size() : node->titles().size();
        const std::size_t value_cols = static_cast<std::size_t>(std::max(1, node->content_cols()));

        return TableData{
            .title_nodes = node->title_nodes(),
            .flat_values = table_impl::flatten_content_nodes(node),
            .row_count = row_count,
            .value_cols = value_cols
        };
    }

    float DocraftLayoutVerticalTableHandler::compute_title_natural_width(const TableData &data) {
        auto &layout_service = edit_context()->edit_layout();
        float title_col_natural_width = 0.0F;

        for (std::size_t r = 0; r < data.row_count; ++r) {
            const auto &title_node = data.title_nodes[r];
            const float saved_x = table_cursor_.x();
            const float saved_y = table_cursor_.y();
            layout_service.set_current_rect_width(layout_service.available_space());
            table_cursor_.move_to(fixed_x_, layout_service.page_height());
            (void) engine_->compute_layout(title_node, table_cursor_);
            table_cursor_.move_to(saved_x, saved_y);

            title_col_natural_width = std::max(title_col_natural_width, title_node->width());
        }

        return title_col_natural_width;
    }

    DocraftLayoutVerticalTableHandler::ColumnPlan DocraftLayoutVerticalTableHandler::resolve_column_plan(
        const std::shared_ptr<model::DocraftTable> &node,
        const TableData &data,
        const float title_natural_width) const {
        const float available_width = table_impl::available_width_for(node, context_, title_natural_width);

        const std::vector<float> weights = table_impl::assign_weights(node->column_weights(), 2);
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
        header_nodes.reserve(header_cols);
        header_lefts.reserve(header_cols);
        header_widths.reserve(header_cols);

        for (std::size_t c = 0; c < header_cols; ++c) {
            header_nodes.emplace_back(node->htitle_nodes()[c]);
            header_lefts.emplace_back(fixed_x_ + plan.title_col_width + (static_cast<float>(c) * plan.value_col_width));
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
            row_lefts.emplace_back(fixed_x_);
            row_widths.emplace_back(plan.title_col_width);

            const std::size_t row_first = r * plan.value_cols;
            for (std::size_t vc = 0; vc < plan.value_cols; ++vc) {
                const std::size_t idx = row_first + vc;
                if (idx >= data.flat_values.size()) {
                    break;
                }
                row_nodes.emplace_back(data.flat_values[idx]);
                row_lefts.emplace_back(
                    fixed_x_ + plan.title_col_width + (static_cast<float>(vc) * plan.value_col_width));
                row_widths.emplace_back(plan.value_col_width);
            }

            const BandView row_band{.nodes = &row_nodes, .lefts = &row_lefts, .widths = &row_widths};
            const float row_height = compute_band_height(row_band, y);
            place_band(row_band, y, row_height);

            total_height += row_height;
            y -= row_height;
        }

        return total_height;
    }

    void DocraftLayoutVerticalTableHandler::finalize_output(const std::shared_ptr<model::DocraftTable> &node,
                                                            model::DocraftTransform *box,
                                                            const float table_width,
                                                            const float table_height) {
        auto &layout_service = edit_context()->edit_layout();
        node->set_position({.x = fixed_x_, .y = fixed_y_});
        node->set_width(table_width);
        node->set_height(table_height);

        if (box) {
            box->set_position(node->position());
            box->set_width(node->width());
            box->set_height(node->height());
        }
        layout_service.set_current_rect_width(saved_available_space_);
    }

    void DocraftLayoutVerticalTableHandler::log_cells(const std::shared_ptr<model::DocraftTable> &node) {
        for (const auto &row: node->content_nodes()) {
            for (const auto &cell: row) {
                if (cell) {
                    LOG_DEBUG(fmt::format("Cell at ({}, {}) with size ({}, {})", cell->position().x,
                        cell->position().y, cell->width(), cell->height()));
                }
            }
        }
    }

    void DocraftLayoutVerticalTableHandler::clear_compute_state() {
        engine_.reset();
    }

    float DocraftLayoutVerticalTableHandler::compute_band_height(const BandView &band,
                                                                 const float row_top_y) {
        float row_height = 0.0F;
        for (std::size_t i = 0; i < band.nodes->size(); ++i) {
            const auto &cell_node = (*band.nodes)[i];
            if (!cell_node) {
                continue;
            }
            table_impl::compute_cell_layout({
                .node = cell_node,
                .engine = *engine_,
                .context = context_,
                .table_cursor = table_cursor_,
                .inner_width = std::max(0.0F, (*band.widths)[i] - padding_x_),
                .x = (*band.lefts)[i] + cell_padding_x_,
                .y = row_top_y - cell_padding_y_
            });
            row_height = std::max(row_height, cell_node->height() + padding_y_);
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
            table_impl::center_text_in_row(cell_node, row_top_y, row_height, baseline_offset_);
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

        const float header_height = layout_header_row(node, plan, fixed_y_);
        const float body_top_y = fixed_y_ - header_height;
        const float body_height = layout_body_rows(data, plan, body_top_y);

        const float table_width = plan.title_col_width + (plan.value_col_width * static_cast<float>(plan.value_cols));
        finalize_output(node, box, table_width, header_height + body_height);
        log_cells(node);
        clear_compute_state();
    }
} // namespace docraft::layout::handler

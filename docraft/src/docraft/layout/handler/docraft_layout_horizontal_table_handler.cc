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
#include "docraft/utils/docraft_logger.h"

namespace docraft::layout::handler {
    void DocraftLayoutHorizontalTableHandler::setup_compute_state(const std::shared_ptr<model::DocraftTable> &node,
                                                                  DocraftCursor &cursor) {
        auto &layout_service = edit_context()->edit_layout();
        table_cursor_ = cursor;
        table_impl::configure_cursor_position(node, table_cursor_);

        fixed_x_ = table_cursor_.x();
        fixed_y_ = table_cursor_.y() - node->padding();
        saved_available_space_ = layout_service.available_space();

        context_ = edit_context();
        engine_.emplace(context_, false);

        constexpr float kCellPaddingY = table_impl::kHorizontalCellPaddingY;
        constexpr float kCellPaddingX = table_impl::kHorizontalCellPaddingX;
        baseline_offset_ = node->baseline_offset();
        cell_padding_x_ = kCellPaddingX;
        cell_padding_y_ = kCellPaddingY;
        offset_y_ = 2.0F * kCellPaddingY;
        offset_x_ = 2.0F * kCellPaddingX;
    }

    DocraftLayoutHorizontalTableHandler::TableContent DocraftLayoutHorizontalTableHandler::collect_table_content(
        const std::shared_ptr<model::DocraftTable> &node) {
        table_impl::ensure_title_nodes(node);
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
        for (std::size_t i = 0; i < cols; ++i) {
            const auto &title_node = content.title_nodes[i];
            const float saved_x = table_cursor_.x();
            const float saved_y = table_cursor_.y();
            table_cursor_.move_to(0.0F, layout_service.page_height());
            (void) engine_->compute_layout(title_node, table_cursor_);
            table_cursor_.move_to(saved_x, saved_y);

            natural_widths[i] = title_node->width();
        }

        const float natural_sum = std::accumulate(natural_widths.begin(), natural_widths.end(), 0.0F);
        const float available_width = table_impl::available_width_for(node, context_, natural_sum);

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

        const std::vector<float> weights = table_impl::assign_weights(node->column_weights(), cols);
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
        const auto lefts = table_impl::build_column_lefts(fixed_x_, col_widths_);
        col_lefts_.assign(lefts.begin(), lefts.end());
        cols_ = col_widths_.size();
    }

    float DocraftLayoutHorizontalTableHandler::layout_body_rows(const TableContent &content,
                                                                const float start_y,
                                                                const float min_row_height) {
        float y = start_y;
        float total_content_height = 0.0F;
        for (const auto &row: content.rows) {
            const float row_height = layout_row(row, y, min_row_height);
            total_content_height += row_height;
            y -= row_height;
        }
        return total_content_height;
    }

    void DocraftLayoutHorizontalTableHandler::finalize_output(const std::shared_ptr<model::DocraftTable> &node,
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

    void DocraftLayoutHorizontalTableHandler::log_cells(const std::shared_ptr<model::DocraftTable> &node) {
        for (const auto &row: node->content_nodes()) {
            for (const auto &cell: row) {
                if (cell) {
                    LOG_DEBUG(fmt::format("Cell at ({}, {}) with size ({}, {})", cell->position().x,
                        cell->position().y, cell->width(), cell->height()));
                }
            }
        }
    }

    void DocraftLayoutHorizontalTableHandler::clear_compute_state() {
        engine_.reset();
    }

    float DocraftLayoutHorizontalTableHandler::layout_row(
        const std::vector<std::shared_ptr<model::DocraftNode> > &row_nodes,
        const float row_top_y,
        const float min_row_height) {
        float row_height = min_row_height;
        for (std::size_t c = 0; c < std::min(row_nodes.size(), cols_); ++c) {
            const auto &cell_node = row_nodes[c];
            if (!cell_node) {
                continue;
            }
            table_impl::compute_cell_layout({
                .node = cell_node,
                .engine = *engine_,
                .context = context_,
                .table_cursor = table_cursor_,
                .inner_width = std::max(0.0F, col_widths_[c] - offset_x_),
                .x = col_lefts_[c] + cell_padding_x_,
                .y = row_top_y - cell_padding_y_
            });
            row_height = std::max(row_height, cell_node->height());
        }

        for (std::size_t c = 0; c < std::min(row_nodes.size(), cols_); ++c) {
            const auto &cell_node = row_nodes[c];
            if (!cell_node) {
                continue;
            }
            table_impl::center_text_in_row(cell_node, row_top_y, row_height + (2.0F * offset_y_), baseline_offset_);
            cell_node->set_position({.x = col_lefts_[c], .y = row_top_y});
            cell_node->set_width(col_widths_[c]);
            cell_node->set_height(row_height);
        }

        return row_height;
    }

    void DocraftLayoutHorizontalTableHandler::compute(const std::shared_ptr<model::DocraftTable> &node,
                                                      model::DocraftTransform *box,
                                                      DocraftCursor &cursor) {
        setup_compute_state(node, cursor);
        const TableContent content = collect_table_content(node);
        const WidthPlan width_plan = compute_width_plan(node, content);
        apply_width_plan(width_plan);

        const float min_row_height = 20.0F + offset_y_;
        const float title_row_height = layout_row(content.title_nodes, fixed_y_, min_row_height);
        const float body_start_y = fixed_y_ - title_row_height;
        const float body_height = layout_body_rows(content, body_start_y, min_row_height);

        finalize_output(node, box, width_plan.table_width, title_row_height + body_height);
        log_cells(node);
        clear_compute_state();
    }
} // namespace docraft::layout::handler

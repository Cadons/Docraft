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

#include "docraft/layout/handler/docraft_layout_table_handler.h"

#include <algorithm>
#include <fmt/format.h>
#include <numeric>
#include <vector>

#include "docraft/exception/docraft_exceptions.h"
#include "docraft/layout/handler/docraft_layout_horizontal_table_handler.h"
#include "docraft/layout/handler/docraft_layout_vertical_table_handler.h"
#include "docraft/model/docraft_text.h"
#include "docraft/utils/docraft_logger.h"

namespace docraft::layout::handler {
    void DocraftLayoutTableHandler::compute(const std::shared_ptr<model::DocraftTable> &node,
                                            model::DocraftTransform *box,
                                            DocraftCursor &cursor) {
        if (!node) {
            throw docraft::exception::InvalidInputException("table node is null");
        }

        if (node->titles().empty() && node->title_nodes().empty()) {
            if (node->position_mode() == model::DocraftPositionType::kBlock) {
                node->set_position({.x = cursor.x(), .y = cursor.y()});
            } else {
                node->set_position({.x = node->position().x, .y = node->position().y});
            }
            node->set_width(0.0F);
            node->set_height(0.0F);

            if (box) {
                box->set_position(node->position());
                box->set_width(node->width());
                box->set_height(node->height());
            }
            return;
        }

        switch (node->orientation()) {
            case model::LayoutOrientation::kHorizontal: {
                DocraftLayoutHorizontalTableHandler h_handler(edit_context());
                h_handler.compute(node, box, cursor);
                break;
            }
            case model::LayoutOrientation::kVertical: {
                DocraftLayoutVerticalTableHandler v_handler(edit_context());
                v_handler.compute(node, box, cursor);
                break;
            }
            default:
                throw docraft::exception::LayoutConfigurationException("unsupported table orientation");
        }
    }

    bool DocraftLayoutTableHandler::handle(const std::shared_ptr<model::DocraftNode> &request,
                                           model::DocraftTransform *result,
                                           DocraftCursor &cursor) {
        if (auto table_node = std::dynamic_pointer_cast<model::DocraftTable>(request)) {
            compute(table_node, result, cursor);
            return true;
        }
        return false;
    }

    void DocraftLayoutTableHandler::compute_with_pipeline(const std::shared_ptr<model::DocraftTable> &node,
                                                          model::DocraftTransform *box,
                                                          DocraftCursor &cursor) {
        setup_pipeline_state(node, cursor);
        prepare_table_layout(node);
        const float table_height = layout_table_content(node);
        finalize_output(node, box, resolve_table_width(), table_height);
        log_cells(node);
        clear_compute_state();
    }

    void DocraftLayoutTableHandler::setup_pipeline_state(const std::shared_ptr<model::DocraftTable> &,
                                                         DocraftCursor &) {
        throw docraft::exception::LayoutConfigurationException(
            "table pipeline hook setup_pipeline_state not implemented");
    }

    void DocraftLayoutTableHandler::prepare_table_layout(const std::shared_ptr<model::DocraftTable> &) {
        throw docraft::exception::LayoutConfigurationException(
            "table pipeline hook prepare_table_layout not implemented");
    }

    float DocraftLayoutTableHandler::layout_table_content(const std::shared_ptr<model::DocraftTable> &) {
        throw docraft::exception::LayoutConfigurationException(
            "table pipeline hook layout_table_content not implemented");
    }

    float DocraftLayoutTableHandler::resolve_table_width() const {
        throw docraft::exception::LayoutConfigurationException(
            "table pipeline hook resolve_table_width not implemented");
    }


    void DocraftLayoutTableHandler::initialize_base_state(const std::shared_ptr<model::DocraftTable> &node,
                                                          DocraftCursor &cursor,
                                                          float cell_padding_x,
                                                          float cell_padding_y) {
        auto &layout_service = edit_context()->edit_layout();
        table_cursor_ = cursor;
        configure_cursor_position(node, table_cursor_);

        fixed_x_ = table_cursor_.x();
        fixed_y_ = table_cursor_.y();
        saved_available_space_ = layout_service.available_space();

        context_ = edit_context();
        engine_.emplace(context_, false);

        baseline_offset_ = node->baseline_offset();
        cell_padding_x_ = cell_padding_x;
        cell_padding_y_ = cell_padding_y;
    }

    void DocraftLayoutTableHandler::ensure_title_nodes(const std::shared_ptr<model::DocraftTable> &node) {
        const auto &titles = node->titles();
        if (node->title_nodes().size() < titles.size()) {
            for (std::size_t i = node->title_nodes().size(); i < titles.size(); ++i) {
                auto title_node = std::make_shared<model::DocraftText>();
                title_node->set_text(titles[i]);
                node->add_title_node(title_node);
            }
        }
    }

    std::vector<std::shared_ptr<model::DocraftNode> > DocraftLayoutTableHandler::flatten_content_nodes(
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

    float DocraftLayoutTableHandler::available_width_for(const std::shared_ptr<model::DocraftTable> &node,
                                                         float fallback) const {
        if (node->auto_fill_width()) {
            return std::max(0.0F, context_->layout().available_space());
        }
        const float w = node->width();
        if (w > 0.0F) return std::max(0.0F, w);
        return std::max(0.0F, fallback);
    }

    void DocraftLayoutTableHandler::configure_cursor_position(const std::shared_ptr<model::DocraftTable> &node,
                                                              DocraftCursor &cursor) {
        if (node->position_mode() != model::DocraftPositionType::kBlock) {
            cursor.move_to(node->position().x, node->position().y);
        }
    }

    void DocraftLayoutTableHandler::center_text_in_row(const std::shared_ptr<model::DocraftNode> &node,
                                                       float row_top_y,
                                                       float row_height,
                                                       float baseline_offset) {
        const auto text_node = std::dynamic_pointer_cast<model::DocraftText>(node);
        if (!text_node) return;
        const float current_center = (text_node->anchors().top_left.y + text_node->anchors().bottom_left.y) * 0.5F;
        const float desired_center = row_top_y - (row_height * 0.5F) + (baseline_offset * text_node->font_size());
        const float delta = current_center - desired_center;
        if (delta != 0.0F) {
            text_node->set_y_for_children(delta);
        }
    }

    void DocraftLayoutTableHandler::compute_cell_layout(const std::shared_ptr<model::DocraftNode> &node,
                                                        float inner_width,
                                                        float x,
                                                        float y) {
        auto &layout_service = context_->edit_layout();
        const float saved_x = table_cursor_.x();
        const float saved_y = table_cursor_.y();

        node->set_height(0.0F); // reset height to allow content to determine it
        layout_service.set_current_rect_width(inner_width); //inner width for cell content
        table_cursor_.move_to(x, y);
        (void) engine_->compute_layout(node, table_cursor_);
        table_cursor_.move_to(saved_x, saved_y);
    }

    //This function is used to adjust the position of the cell content to ensure that text nodes are vertically centered within the cell.
    std::vector<float> DocraftLayoutTableHandler::build_column_lefts(float fixed_x,
                                                                     const std::vector<float> &col_widths) {
        std::vector col_lefts(col_widths.size(), fixed_x);
        float x = fixed_x;
        for (std::size_t i = 0; i < col_widths.size(); ++i) {
            col_lefts[i] = x; // assign left position for column i
            x += col_widths[i]; // move x to the right edge of column i for the next iteration
        }
        return col_lefts;
    }

    std::vector<float> DocraftLayoutTableHandler::assign_weights(const std::vector<float> &source,
                                                                 std::size_t cols) {
        std::vector<float> weights = source;
        if (weights.size() != cols) {
            weights.assign(cols, 1.0F);
        }
        const float total = std::accumulate(weights.begin(), weights.end(), 0.0F);
        if (total <= 0.0F) {
            weights.assign(cols, 1.0F);
        }
        return weights;
    }

    void DocraftLayoutTableHandler::finalize_output(const std::shared_ptr<model::DocraftTable> &node,
                                                    model::DocraftTransform *box,
                                                    float table_width,
                                                    float table_height) {
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

    void DocraftLayoutTableHandler::log_cells(const std::shared_ptr<model::DocraftTable> &node) {
        for (const auto &row: node->content_nodes()) {
            for (const auto &cell: row) {
                if (cell) {
                    LOG_DEBUG(fmt::format("Cell at ({}, {}) with size ({}, {})", cell->position().x,
                        cell->position().y, cell->width(), cell->height()));
                }
            }
        }
    }

    std::size_t DocraftLayoutTableHandler::effective_band_size(const RowBand &band) {
        if (band.lefts.size() != band.nodes.size() || band.widths.size() != band.nodes.size()) {
            throw docraft::exception::LayoutConfigurationException(
                "table row band must have same number of nodes, lefts, widths");
        }
        return band.nodes.size();
    }

    float DocraftLayoutTableHandler::compute_row_height(const RowBand &band, const float row_top_y) {
        const std::size_t cells = effective_band_size(band);
        float resolved_row_height = std::max(0.0F, band.min_row_height);

        const float pad_x = get_cell_padding_x();
        const float pad_y = get_cell_padding_y();

        for (std::size_t c = 0; c < cells; ++c) {
            const auto &cell_node = band.nodes[c];
            if (!cell_node) {
                continue;
            }

            // Shrink inner_width and offset x by cell padding so text content is inset from the column borders.
            const float content_width = std::max(0.0F, band.widths[c] - (2.0F * pad_x));
            // content width is the column width minus horizontal padding on both sides
            const float content_x = band.lefts[c] + pad_x;
            // content x position is the left edge of the column plus horizontal padding
            compute_cell_layout(cell_node, content_width, content_x, row_top_y);
            // compute the layout of the cell content, which will update the cell_node's height based on its content

            // Add vertical padding to the measured content height so the row has breathing room.
            const float total_cell_height = cell_node->height() + (2.0F * pad_y);
            // total cell height is the content height plus vertical padding on top and bottom
            resolved_row_height = std::max(resolved_row_height, total_cell_height); // the resolved row height is
            //the maximum of the current resolved row height and the total cell height, ensuring that the row is tall
            //enough to fit its tallest cell content plus padding
        }

        return resolved_row_height;
    }

    void DocraftLayoutTableHandler::place_row_band(const RowBand &band,
                                                   const float row_top_y,
                                                   const float row_height) {
        const std::size_t cells = effective_band_size(band);
        for (std::size_t c = 0; c < cells; ++c) {
            const auto &cell_node = band.nodes[c];
            if (!cell_node) {
                continue;
            }
            // Align the cell content vertically within the row by centering it based on the computed row height
            center_text_in_row(cell_node, row_top_y, row_height, get_baseline_offset());
            cell_node->set_position({.x = band.lefts[c], .y = row_top_y}); //apply position
            cell_node->set_width(std::max(0.0F, band.widths[c])); //apply width
            cell_node->set_height(row_height); //apply height
        }
    }

    float DocraftLayoutTableHandler::layout_row_band(const RowBand &band, const float row_top_y) {
        const float row_height = compute_row_height(band, row_top_y);
        place_row_band(band, row_top_y, row_height);
        return row_height;
    }

    void DocraftLayoutTableHandler::clear_compute_state() {
        engine_.reset();
    }


    const std::shared_ptr<DocraftDocumentContext> &DocraftLayoutTableHandler::get_context() const {
        return context_;
    }

    DocraftLayoutEngine &DocraftLayoutTableHandler::get_engine() {
        if (!engine_.has_value()) {
            throw docraft::exception::InvalidInputException("Layout engine not initialized");
        }
        return engine_.value();
    }

    DocraftCursor &DocraftLayoutTableHandler::get_table_cursor() {
        return table_cursor_;
    }

    float DocraftLayoutTableHandler::get_cell_padding_x() const {
        return cell_padding_x_;
    }

    float DocraftLayoutTableHandler::get_cell_padding_y() const {
        return cell_padding_y_;
    }

    float DocraftLayoutTableHandler::get_baseline_offset() const {
        return baseline_offset_;
    }

    float DocraftLayoutTableHandler::get_fixed_x() const {
        return fixed_x_;
    }

    float DocraftLayoutTableHandler::get_fixed_y() const {
        return fixed_y_;
    }

    void DocraftLayoutTableHandler::set_fixed_y(float y) {
        fixed_y_ = y;
    }

    float DocraftLayoutTableHandler::get_saved_available_space() const {
        return saved_available_space_;
    }
} // namespace docraft::layout::handler

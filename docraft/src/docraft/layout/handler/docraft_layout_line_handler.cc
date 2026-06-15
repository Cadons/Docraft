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

#include "docraft/layout/handler/docraft_layout_line_handler.h"

#include <algorithm>
#include <cmath>

namespace docraft::layout::handler {
    void DocraftLayoutLineHandler::align_horizontal_row_lines_to_baseline(
        const std::vector<HorizontalChildPlacement> &placements,
        std::vector<model::DocraftTransform> &row_boxes) {
        if (placements.empty() || row_boxes.empty()) {
            return;
        }

        float row_bottom = row_boxes.front().anchors().bottom_left.y;
        for (const auto &box: row_boxes) {
            row_bottom = std::min(row_bottom, box.anchors().bottom_left.y);
        }

        for (const auto &placement: placements) {
            if (!placement.node || placement.box_index >= row_boxes.size()) {
                continue;
            }

            auto line = std::dynamic_pointer_cast<model::DocraftLine>(placement.node);
            if (!line || line->position_mode() != model::DocraftPositionType::kBlock) {
                continue;
            }

            auto &line_box = row_boxes[placement.box_index];
            const float stroke_width = std::max(1.0F, line->border_width());
            const float baseline_y = row_bottom + (stroke_width * 0.5F);

            // In block mode, painter uses origin + width; keep local segment flat at origin.y.
            line->set_start({.x = 0.0F, .y = 0.0F});
            line->set_end({.x = line->width(), .y = 0.0F});
            line->set_position({.x = line_box.position().x, .y = baseline_y});

            line_box.set_position({.x = line_box.position().x, .y = baseline_y});
            line_box.set_height(std::max(line_box.height(), stroke_width));
        }
    }

    void DocraftLayoutLineHandler::compute(const std::shared_ptr<model::DocraftLine> &node,
                                           model::DocraftTransform *box,
                                           DocraftCursor &cursor) {
        if (!box) {
            throw docraft::exception::InvalidInputException("result transform is null");
        }

        // Check if the line is positioned absolutely with explicit coordinates
        if (node->position_mode() == model::DocraftPositionType::kAbsolute) {
            // Absolute positioning: use the node's coordinates as-is
            return;
        }

        // Block mode: adapt line width to the current layout slot.
        // - If node width is set, keep it but clamp to the available slot width.
        // - Otherwise derive width from explicit points or fallback to slot width.
        const auto &layout_context = edit_context();
        const float allocated_width = std::max(0.0F, layout_context->layout().current_rect_width());
        const float intrinsic_width = std::fabs(node->end().x - node->start().x);
        float effective_width = node->width() > 0.0F ? node->width() : allocated_width;
        if (effective_width <= 0.0F) {
            effective_width = intrinsic_width;
        }
        if (allocated_width > 0.0F) {
            effective_width = std::min(effective_width, allocated_width);
        }

        box->set_width(effective_width);

        // Center the stroke in a thin line box while keeping the box top aligned with siblings.
        const float line_height = std::max(node->border_width(), 4.0F);
        const float mid_y = line_height / 2.0F;

        box->set_position({.x = cursor.x(), .y = cursor.y()});
        node->set_start({.x = 0.0F, .y = mid_y});
        node->set_end({.x = effective_width, .y = mid_y});
        // Provide minimal height so the box occupies space in flow
        box->set_height(line_height);
    }

    bool DocraftLayoutLineHandler::handle(const std::shared_ptr<model::DocraftNode> &request,
                                          model::DocraftTransform *result,
                                          DocraftCursor &cursor) {
        auto line_node = std::dynamic_pointer_cast<model::DocraftLine>(request);
        if (!line_node) {
            return false;
        }

        compute(line_node, result, cursor);
        // Note: cursor advancement is handled by finalize_layout in the layout engine,
        // which uses the box height we set in compute().
        return true;
    }
} // namespace docraft::layout::handler





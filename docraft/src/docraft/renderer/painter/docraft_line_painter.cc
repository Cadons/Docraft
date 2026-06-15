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

#include "docraft/renderer/painter/docraft_line_painter.h"

#include "docraft/backend/docraft_shape_rendering_backend.h"

namespace docraft::renderer::painter {
    DocraftLinePainter::DocraftLinePainter(const model::DocraftLine &line_node) : line_node_(line_node) {
    }

    void DocraftLinePainter::draw(const std::shared_ptr<DocraftDocumentContext> &context) {
        if (!context) return;
        auto &rendering_service = context->rendering();
        auto shape_backend = rendering_service.shape_rendering();
        auto line_backend = rendering_service.line_rendering();
        if (!shape_backend || !line_backend) return;

        const auto &stroke_color = line_node_.border_color().toRGB();
        const float stroke_width = line_node_.border_width();
        if (stroke_width <= 0.0F || stroke_color.a <= 0.0F) {
            return;
        }

        shape_backend->save_state();

        if (stroke_color.a < 1.0F) {
            shape_backend->set_stroke_alpha(stroke_color.a);
        }
        line_backend->set_line_width(stroke_width);
        line_backend->set_stroke_color(stroke_color.r, stroke_color.g, stroke_color.b);

        const auto &origin = line_node_.position();

        // If line has width in block mode: draw as horizontal line from origin.
        if (line_node_.width() > 0.0F && line_node_.position_mode() == model::DocraftPositionType::kBlock) {
            const float x1 = origin.x;
            const float y1 = origin.y;
            const float x2 = origin.x + line_node_.width();
            const float y2 = origin.y;
            line_backend->draw_line(x1, y1, x2, y2);
        } else {
            // Use explicit start/end points (absolute positioning).
            const auto &start = line_node_.start();
            const auto &end = line_node_.end();

            const float x1 = origin.x + start.x;
            const float y1 = origin.y + start.y;
            const float x2 = origin.x + end.x;
            const float y2 = origin.y + end.y;

            line_backend->draw_line(x1, y1, x2, y2);
        }

        shape_backend->restore_state();
    }
} // docraft::renderer::painter

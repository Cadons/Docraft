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

#include "docraft/renderer/painter/docraft_circle_painter.h"

#include <algorithm>

#include "docraft/backend/docraft_shape_rendering_backend.h"

namespace docraft::renderer::painter {
    DocraftCirclePainter::DocraftCirclePainter(const model::DocraftCircle &circle_node) : circle_node_(circle_node) {
    }

    void DocraftCirclePainter::draw(const std::shared_ptr<DocraftDocumentContext> &context) {
        if (!context) return;
        auto backend = context->shape_backend();
        if (!backend) return;

        const auto &bg_color = circle_node_.background_color().toRGB();
        const auto &border_color = circle_node_.border_color().toRGB();
        float border_width = circle_node_.border_width();

        if (bg_color.a <= 0.0F && (border_width <= 0.0F || border_color.a <= 0.0F)) {
            return;
        }

        const float radius = std::min(circle_node_.width(), circle_node_.height()) * 0.5F;
        if (radius <= 0.0F) {
            return;
        }

        backend->save_state();

        if (bg_color.a < 1.0F || border_color.a < 1.0F) {
            backend->set_fill_alpha(bg_color.a);
            backend->set_stroke_alpha(border_color.a);
        }

        if (border_width > 0.0F) {
            backend->set_line_width(border_width);
        }

        backend->set_fill_color(bg_color.r, bg_color.g, bg_color.b);
        backend->set_stroke_color(border_color.r, border_color.g, border_color.b);

        backend->draw_circle(circle_node_.center().x, circle_node_.center().y, radius);

        const bool has_fill = bg_color.a > 0.0F;
        const bool has_stroke = border_width > 0.0F && border_color.a > 0.0F;

        if (has_fill && has_stroke) {
            backend->fill_stroke();
        } else if (has_fill) {
            backend->fill();
        } else if (has_stroke) {
            backend->stroke();
        }

        backend->restore_state();
    }
} // docraft::renderer::painter

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

#include "docraft/renderer/painter/docraft_polygonal_shape_painter_base.h"

#include "docraft/backend/docraft_shape_rendering_backend.h"

namespace docraft::renderer::painter {
    void DocraftPolygonalShapePainterBase::draw(const std::shared_ptr<DocraftDocumentContext> &context) {
        if (!context) {
            return;
        }

        const auto &points = shape_points();
        const auto &origin = shape_origin();
        const auto &background_color = shape_background_color();
        const auto &border_color = shape_border_color();
        const float border_width = shape_border_width();

        const auto &rendering_service = context->rendering();
        auto shape_backend = rendering_service.shape_rendering();
        auto line_backend = rendering_service.line_rendering();
        if (!shape_backend || !line_backend) {
            return;
        }

        const auto bg_color = background_color.toRGB();
        const auto stroke_color = border_color.toRGB();
        if (bg_color.a <= 0.0F && (border_width <= 0.0F || stroke_color.a <= 0.0F)) {
            return;
        }
        if (points.size() < 3U) {
            return;
        }

        std::vector<model::DocraftPoint> transformed;
        transformed.reserve(points.size());
        for (const auto &pt: points) {
            transformed.push_back({.x = origin.x + pt.x, .y = origin.y - pt.y});
        }

        shape_backend->save_state();
        if (bg_color.a < 1.0F || stroke_color.a < 1.0F) {
            shape_backend->set_fill_alpha(bg_color.a);
            shape_backend->set_stroke_alpha(stroke_color.a);
        }
        if (border_width > 0.0F) {
            line_backend->set_line_width(border_width);
        }

        shape_backend->set_fill_color(bg_color.r, bg_color.g, bg_color.b);
        line_backend->set_stroke_color(stroke_color.r, stroke_color.g, stroke_color.b);
        shape_backend->draw_polygon(transformed);

        const bool has_fill = bg_color.a > 0.0F;
        const bool has_stroke = border_width > 0.0F && stroke_color.a > 0.0F;
        if (has_fill && has_stroke) {
            shape_backend->fill_stroke();
        } else if (has_fill) {
            shape_backend->fill();
        } else if (has_stroke) {
            shape_backend->stroke();
        }

        shape_backend->restore_state();
    }
} // namespace docraft::renderer::painter


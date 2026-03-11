#include "docraft/renderer/painter/docraft_polygon_painter.h"

#include "docraft/backend/docraft_shape_rendering_backend.h"

namespace docraft::renderer::painter {
    DocraftPolygonPainter::DocraftPolygonPainter(const model::DocraftPolygon &polygon_node) : polygon_node_(polygon_node) {
    }

    void DocraftPolygonPainter::draw(const std::shared_ptr<DocraftDocumentContext> &context) {
        if (!context) return;
        auto backend = context->shape_backend();
        if (!backend) return;

        const auto &bg_color = polygon_node_.background_color().toRGB();
        const auto &border_color = polygon_node_.border_color().toRGB();
        float border_width = polygon_node_.border_width();

        if (bg_color.a <= 0.0F && (border_width <= 0.0F || border_color.a <= 0.0F)) {
            return;
        }

        const auto &points = polygon_node_.points();
        if (points.size() < 3U) {
            return;
        }

        std::vector<model::DocraftPoint> transformed;
        transformed.reserve(points.size());
        const auto &origin = polygon_node_.position();
        for (const auto &pt : points) {
            transformed.push_back({.x = origin.x + pt.x, .y = origin.y - pt.y});
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

        backend->draw_polygon(transformed);

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

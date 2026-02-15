#include "renderer/painter/docraft_line_painter.h"

#include <backend/docraft_shape_rendering_backend.h>

namespace docraft::renderer::painter {
    DocraftLinePainter::DocraftLinePainter(const model::DocraftLine &line_node) : line_node_(line_node) {
    }

    void DocraftLinePainter::draw(const std::shared_ptr<DocraftDocumentContext> &context) {
        if (!context) return;
        auto backend = context->shape_backend();
        if (!backend) return;

        const auto &stroke_color = line_node_.border_color().toRGB();
        const float stroke_width = line_node_.border_width();
        if (stroke_width <= 0.0F || stroke_color.a <= 0.0F) {
            return;
        }

        backend->save_state();

        if (stroke_color.a < 1.0F) {
            backend->set_stroke_alpha(stroke_color.a);
        }
        backend->set_line_width(stroke_width);
        backend->set_stroke_color(stroke_color.r, stroke_color.g, stroke_color.b);

        const auto &start = line_node_.start();
        const auto &end = line_node_.end();
        const auto &origin = line_node_.position();

        const float x1 = origin.x + start.x;
        const float y1 = origin.y - start.y;
        const float x2 = origin.x + end.x;
        const float y2 = origin.y - end.y;

        backend->draw_line(x1, y1, x2, y2);

        backend->restore_state();
    }
} // docraft::renderer::painter

#include "docraft/renderer/painter/docraft_rectangle_painter.h"

#include "docraft/backend/docraft_shape_rendering_backend.h"

namespace docraft::renderer::painter {
    DocraftRectanglePainter::DocraftRectanglePainter(const model::DocraftRectangle &rectangle_node) : rectangle_node_(
        rectangle_node) {
    }

    void DocraftRectanglePainter::draw(const std::shared_ptr<DocraftDocumentContext> &context) {
        // Validate context and handles early to avoid invalid-document errors
        if (!context) return;
        auto backend = context->shape_backend();
        if (!backend) return;

        // const auto& box = rectangle_node_.transform_box();
        const auto& bg_color = rectangle_node_.background_color().toRGB();
        const auto& border_color = rectangle_node_.border_color().toRGB();
        float border_width = rectangle_node_.border_width();

        // If nothing is visible, just return
        if (bg_color.a <= 0.0F && (border_width <= 0.0F || border_color.a <= 0.0F)) {
            return;
        }

        // Save graphics state to isolate painter changes
        backend->save_state();

        // 1. SET GRAPHICS STATE FIRST (Alpha, Line Width)
        // These MUST be set before starting a path (rectangle starts a path)
        if (bg_color.a < 1.0F || border_color.a < 1.0F) {
            backend->set_fill_alpha(bg_color.a);
            backend->set_stroke_alpha(border_color.a);
        }

        if (border_width > 0.0F) {
            backend->set_line_width(border_width);
        }

        // 2. SET COLORS
        backend->set_fill_color(bg_color.r, bg_color.g, bg_color.b);
        backend->set_stroke_color(border_color.r, border_color.g, border_color.b);

        // 3. DEFINE AND EXECUTE PATH
        backend->draw_rectangle(
            rectangle_node_.anchors().bottom_left.x,
            rectangle_node_.anchors().bottom_left.y,
            rectangle_node_.width(),
            rectangle_node_.height());

        // Determine drawing operation
        bool has_fill = bg_color.a > 0.0F;
        bool has_stroke = border_width > 0.0F && border_color.a > 0.0F;

        if (has_fill && has_stroke) {
            backend->fill_stroke();
        } else if (has_fill) {
            backend->fill();
        } else if (has_stroke) {
            backend->stroke();
        }

        // Restore graphics state
        backend->restore_state();
    }
} // docraft

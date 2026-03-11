#include "docraft/renderer/docraft_pdf_renderer.h"

#include <algorithm>

#include "docraft/renderer/painter/docraft_image_painter.h"
#include "docraft/docraft_color.h"
#include "docraft/renderer/painter/docraft_blank_line_painter.h"
#include "docraft/renderer/painter/docraft_circle_painter.h"
#include "docraft/renderer/painter/docraft_line_painter.h"
#include "docraft/renderer/painter/docraft_polygon_painter.h"
#include "docraft/renderer/painter/docraft_rectangle_painter.h"
#include "docraft/renderer/painter/docraft_table_painter.h"
#include "docraft/renderer/painter/docraft_text_painter.h"
#include "docraft/renderer/painter/docraft_triangle_painter.h"

namespace docraft::renderer {
    void DocraftPDFRenderer::render_text(const model::DocraftText &text_node) {
        painter::DocraftTextPainter painter(text_node);
        painter.draw(context());
    }
    void DocraftPDFRenderer::render_section(const model::DocraftSection &section_node) {
        auto backend = context()->shape_backend();
        if (!backend) return;

        const float left = section_node.position().x;
        const float right = section_node.position().x + section_node.width();
        const float top = section_node.position().y;
        const float bottom = section_node.position().y - section_node.height();

        const float content_width = std::max(0.0F, right - left);
        const float content_height = std::max(0.0F, top - bottom);

        const auto &bg_color = section_node.background_color().toRGB();
        const auto &border_color = section_node.border_color().toRGB();
        float border_width = section_node.border_width();

        const bool has_fill = bg_color.a > 0.0F;
        bool has_stroke = border_width > 0.0F && border_color.a > 0.0F;

        if (content_width > 0.0F && content_height > 0.0F && (has_fill || has_stroke)) {
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

            backend->draw_rectangle(left, bottom, content_width, content_height);

            if (has_fill && has_stroke) {
                backend->fill_stroke();
            } else if (has_fill) {
                backend->fill();
            } else if (has_stroke) {
                backend->stroke();
            }

            backend->restore_state();
        }

        const auto &stroke_color = section_node.border_color().toRGB();
        const float stroke_width = section_node.border_width();
        const bool has_margin_stroke = stroke_width > 0.0F && stroke_color.a > 0.0F;
        if (has_margin_stroke) {
            backend->save_state();
            if (stroke_color.a < 1.0F) {
                backend->set_stroke_alpha(stroke_color.a);
            }
            backend->set_line_width(stroke_width);
            backend->set_stroke_color(stroke_color.r, stroke_color.g, stroke_color.b);

            if (section_node.margin_left() > 0.0F) {
                backend->draw_line(left, top, left, bottom);
            }
            if (section_node.margin_right() > 0.0F) {
                backend->draw_line(right, top, right, bottom);
            }
            if (section_node.margin_top() > 0.0F) {
                backend->draw_line(left, top, right, top);
            }
            if (section_node.margin_bottom() > 0.0F) {
                backend->draw_line(left, bottom, right, bottom);
            }

            backend->restore_state();
        }
    }
    void DocraftPDFRenderer::render_image(const model::DocraftImage &image_node) {
        painter::DocraftImagePainter painter(image_node);
        painter.draw(context());
    }

    void DocraftPDFRenderer::render_table(const model::DocraftTable &table_node) {
        painter::DocraftTablePainter painter(table_node);
        painter.draw(context());
    }

    void DocraftPDFRenderer::render_rectangle(const model::DocraftRectangle &rectangle_node) {
        painter::DocraftRectanglePainter painter(rectangle_node);
        painter.draw(context());
    }
    void DocraftPDFRenderer::render_circle(const model::DocraftCircle &circle_node) {
        painter::DocraftCirclePainter painter(circle_node);
        painter.draw(context());
    }
    void DocraftPDFRenderer::render_triangle(const model::DocraftTriangle &triangle_node) {
        painter::DocraftTrianglePainter painter(triangle_node);
        painter.draw(context());
    }
    void DocraftPDFRenderer::render_line(const model::DocraftLine &line_node) {
        painter::DocraftLinePainter painter(line_node);
        painter.draw(context());
    }
    void DocraftPDFRenderer::render_polygon(const model::DocraftPolygon &polygon_node) {
        painter::DocraftPolygonPainter painter(polygon_node);
        painter.draw(context());
    }
    void DocraftPDFRenderer::render_blank_line(const model::DocraftBlankLine &blank_line_node) {
        painter::docraft_blank_line_painter painter(blank_line_node);
        painter.draw(context());
    }
} // docraft

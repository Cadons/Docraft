#include "renderer/docraft_pdf_renderer.h"

#include <hpdf.h>

#include "../../include/renderer/painter/docraft_image_painter.h"
#include "renderer/painter/docraft_blank_line_painter.h"
#include "renderer/painter/docraft_rectangle_painter.h"
#include "renderer/painter/docraft_table_painter.h"
#include "renderer/painter/docraft_text_painter.h"

namespace docraft::renderer {
    void DocraftPDFRenderer::render_text(const model::DocraftText &text_node) {
        painter::DocraftTextPainter painter(text_node);
        painter.draw(context());
    }
    void DocraftPDFRenderer::render_section(const model::DocraftSection &section_node) {
        // Currently, sections do not have specific rendering logic.
        // This method can be expanded in the future if needed.
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
    void DocraftPDFRenderer::draw_line(HPDF_Page page, model::DocraftPoint start, model::DocraftPoint end, DocraftColor color) {
        const auto rgb=color.toRGB();
        HPDF_Page_SetRGBStroke(page, rgb.r, rgb.g, rgb.b);
        HPDF_Page_MoveTo(page, start.x, start.y);
        HPDF_Page_LineTo(page, end.x, end.y);
        HPDF_Page_Stroke(page);
    }

    void DocraftPDFRenderer::render_blank_line(const model::DocraftBlankLine &blank_line_node) {
        painter::docraft_blank_line_painter painter(blank_line_node);
        painter.draw(context());
    }
} // docraft

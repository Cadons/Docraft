#include "renderer/painter/docraft_rectangle_painter.h"

#include <hpdf.h>

#include "renderer/docraft_pdf_renderer.h"

namespace docraft::renderer::painter {
    DocraftRectanglePainter::DocraftRectanglePainter(const model::DocraftRectangle &rectangle_node) : rectangle_node_(
        rectangle_node) {
    }

    void DocraftRectanglePainter::draw(const std::shared_ptr<DocraftPDFContext> &context) {
        // Validate context and handles early to avoid invalid-document errors
        if (!context) return;
        auto *page = context->page();
        auto *doc = context->pdf_doc();
        if (!page || !doc) return;

        // const auto& box = rectangle_node_.transform_box();
        const auto& bg_color = rectangle_node_.background_color().toRGB();
        const auto& border_color = rectangle_node_.border_color().toRGB();
        float border_width = rectangle_node_.border_width();

        // If nothing is visible, just return
        if (bg_color.a <= 0.0F && (border_width <= 0.0F || border_color.a <= 0.0F)) {
            return;
        }

        // Save graphics state to isolate painter changes
        HPDF_Page_GSave(page);

        // 1. SET GRAPHICS STATE FIRST (Alpha, Line Width)
        // These MUST be set before starting a path (HPDF_Page_Rectangle starts a path)
        if (bg_color.a < 1.0F || border_color.a < 1.0F) {
            auto *ext = HPDF_CreateExtGState(doc);
            if (ext) {
                HPDF_ExtGState_SetAlphaFill(ext, bg_color.a);
                HPDF_ExtGState_SetAlphaStroke(ext, border_color.a);
                HPDF_Page_SetExtGState(page, ext);
            }
        }

        if (border_width > 0.0F) {
            HPDF_Page_SetLineWidth(page, border_width);
        }

        // 2. SET COLORS
        HPDF_Page_SetRGBFill(page, bg_color.r, bg_color.g, bg_color.b);
        HPDF_Page_SetRGBStroke(page, border_color.r, border_color.g, border_color.b);

        // 3. DEFINE AND EXECUTE PATH
        HPDF_Page_Rectangle(page,rectangle_node_.anchors().bottom_left.x, rectangle_node_.anchors().bottom_left.y,
                            rectangle_node_.width(), rectangle_node_.height());

        // Determine drawing operation
        bool has_fill = bg_color.a > 0.0F;
        bool has_stroke = border_width > 0.0F && border_color.a > 0.0F;

        if (has_fill && has_stroke) {
            HPDF_Page_FillStroke(page);
        } else if (has_fill) {
            HPDF_Page_Fill(page);
        } else if (has_stroke) {
            HPDF_Page_Stroke(page);
        }

        // Restore graphics state
        HPDF_Page_GRestore(page);
    }
} // docraft

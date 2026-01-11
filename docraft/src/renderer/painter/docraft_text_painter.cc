#include "renderer/painter/docraft_text_painter.h"

#include <algorithm>
#include <hpdf.h>
#include <print>

#include "generic/docraft_font_applier.h"
#define WORD_SPACE_MAX 300
#define WORD_SPACE_MIN -30

namespace docraft::renderer::painter {
    DocraftTextPainter::DocraftTextPainter(const model::DocraftText &text_node) : text_node_(text_node) {
    }


    void DocraftTextPainter::render_justified(const std::shared_ptr<DocraftPDFContext> &context,
                                              const std::string &text) {
        auto page = context->page();
        auto &cursor = context->cursor();

        float max_width = context->current_rect_width() - (2 * cursor.offset_x());
        float actual_width = HPDF_Page_TextWidth(page, text.c_str());

        size_t spaces = std::count(text.begin(), text.end(), ' ');

        if (spaces > 0) {
            float extra_space = (max_width - actual_width) / static_cast<float>(spaces);
            if (extra_space <= WORD_SPACE_MAX && extra_space >= WORD_SPACE_MIN) {
                HPDF_Page_SetWordSpace(page, extra_space); //this set the space between words
            }else {
                if (extra_space > WORD_SPACE_MAX) {
                    HPDF_Page_SetWordSpace(page, WORD_SPACE_MAX);
                } else {
                    HPDF_Page_SetWordSpace(page, WORD_SPACE_MIN);
                }
            }
        }


        HPDF_Page_TextOut(page, current_line_->x(), current_line_->y(), text.c_str());

        HPDF_Page_SetWordSpace(page, 0);
    }

    std::pair<std::pair<float, float>, std::pair<float, float> > DocraftTextPainter::render_text(
        const std::shared_ptr<DocraftPDFContext> &context, const std::string &text) {
        auto page = context->page();

        //begin drawing
        HPDF_Page_BeginText(page);
        if (current_line_->alignment() == model::TextAlignment::kJustified) {
            render_justified(context, text);
        } else {
            HPDF_Page_TextOut(page, current_line_->x(), current_line_->y(), text.c_str());
        }
        HPDF_Page_EndText(page);
        return std::make_pair(std::make_pair(current_line_->x(), current_line_->y()),
                              std::make_pair(current_line_->x() + HPDF_Page_TextWidth(page, text.c_str()),
                                             current_line_->y() + current_line_->font_size()));
    }

    std::pair<std::pair<float, float>, std::pair<float, float> > DocraftTextPainter::draw_text(
        const std::shared_ptr<DocraftPDFContext> &context, const std::string &text) {
        return render_text(context, text);
    }

    void DocraftTextPainter::
    draw_underline(const std::shared_ptr<DocraftPDFContext> &context, const std::string &text) {
        //TODO: improve text underline for alignment different from Left
        auto &cursor = context->cursor();
        auto *page = context->page();

        // 1. Draw the text normally first
        auto result = draw_text(context, text);

        // Place the line slightly below the baseline (y)
        float underline_top = result.first.second - (current_line_->font_size() * 0.15F);
        float thickness = current_line_->font_size() * 0.07F;

        // 3. Draw the line
        auto rgb = current_line_->color().toRGB();
        HPDF_Page_SetRGBStroke(page, rgb.r, rgb.g, rgb.b);
        HPDF_Page_SetLineWidth(page, thickness);

        HPDF_Page_MoveTo(page, result.first.first, underline_top);
        HPDF_Page_LineTo(page, result.second.first, underline_top);
        HPDF_Page_Stroke(page);
    }

    void DocraftTextPainter::draw(const std::shared_ptr<DocraftPDFContext> &context) {
        for (const auto &line: text_node_.lines()) {
            current_line_ = line;
             context->font_applier()->apply_font(current_line_);
            if (line->underline()) {
                draw_underline(context, line->text());
            } else {
                draw_text(context, line->text());
            }
        }
        current_line_ = nullptr;
    }
} // docraft

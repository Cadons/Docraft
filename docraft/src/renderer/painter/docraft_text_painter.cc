#include "renderer/painter/docraft_text_painter.h"

#include <algorithm>
#include <hpdf.h>
#include <print>
#include <sstream>

#include "generic/docraft_font_applier.h"
#define WORD_SPACE_MAX 300
#define WORD_SPACE_MIN -30

namespace docraft::renderer::painter {
    DocraftTextPainter::DocraftTextPainter(const model::DocraftText &text_node) : text_node_(text_node) {
    }


    void DocraftTextPainter::render_justified(const std::shared_ptr<DocraftDocumentContext> &context,
                                              const std::string &text) {
        auto page = context->page();

        // Use the computed line width as the target for justification.
        float max_width = current_line_->width();
        float actual_width = HPDF_Page_TextWidth(page, text.c_str());

        size_t spaces = std::count(text.begin(), text.end(), ' ');

        if (spaces == 0 || max_width <= actual_width) {
            // Fallback: no extra spacing to distribute.
            HPDF_Page_SetTextMatrix(page, 1, 0, 0, 1, current_line_->position().x, current_line_->position().y);
            HPDF_Page_ShowText(page, text.c_str());
            return;
        }

        // Manually distribute extra space between words for visible justification.
        const float extra_space = (max_width - actual_width) / static_cast<float>(spaces);
        const float space_width = HPDF_Page_TextWidth(page, " ");
        float x = current_line_->position().x;
        const float y = current_line_->position().y;

        std::istringstream iss(text);
        std::string word;
        bool first = true;
        while (iss >> word) {
            if (!first) {
                x += space_width + extra_space;
            }
            HPDF_Page_SetTextMatrix(page, 1, 0, 0, 1, x, y);
            HPDF_Page_ShowText(page, word.c_str());
            x += HPDF_Page_TextWidth(page, word.c_str());
            first = false;
        }
    }

    std::pair<std::pair<float, float>, std::pair<float, float> > DocraftTextPainter::render_text(
        const std::shared_ptr<DocraftDocumentContext> &context, const std::string &text) {
        auto page = context->page();

        //begin drawing
        HPDF_Page_BeginText(page);
        if (current_line_->alignment() == model::TextAlignment::kJustified) {
            render_justified(context, text);
        } else {
            HPDF_Page_TextOut(page, current_line_->position().x, current_line_->position().y, text.c_str());
        }
        HPDF_Page_EndText(page);
        return std::make_pair(std::make_pair(current_line_->position().x, current_line_->position().y),
                              std::make_pair(current_line_->position().y + HPDF_Page_TextWidth(page, text.c_str()),
                                             current_line_->position().y + current_line_->font_size()));
    }

    std::pair<std::pair<float, float>, std::pair<float, float> > DocraftTextPainter::draw_text(
        const std::shared_ptr<DocraftDocumentContext> &context, const std::string &text) {
        return render_text(context, text);
    }

    void DocraftTextPainter::
    draw_underline(const std::shared_ptr<DocraftDocumentContext> &context, const std::string &text) {
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

    void DocraftTextPainter::draw(const std::shared_ptr<DocraftDocumentContext> &context) {
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

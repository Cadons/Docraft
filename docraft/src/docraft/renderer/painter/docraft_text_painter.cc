#include "docraft/renderer/painter/docraft_text_painter.h"

#include <algorithm>
#include <print>
#include <sstream>

#include "docraft/generic/docraft_font_applier.h"
#define WORD_SPACE_MAX 300
#define WORD_SPACE_MIN -30

namespace docraft::renderer::painter {
    DocraftTextPainter::DocraftTextPainter(const model::DocraftText &text_node) : text_node_(text_node) {
    }


    void DocraftTextPainter::render_justified(const std::shared_ptr<DocraftDocumentContext> &context,
                                              const std::string &text) {
        auto backend = context->text_backend();

        // Use the computed line width as the target for justification.
        float max_width = current_line_->width();
        float actual_width = backend->measure_text_width(text);

        size_t spaces = std::count(text.begin(), text.end(), ' ');

        if (spaces == 0 || max_width <= actual_width) {
            // Fallback: no extra spacing to distribute.
            backend->draw_text_matrix(text, 1, 0, 0, 1, current_line_->position().x, current_line_->position().y);
            return;
        }

        // Manually distribute extra space between words for visible justification.
        const float extra_space = (max_width - actual_width) / static_cast<float>(spaces);
        const float space_width = backend->measure_text_width(" ");
        float x = current_line_->position().x;
        const float y = current_line_->position().y;

        std::istringstream iss(text);
        std::string word;
        bool first = true;
        while (iss >> word) {
            if (!first) {
                x += space_width + extra_space;
            }
            backend->draw_text_matrix(word, 1, 0, 0, 1, x, y);
            x += backend->measure_text_width(word);
            first = false;
        }
    }

    std::pair<std::pair<float, float>, std::pair<float, float> > DocraftTextPainter::render_text(
        const std::shared_ptr<DocraftDocumentContext> &context, const std::string &text) {
        auto backend = context->text_backend();

        //begin drawing
        backend->begin_text();
        if (current_line_->alignment() == model::TextAlignment::kJustified) {
            render_justified(context, text);
        } else {
            backend->draw_text(text, current_line_->position().x, current_line_->position().y);
        }
        backend->end_text();
        const float actual_width = backend->measure_text_width(text);
        float rendered_width = actual_width;
        if (current_line_->alignment() == model::TextAlignment::kJustified) {
            const size_t spaces = std::count(text.begin(), text.end(), ' ');
            if (spaces > 0 && current_line_->width() > actual_width) {
                rendered_width = current_line_->width();
            }
        }
        return std::make_pair(std::make_pair(current_line_->position().x, current_line_->position().y),
                              std::make_pair(current_line_->position().x + rendered_width,
                                             current_line_->position().y + current_line_->font_size()));
    }

    std::pair<std::pair<float, float>, std::pair<float, float> > DocraftTextPainter::draw_text(
        const std::shared_ptr<DocraftDocumentContext> &context, const std::string &text) {
        return render_text(context, text);
    }

    void DocraftTextPainter::
    draw_underline(const std::shared_ptr<DocraftDocumentContext> &context, const std::string &text) {
        auto backend = context->text_backend();

        // 1. Draw the text normally first
        auto result = draw_text(context, text);

        // Place the line slightly below the baseline (y)
        float underline_top = result.first.second - (current_line_->font_size() * 0.15F);
        float thickness = current_line_->font_size() * 0.07F;

        // 3. Draw the line
        auto rgb = current_line_->color().toRGB();
        backend->set_stroke_color(rgb.r, rgb.g, rgb.b);
        backend->set_line_width(thickness);
        backend->draw_line(result.first.first, underline_top, result.second.first, underline_top);
    }

    void DocraftTextPainter::draw(const std::shared_ptr<DocraftDocumentContext> &context) {
        for (const auto &line: text_node_.lines()) {
            current_line_ = line;
             context->font_applier()->apply_font(current_line_);
            auto rgb = current_line_->color().toRGB();
            context->text_backend()->set_text_color(rgb.r, rgb.g, rgb.b);
            if (line->underline()) {
                draw_underline(context, line->text());
            } else {
                draw_text(context, line->text());
            }
        }
        current_line_ = nullptr;
    }
} // docraft

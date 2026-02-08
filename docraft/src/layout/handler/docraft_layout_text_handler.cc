#include "layout/handler/docraft_layout_text_handler.h"

#include <algorithm>
#include <hpdf.h>
#include <print>

#include "generic/docraft_font_applier.h"
#include "model/docraft_layout.h"
#include "utils/docraft_logger.h"


namespace docraft::layout::handler {
    void DocraftLayoutTextHandler::filter_text(const std::shared_ptr<model::DocraftText>& node) {
        // Remove new line characters
        std::string filtered_text;
        for (char c: node->text()) {
            if (c != '\n' && c != '\r') {
                filtered_text += c;
            }
        }
        node->set_text(filtered_text);
    }

    float DocraftLayoutTextHandler::measure_text_width(const std::shared_ptr<model::DocraftText> &node) const {
        auto *page = context()->page();
        if (!page) {
            throw std::runtime_error("PDF page is not initialized in context");
        }
        generic::DocraftFontApplier font_applier(context());
        HPDF_Font font = HPDF_GetFont(context()->pdf_doc(),font_applier.get_font_registred_name(node->font_name()) , nullptr);
        HPDF_Page_SetFontAndSize(page, font, node->font_size());
        return HPDF_Page_TextWidth(page, node->text().c_str());
    }

    float DocraftLayoutTextHandler::measure_test_width(const std::string &text) const {
        auto *page = context()->page();
        if (!page) {
            throw std::runtime_error("PDF page is not initialized in context");
        }
        return HPDF_Page_TextWidth(page, text.c_str());
    }

    void DocraftLayoutTextHandler::compute(const std::shared_ptr<model::DocraftText> &node,
                                           model::DocraftTransform* box,
                                           DocraftCursor& cursor) {
        filter_text(node);
        generic::DocraftFontApplier font_applier(context());
        font_applier.apply_font(node);
        auto global_cursor = cursor;
        node->set_position({.x=global_cursor.x(), .y=global_cursor.y()});
        DocraftCursor text_cursor = cursor;//cursor for the text box, start from the current global cursor


        //Divide text into lines if necessary
        for (size_t start = 0; start < node->text().length();)
        {
            //get the maximum string that fits in the current width
            size_t end = node->text().find('\n', start);
            if (end == std::string::npos) {
                end = node->text().length();
            }
            std::string line = node->text().substr(start, end - start);
            float line_width = measure_test_width(line);
            if (line_width > context()->available_space()) {
                // If the line exceeds the width, break it into smaller parts
                size_t line_start = 0;
                while (line_start < line.length()) {
                    size_t probe_end = line_start +1;//start probing from the next character
                    size_t last_fit_end = line_start;

                    while (probe_end <= line.length()) {
                        std::string sub_line = line.substr(line_start, probe_end - line_start);
                        float sub_line_width = measure_test_width(sub_line);

                        if (sub_line_width > context()->available_space()) {
                            break;
                        }
                        last_fit_end = probe_end;
                        ++probe_end;
                    }
                    // If nothing fits (even 1 char), force progress by consuming 1 char.
                    if (last_fit_end == line_start) {
                        last_fit_end = std::min(line_start + 1, line.length());
                    }

                    node->add_line(std::make_shared<model::DocraftText>(
                        line.substr(line_start, last_fit_end - line_start)
                    ));
                    line_start = last_fit_end;

                }
            } else {
                node->add_line(std::make_shared<model::DocraftText>(line));
            }
            start = end + 1;
        }

        //Compute position for each line
 float total_height = 0.0F;
        float total_width = 0.0F;

        const float line_height = node->font_size() * interline_space_;

        // Always move to the first baseline below the current cursor Y,
        // but clamp so the first line doesn't get clipped above the page.
        const float kTopSafe = context()->page_height() - line_height;
        float first_baseline_y = text_cursor.y() - line_height;
        if (first_baseline_y > kTopSafe) {
            first_baseline_y = kTopSafe;
        }
        text_cursor.move_to(text_cursor.x(), first_baseline_y);

        const auto lines = node->lines();
        for (std::size_t i = 0; i < lines.size(); ++i) {
            const auto& line = lines[i];
            line->set_font_name(node->font_name());
            line->set_font_size(node->font_size());

            float line_width = measure_text_width(line);
            const bool is_last_line = (i + 1 == lines.size());
            if (node->alignment() == model::TextAlignment::kJustified) {
                if (is_last_line) {
                    line->set_alignment(model::TextAlignment::kLeft);
                    line->set_width(line_width);
                } else {
                    line->set_alignment(model::TextAlignment::kJustified);
                    line->set_width(context()->available_space());
                }
            } else {
                line->set_alignment(node->alignment());
                line->set_width(line_width);
            }
            line->set_height(line_height);

            switch (line->alignment()) {
                case model::TextAlignment::kLeft:
                    line->set_position({.x = text_cursor.x(), .y = text_cursor.y()});
                    break;
                case model::TextAlignment::kCenter:
                    line->set_position({.x = text_cursor.x() + ((context()->available_space() - line_width) / 2), .y = text_cursor.y()});
                    break;
                case model::TextAlignment::kRight:
                    line->set_position({.x = text_cursor.x() + (context()->available_space() - line_width), .y = text_cursor.y()});
                    break;
                case model::TextAlignment::kJustified:
                    line->set_position({.x = text_cursor.x(), .y = text_cursor.y()});
                    break;
            }

            total_height += line->height();
            total_width = std::max(total_width, line->width());

            text_cursor.move_to(text_cursor.x(), std::max(text_cursor.y() - line->height(),line->height()));
        }

        node->set_position({.x = global_cursor.x(), .y = global_cursor.y()});
        node->set_height(total_height);
        node->set_width(total_width);

        if (box) {
            box->set_position({.x = node->position().x, .y = node->position().y});
            box->set_width(node->width());
            box->set_height(node->height());
        }

        LOG_DEBUG(node->text() + ":" + node->to_string());
    }

    bool DocraftLayoutTextHandler::handle(const std::shared_ptr<model::DocraftNode> &request,
                                          model::DocraftTransform * result,
                                          DocraftCursor& cursor) {
        if (auto text_node = std::dynamic_pointer_cast<model::DocraftText>(request)) {
            compute(text_node,result, cursor);
            return true;
        }
        return false;
    }
} // docraft

#include "docraft/layout/handler/docraft_layout_text_handler.h"

#include <algorithm>
#include <sstream>
#include <print>

#include "docraft/generic/docraft_font_applier.h"
#include "docraft/model/docraft_layout.h"
#include "docraft/model/docraft_page_number.h"
#include "docraft/utils/docraft_logger.h"


namespace docraft::layout::handler {
    void DocraftLayoutTextHandler::filter_text(const std::shared_ptr<model::DocraftText>& node) {
        // Remove carriage returns, keep newlines for paragraph splitting.
        std::string filtered_text;
        for (char c: node->text()) {
            if (c != '\r') {
                filtered_text += c;
            }
        }
        node->set_text(filtered_text);
    }

    float DocraftLayoutTextHandler::measure_text_width(const std::shared_ptr<model::DocraftText> &node) const {
        generic::DocraftFontApplier font_applier(context());
        font_applier.apply_font(node);
        return context()->text_backend()->measure_text_width(node->text());
    }

    float DocraftLayoutTextHandler::measure_test_width(const std::string &text) const {
        return context()->text_backend()->measure_text_width(text);
    }

    void DocraftLayoutTextHandler::compute(const std::shared_ptr<model::DocraftText> &node,
                                           model::DocraftTransform* box,
                                           DocraftCursor& cursor) {
        filter_text(node);
        generic::DocraftFontApplier font_applier(context());
        font_applier.apply_font(node);
        auto global_cursor = cursor;

        DocraftCursor text_cursor = cursor;//cursor for the text box, start from the current global cursor
        if (node->position_mode() == model::DocraftPositionType::kBlock) {
            node->set_position({.x=global_cursor.x(), .y=global_cursor.y()});
        }
        else {
            node->set_position({.x=node->position().x, .y=node->position().y});
            text_cursor.move_to(node->position().x, node->position().y);
        }

        node->clear_lines(); // Recompute wrapping from scratch to avoid duplicate lines.

        const float padding = std::max(0.0F, node->padding());
        const float available_width = std::max(0.0F, context()->available_space() - (2.0F * padding));
        auto add_wrapped_word = [&](const std::string& word) {
            if (word.empty()) {
                return;
            }
            if (measure_test_width(word) <= available_width) {
                node->add_line(std::make_shared<model::DocraftText>(word));
                return;
            }
            // If a single word exceeds available width, split by characters.
            size_t line_start = 0;
            while (line_start < word.length()) {
                size_t probe_end = line_start + 1;
                size_t last_fit_end = line_start;
                while (probe_end <= word.length()) {
                    std::string sub_line = word.substr(line_start, probe_end - line_start);
                    float sub_line_width = measure_test_width(sub_line);
                    if (sub_line_width > available_width) {
                        break;
                    }
                    last_fit_end = probe_end;
                    ++probe_end;
                }
                if (last_fit_end == line_start) {
                    last_fit_end = std::min(line_start + 1, word.length());
                }
                node->add_line(std::make_shared<model::DocraftText>(
                    word.substr(line_start, last_fit_end - line_start)
                ));
                line_start = last_fit_end;
            }
        };

        auto wrap_paragraph = [&](const std::string& paragraph) {
            std::istringstream iss(paragraph);
            std::string word;
            std::string current_line;
            while (iss >> word) {
                if (current_line.empty()) {
                    if (measure_test_width(word) <= available_width) {
                        current_line = word;
                    } else {
                        add_wrapped_word(word);
                    }
                    continue;
                }
                std::string candidate = current_line + " " + word;
                if (measure_test_width(candidate) <= available_width) {
                    current_line = candidate;
                } else {
                    node->add_line(std::make_shared<model::DocraftText>(current_line));
                    current_line.clear();
                    if (measure_test_width(word) <= available_width) {
                        current_line = word;
                    } else {
                        add_wrapped_word(word);
                    }
                }
            }
            if (!current_line.empty()) {
                node->add_line(std::make_shared<model::DocraftText>(current_line));
            }
        };

        // Split on explicit newlines, then wrap each paragraph by words.
        for (size_t start = 0; start < node->text().length();) {
            size_t end = node->text().find('\n', start);
            if (end == std::string::npos) {
                end = node->text().length();
            }
            wrap_paragraph(node->text().substr(start, end - start));
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
            line->set_color(node->color());
            line->set_style(node->style());
            line->set_underline(node->underline());

            float line_width = measure_text_width(line);
            const bool is_last_line = (i + 1 == lines.size());
            if (node->alignment() == model::TextAlignment::kJustified) {
                if (is_last_line) {
                    line->set_alignment(model::TextAlignment::kLeft);
                    line->set_width(line_width);
                } else {
                    // Stretch non-final lines to the full available width for visible justification.
                    line->set_alignment(model::TextAlignment::kJustified);
                    line->set_width(available_width);
                }
            } else {
                line->set_alignment(node->alignment());
                line->set_width(line_width);
            }
            line->set_height(line_height);

            const float base_x = text_cursor.x() + padding;
            switch (line->alignment()) {
                case model::TextAlignment::kLeft:
                    line->set_position({.x = base_x, .y = text_cursor.y()});
                    break;
                case model::TextAlignment::kCenter:
                    line->set_position({.x = base_x + ((available_width - line_width) / 2), .y = text_cursor.y()});
                    break;
                case model::TextAlignment::kRight:
                    line->set_position({.x = base_x + (available_width - line_width), .y = text_cursor.y()});
                    break;
                case model::TextAlignment::kJustified:
                    line->set_position({.x = base_x, .y = text_cursor.y()});
                    break;
            }

            total_height += line->height();
            total_width = std::max(total_width, line->width());

            text_cursor.move_to(text_cursor.x(), std::max(text_cursor.y() - line->height(), 0.0F));
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

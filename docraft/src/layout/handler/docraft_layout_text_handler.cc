#include "layout/handler/docraft_layout_text_handler.h"

#include <hpdf.h>
#include <print>

#include "generic/docraft_font_applier.h"
#include "model/docraft_layout.h"


namespace docraft::layout::handler {


    void DocraftLayoutTextHandler::filter_text(std::shared_ptr<model::DocraftText> node) {
        // Remove new line characters
        std::string filtered_text;
        for (char c: node->text()) {
            if (c != '\n' && c != '\r') {
                filtered_text += c;
            }
        }
        node->set_text(filtered_text);
    }

    void DocraftLayoutTextHandler::compute(const std::shared_ptr<model::DocraftText>& node) {
        auto &cursor = context()->cursor();
        auto *page = context()->page();
        auto *pdf = context()->pdf_doc();

        filter_text(node);
        const float max_width = context()->current_rect_width();
        const float line_height = node->font_size() * 1.2F; // line height factor
        std::string remaining_text = node->text();
         context()->font_applier()->apply_font(node);
        float total_height = 0.0F;
        float total_width = 0.0F;
        while (!remaining_text.empty()) {
            // compute how much space is left in the current line
            float available_width = max_width;

            if (available_width <= node->font_size() * 0.5F) {
                // if no space create a new line
                cursor.reset_x();
                cursor.move_to(cursor.x(), cursor.y() - line_height);
                available_width = max_width;
            }
            //Measure how many letters can be placed in that space
            // HPDF_TRUE enable word breaking
            HPDF_UINT measured_chars = HPDF_Page_MeasureText(page, remaining_text.c_str(),
                                                             available_width - cursor.offset_x(), HPDF_TRUE,
                                                             nullptr);


            std::string current_line = remaining_text.substr(0, measured_chars);
            if (measured_chars==0) {
                break;
            }
            float position_x = cursor.x();

            switch (node->alignment()) {
                case model::TextAlignment::kCenter: {
                    float actual_width = HPDF_Page_TextWidth(page, current_line.c_str()) + cursor.offset_x();
                    float centered_x = cursor.offset_x() + ((max_width - actual_width) / 2.0F);
                    position_x = centered_x;
                    break;
                }
                case model::TextAlignment::kRight: {
                    float text_width = HPDF_Page_TextWidth(page, current_line.c_str()) + (2 * cursor.offset_x());
                    cursor.reset_x();
                    float start_x = cursor.x() + (max_width - text_width);
                    position_x = start_x;
                    break;
                }

                case model::TextAlignment::kLeft:
                default:


                    break;
            }
            // Update the cursor
            float actual_width = HPDF_Page_TextWidth(page, current_line.c_str());
            if (!remaining_text.empty()) {
                //If there is other text return a new line
                cursor.reset_x();
                if (cursor.y()< line_height) {
                    //avoid going out of the page
                    cursor.move_to(position_x, line_height);
                }else {
                    cursor.move_to(position_x, cursor.y() - line_height);
                }
            } else {
                //if text is ended move cursor at the end of the text
                cursor.move_x(position_x+ actual_width + cursor.offset_x());
            }
            remaining_text.erase(0, measured_chars);


            // Remove spaces at the beginning of the line if the cursor is near the left margin
            if (current_line.starts_with(' ') && cursor.x() <= cursor.offset_x() + 1.0F) {
                current_line.erase(0, 1);
            }
            //add text line
            auto line = std::make_shared<model::DocraftText>(*node);
            line->set_text(current_line);
            line->set_width(HPDF_Page_TextWidth(page, current_line.c_str())+node->padding());
            line->set_height(line_height+node->padding());
            set_node_position(line);
            node->add_line(line);
            total_height += line->height();
            total_width = std::max(total_width, actual_width);
            set_node_transform_box(line);
        }
        node->set_height(total_height);
        node->set_width(total_width);
        //For debug
        for (const auto &line: node->lines()) {
            std::print("Text Node ID {} drawn at ({}, {}), size: {}x{}\n", line->id(), line->x(), line->y(),
                       line->width(), line->height());
        }
    }

    bool DocraftLayoutTextHandler::handle(const std::shared_ptr<model::DocraftNode> request) {
        if (auto text_node=std::dynamic_pointer_cast<model::DocraftText>(request)) {
            compute(text_node);
            if (text_node->text().empty()) {
                text_node->set_height(text_node->font_size() * 1.2F); // set minimum height for empty text
            }
            set_node_transform_box(text_node);
            return true;
        }
        return false;
    }
} // docraft

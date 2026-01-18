#include "layout/handler/docraft_layout_section_handler.h"

#include <iostream>

#include "layout/docraft_layout_engine.h"
#include "model/docraft_body.h"
#include "model/docraft_footer.h"
#include "model/docraft_header.h"
#include "model/docraft_layout.h"

namespace docraft::layout::handler {
    void DocraftLayoutSectionHandler::compute(const std::shared_ptr<model::DocraftSection> &node) {
        auto &cursor = context()->cursor();
        cursor.set_offset_x(node->margin_left());
        cursor.set_offset_y(node->margin_top());

        cursor.reset_x();
        if (std::dynamic_pointer_cast<model::DocraftHeader>(node)) {
            cursor.move_y(context()->page_height() - cursor.offset_y());
            context()->set_header(std::dynamic_pointer_cast<model::DocraftHeader>(node));
        } else if (std::dynamic_pointer_cast<model::DocraftBody>(node)) {
            //apply margins
            if (context()->header()) {
                cursor.move_y(context()->page_height() - context()->header()->height() - cursor.offset_y());
            } else {
                throw std::runtime_error("Body section must be placed after header section");
            }
            cursor.move_to(cursor.x() + node->margin_left(), cursor.y());
            context()->set_body(std::dynamic_pointer_cast<model::DocraftBody>(node));
        } else if (std::dynamic_pointer_cast<model::DocraftFooter>(node)) {
            if (context()->body()) {
                if (node->height() == 0.0F) {
                    node->set_height(kFooterDefaultHeight_);
                }
                auto transform_box = context()->body()->transform_box();
                transform_box.bottom_left_.y = transform_box.bottom_left_.y + node->height() + node->margin_bottom();
                context()->body()->set_transform_box(transform_box);
                float footer_top_y = context()->body()->transform_box().bottom_left_.y + (
                                         2 * (context()->body()->margin_top()
                                              + node->margin_bottom()));
                cursor.move_to(node->margin_left(), footer_top_y);
                if (node->height() < context()->body()->height()) {
                    context()->body()->set_height(context()->body()->height() - node->height() - cursor.offset_y());
                }
            } else {
                throw std::runtime_error("Footer section must be placed after body section");
            }

            context()->set_footer(std::dynamic_pointer_cast<model::DocraftFooter>(node));
        }
        float original_width = context()->current_rect_width();
        context()->set_current_rect_width(original_width - (node->margin_left() + node->margin_right()));
    }

    void DocraftLayoutSectionHandler::post_process_layouts(
        const std::shared_ptr<model::DocraftSection> &section) const {
        section->set_x(section->margin_left());
        section->set_width(context()->current_rect_width());
        auto handler_context = context();
        if (std::dynamic_pointer_cast<model::DocraftHeader>(section)) {
            section->set_y(handler_context->page_height());
            section->set_height(section->height() + kHeaderBottomMargin_); //add some space below header
        } else if (std::dynamic_pointer_cast<model::DocraftBody>(section)) {
            if (handler_context->header()) {
                section->set_y(
                    handler_context->page_height() - handler_context->header()->height());
                section->set_height(
                    handler_context->page_height() - handler_context->header()->height() - handler_context->footer()->
                    height()
                    - section->margin_top() - section->margin_bottom());
            } else {
                section->set_y(handler_context->page_height());
                section->set_height(
                    handler_context->page_height());
            }
        } else if (std::dynamic_pointer_cast<model::DocraftFooter>(section)) {
            if (handler_context->body()) {
                section->set_y(
                    handler_context->body()->transform_box().bottom_left_.y - section->margin_bottom());
            } else {
                section->set_y(section->height() + section->margin_bottom());
            }
        }
    }

    bool DocraftLayoutSectionHandler::handle(const std::shared_ptr<model::DocraftNode> request) {
        if (auto section_node = std::dynamic_pointer_cast<model::DocraftSection>(request)) {
            compute(section_node);
            const float section_width = context()->page_width();
            //compute children layout
            float total_height = section_node->height();
            float total_width = section_node->width();
            for (const auto &child: section_node->children()) {
                docraft::layout::DocraftLayoutEngine::layout(child, context());
                total_height += child->height();
                total_width = std::max(total_width, child->width());
                context()->set_current_rect_width(section_width);
                float current_y = context()->cursor().y();
                //save current y position, it will be modified in the layout computation
                context()->cursor().move_y(current_y); //restore y position to the current position
            }
            section_node->set_height(total_height);
            section_node->set_width(total_width);
            // For debug
            std::cout << "Section Node ID " << section_node->id() << " computed size: "
                    << section_node->width() << "x" << section_node->height() << std::endl;
            set_node_transform_box(section_node);

            return true;
        }
        return false;
    }
} // docraft

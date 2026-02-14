#include "layout/docraft_layout_engine.h"

#include <algorithm>

#include "layout/handler/docraft_basic_layout_handler.h"
#include "layout/handler/docraft_layout_blank_line.h"
#include "layout/handler/docraft_layout_handler.h"
#include "layout/handler/docraft_layout_list_handler.h"
#include "layout/handler/docraft_layout_table_handler.h"
#include "layout/handler/docraft_layout_text_handler.h"

#include "model/docraft_header.h"
#include "model/docraft_body.h"
#include "model/docraft_footer.h"
#include "model/docraft_list.h"
#include "utils/docraft_logger.h"

namespace docraft::layout {
    DocraftLayoutEngine::DocraftLayoutEngine(const std::shared_ptr<DocraftDocumentContext> &context, const bool reset_cursor)
        : context_(context) {
        configure_handlers(context);
        if (reset_cursor && context) {
            context->cursor().move_to(0, context->page_height());
        }
    }

    void DocraftLayoutEngine::
    configure_handlers(const std::shared_ptr<DocraftDocumentContext> &context) {
        handlers_.emplace_back(std::make_unique<handler::DocraftLayoutTextHandler>(context));
        handlers_.emplace_back(std::make_unique<handler::DocraftLayoutHandler>(context));
        handlers_.emplace_back(std::make_unique<handler::DocraftLayoutTableHandler>(context));
        handlers_.emplace_back(std::make_unique<handler::DocraftLayoutBlankLine>(context));
        handlers_.emplace_back(std::make_unique<handler::DocraftLayoutListHandler>(context));
        handlers_.emplace_back(std::make_unique<handler::DocraftBasicLayoutHandler>(context));
    }

    const std::shared_ptr<DocraftDocumentContext> &DocraftLayoutEngine::context() const {
        return context_;
    }

    model::DocraftTransform DocraftLayoutEngine::compute_max_rect(const std::vector<model::DocraftTransform> &boxes) {
        if (boxes.empty()) {
            return model::DocraftTransform{};
        }

        float min_x = boxes[0].anchors().top_left.x;
        float max_x = boxes[0].anchors().top_right.x;
        float min_y = boxes[0].anchors().bottom_left.y;
        float max_y = boxes[0].anchors().top_left.y;

        for (const auto &box: boxes) {
            min_x = std::min(min_x, box.anchors().top_left.x);
            max_x = std::max(max_x, box.anchors().top_right.x);
            min_y = std::min(min_y, box.anchors().bottom_left.y);
            max_y = std::max(max_y, box.anchors().top_left.y);
        }

        float width = max_x - min_x;
        float height = max_y - min_y;

        return model::DocraftTransform({.x = min_x, .y = max_y}, width, height);
    }

    bool DocraftLayoutEngine::compute_node(const std::shared_ptr<model::DocraftNode> &node,
                                           model::DocraftTransform *box,
                                           DocraftCursor& cursor) const {
        for (const auto &handler: handlers_) {
            if (handler->handle(node, box, cursor)) {
                return true;
            }
        }
        return false;
    }


    model::DocraftTransform DocraftLayoutEngine::compute_layout(const std::shared_ptr<model::DocraftNode> &node) {
        auto& cursor = context()->cursor();
        return compute_layout(node, cursor);
    }

    model::DocraftTransform DocraftLayoutEngine::compute_layout(const std::shared_ptr<model::DocraftNode> &node,
                                                                DocraftCursor& cursor) {
        std::vector<model::DocraftTransform> child_boxes;
        float max_width = context()->available_space();

        if (auto section_node = std::dynamic_pointer_cast<model::DocraftSection>(node)) {
            cursor.move_to(section_node->margin_left(), cursor.y());// Move cursor to the left margin of the section
            max_width = max_width - section_node->margin_right();//compute the width from left margin to right margin
            context()->set_current_rect_width(max_width);
        }        if (std::dynamic_pointer_cast<model::DocraftLayout>(node)) {
            //Move the cursor direction based on layout orientation to layout children correctly
            auto layout_node = std::dynamic_pointer_cast<model::DocraftLayout>(node);
            if (layout_node->orientation() == model::LayoutOrientation::kHorizontal) {
                cursor.push_direction(DocraftCursorDirection::kHorizontal);
            } else {
                cursor.push_direction(DocraftCursorDirection::kVertical);
            }
        }
        //Process nodes from here
        if (auto list_node = std::dynamic_pointer_cast<model::DocraftList>(node)) {
            handler::DocraftLayoutListHandler* list_handler = nullptr;
            for (const auto &handler : handlers_) {
                list_handler = dynamic_cast<handler::DocraftLayoutListHandler*>(handler.get());
                if (list_handler) {
                    break;
                }
            }
            if (!list_handler) {
                throw std::runtime_error("DocraftLayoutListHandler not configured");
            }
            DocraftCursor list_cursor = cursor;
            list_handler->compute_children(
                list_node,
                list_cursor,
                child_boxes,
                [this](const std::shared_ptr<model::DocraftNode>& child, DocraftCursor& child_cursor) {
                    return compute_layout(child, child_cursor);
                },
                max_width);
        } else if (std::dynamic_pointer_cast<model::DocraftChildrenContainerNode>(node)) {
            auto container_node = std::dynamic_pointer_cast<model::DocraftChildrenContainerNode>(node);
            if (!container_node->children().empty()) {
                bool no_weigth_assigned = false;
                for (const auto &child: container_node->children()) {
                    if (child->weight() == -1.0F) {
                        no_weigth_assigned = true;
                        break;
                    }
                }
                if (no_weigth_assigned) {
                    // If no weight is assigned to children, distribute weights equally

                    const float equal_weight = 1.0F / static_cast<float>(container_node->children().size());
                    for (const auto &child: container_node->children()) {
                        child->set_weight(equal_weight);
                    }
                }
            }
            const float saved_available_space = context()->available_space();
            const bool is_horizontal = (cursor.direction() == DocraftCursorDirection::kHorizontal);
            const std::size_t child_count = container_node->children().size();
            float available_width_for_children = max_width;
            if (is_horizontal && child_count > 1) {
                // Reserve fixed gaps between columns, then distribute the remaining width by weight.
                const float total_spacing = kHorizontalSpacing_ * static_cast<float>(child_count - 1);
                available_width_for_children = std::max(0.0F, max_width - total_spacing);
            }
            for (const auto &child: container_node->children()) {
                if (is_horizontal) {
                    context()->set_current_rect_width(available_width_for_children * child->weight());
                } else {
                    context()->set_current_rect_width(max_width);
                }
                auto child_box = compute_layout(child, cursor);
                child_boxes.emplace_back(child_box);
            }
            context()->set_current_rect_width(saved_available_space);
        }

        auto max_rect = compute_max_rect(child_boxes);

        if (!compute_node(node, &max_rect, cursor)) {
            throw std::runtime_error("compute node failed");
        }
        node->set_position(max_rect.position());
        node->set_width(max_rect.width());
        node->set_height(max_rect.height());
        if (cursor.direction() == DocraftCursorDirection::kHorizontal) {
            // Advance cursor to the next column start, keeping a fixed horizontal gap.
            cursor.move_to(max_rect.anchors().top_right.x + kHorizontalSpacing_, max_rect.anchors().top_right.y);
        } else {
            // Advance cursor down with a fixed vertical gap or per-node padding (whichever is larger).
            const float spacing = std::max(kVerticalSpacing_, node->padding());
            float next_y = max_rect.anchors().bottom_left.y - spacing;
            if (next_y < 0.0F) {
                next_y = 0.0F;
            }
            cursor.move_to(max_rect.anchors().bottom_left.x, next_y);
        }
        return max_rect;
    }

    float DocraftLayoutEngine::compute_width(const std::shared_ptr<model::DocraftSection> &node) const {
        float margin_left = node->margin_left();
        float margin_right = node->margin_right();
        return context()->page_width() - (margin_left + margin_right);
    }

    void DocraftLayoutEngine::compute_document_layout(const std::vector<std::shared_ptr<model::DocraftNode>> &nodes) {
        //Split sections
        std::shared_ptr<model::DocraftHeader> header=nullptr;
        std::shared_ptr<model::DocraftBody> body=nullptr;
        std::shared_ptr<model::DocraftFooter> footer=nullptr;
        for (const auto& node : nodes) {
            if (const auto header_node = std::dynamic_pointer_cast<model::DocraftHeader>(node)) {
                header = header_node;
            } else if (const auto body_node = std::dynamic_pointer_cast<model::DocraftBody>(node)) {
                body = body_node;
            } else if (const auto footer_node = std::dynamic_pointer_cast<model::DocraftFooter>(node)) {
                footer = footer_node;
            }
        }
        if (body==nullptr) {
            throw std::runtime_error("Document must have a body section");
        }
        constexpr float kLineHeightOffset = 12.0F;
        //Layout header
        if (header) {
            context()->cursor().move_to(0, context()->page_height());
            (void)compute_layout(header, context()->cursor());
            header->set_position({.x = header->margin_left(), .y = context()->page_height()});
            header->set_width(compute_width(header));
            header->set_height(context()->page_height()*kHeaderHeightRatio_);
        }
        //Layout body
        if (body) {
            float body_start_y = context()->page_height();
            if (header) {
                body_start_y = header->anchors().bottom_left.y;
            }
            context()->cursor().move_to(0, body_start_y - kLineHeightOffset);
            compute_layout(body, context()->cursor());
            body->set_position({.x = body->margin_left(), .y = body_start_y});
            body->set_width(compute_width(body));
            body->set_height(context()->page_height()*kBodyHeightRatio_);
        }
        //Layout footer
        if  (footer) {
            float footer_start_y = 0.0F;
            if (body) {
                footer_start_y = body->anchors().bottom_left.y;
            }
            context()->cursor().move_to(0, footer_start_y - kLineHeightOffset);
            compute_layout(footer, context()->cursor());
            footer->set_position({.x = footer->margin_left(), .y = footer_start_y});
            footer->set_width(compute_width(footer));
            footer->set_height(context()->page_height()*kFooterHeightRatio_);
        }
    }
} // docraft

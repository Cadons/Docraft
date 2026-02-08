#include "layout/docraft_layout_engine.h"

#include <algorithm>

#include "layout/handler/docraft_basic_layout_handler.h"
#include "layout/handler/docraft_layout_blank_line.h"
#include "layout/handler/docraft_layout_handler.h"
#include "layout/handler/docraft_layout_table_handler.h"
#include "layout/handler/docraft_layout_text_handler.h"

#include "model/docraft_header.h"
#include "model/docraft_body.h"
#include "model/docraft_footer.h"
#include "utils/docraft_logger.h"

namespace docraft::layout {
    DocraftLayoutEngine::DocraftLayoutEngine(const std::shared_ptr<DocraftDocumentContext> &context) : context_(context) {
        configure_handlers(context);
        context->cursor().move_to(0, context->page_height());
    }

    void DocraftLayoutEngine::
    configure_handlers(const std::shared_ptr<DocraftDocumentContext> &context) {
        handlers_.emplace_back(std::make_unique<handler::DocraftLayoutTextHandler>(context));
        handlers_.emplace_back(std::make_unique<handler::DocraftLayoutHandler>(context));
        handlers_.emplace_back(std::make_unique<handler::DocraftLayoutTableHandler>(context));
        handlers_.emplace_back(std::make_unique<handler::DocraftLayoutBlankLine>(context));
        handlers_.emplace_back(std::make_unique<handler::DocraftBasicLayoutHandler>(context));
    }

    const std::shared_ptr<DocraftDocumentContext> &DocraftLayoutEngine::context() const {
        return context_;
    }

    void DocraftLayoutEngine::layout(const std::shared_ptr<model::DocraftNode> &node,
                                     const std::shared_ptr<DocraftDocumentContext> &context) {
        throw std::runtime_error("DocraftLayoutEngine deprecated");
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
                                           model::DocraftTransform *box) const {
        for (const auto &handler: handlers_) {
            if (handler->handle(node, box)) {
                return true;
            }
        }
        return false;
    }

    void DocraftLayoutEngine::post_process(const std::shared_ptr<model::DocraftNode> &node, const model::DocraftTransform &docraft_transform) {
        //if layout post processing to adapt width of the childern to the layout width
        if (std::dynamic_pointer_cast<model::DocraftLayout>(node)) {
            auto layout_node = std::dynamic_pointer_cast<model::DocraftLayout>(node);
            if (layout_node->orientation() == model::LayoutOrientation::kHorizontal) {
                //this post processing is necessary when a layout has other layout as child, in this case the child layout will compute its width based on the available space, but then the parent layout will adapt the child width to fit the layout width
                for (const auto &child: layout_node->children()) {
                    if (std::dynamic_pointer_cast<model::DocraftLayout>(child)) {
                        auto child_layout = std::dynamic_pointer_cast<model::DocraftLayout>(child);
                        for (const auto &child_box: child_layout->children()) {
                            LOG_DEBUG("Post processing layout: adapting child box width to child layout width");
                            if (child_box->width() > child->width()) {
                                //available space is equal to child width
                                context()->cursor().move_to(child_box->position().x, child_box->position().y);
                                context()->set_current_rect_width(child->width()- child_box->position().x);//compute layout again to adapt the child box width to the child layout width
                                auto child_box_layout = compute_layout(child_box);
                                child_box->set_width(child_box_layout.width());
                                child_box->set_height(child_box_layout.height());
                            }
                        }
                    }
                }
            }
        }
    }

    model::DocraftTransform DocraftLayoutEngine::compute_layout(const std::shared_ptr<model::DocraftNode> &node) {
        std::vector<model::DocraftTransform> child_boxes;
        auto &cursor = context()->cursor();
        float max_width = context()->page_width() - cursor.x();

        if (std::dynamic_pointer_cast<model::DocraftSection>(node)) {
            auto section_node = std::dynamic_pointer_cast<model::DocraftSection>(node);
            max_width = context()->available_space() - (section_node->margin_right())-(section_node->margin_left());
            context()->set_current_rect_width(max_width);
        }
        if (std::dynamic_pointer_cast<model::DocraftLayout>(node)) {
            //Move the cursor direction based on layout orientation to layout children correctly
            auto layout_node = std::dynamic_pointer_cast<model::DocraftLayout>(node);
            if (layout_node->orientation() == model::LayoutOrientation::kHorizontal) {
                cursor.push_direction(DocraftCursorDirection::kHorizontal);
            } else {
                cursor.push_direction(DocraftCursorDirection::kVertical);
            }
        }
        //Process nodes from here
        if (std::dynamic_pointer_cast<model::DocraftChildrenContainerNode>(node)) {
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
            for (const auto &child: container_node->children()) {
                float last_child_width = child_boxes.empty() ? 0.0F : child_boxes.back().width();
                if (child->width() == 0.0F) {
                    if (cursor.direction()==DocraftCursorDirection::kHorizontal) {
                        child->set_width(std::max(0.0F, (max_width) - last_child_width));
                    }else {
                        child->set_width(max_width);
                    }
                }
                auto child_box = compute_layout(child);
                child_boxes.emplace_back(child_box);
                child->set_position({.x = child_box.position().x, .y = child_box.position().y});
                if (child->anchors().top_right.x>max_width) {
                    child->set_width(max_width - child->anchors().top_left.x);
                }else {
                    child->set_width(child_box.width());
                }
                child->set_height(child_box.height());
            }
        }

        auto max_rect = compute_max_rect(child_boxes);

        if (!compute_node(node, &max_rect)) {
            throw std::runtime_error("compute node failed");
        }
        //Post process layout if necessary
        post_process(node, max_rect);
        if (cursor.direction() == DocraftCursorDirection::kHorizontal) {
            cursor.move_to(max_rect.anchors().top_right.x, max_rect.anchors().top_right.y);
        } else {
            if (max_rect.anchors().bottom_left.y>0)
                cursor.move_to(max_rect.anchors().bottom_left.x, max_rect.anchors().bottom_left.y-1.0F);
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
            context()->cursor().move_to(header->margin_left(), context()->page_height());
            (void)compute_layout(header);
            header->set_position({.x=header->margin_left(), .y=context()->page_height()});
            header->set_width(compute_width(header));
            header->set_height(context()->page_height()*kHeaderHeightRatio_);
        }
        //Layout body
        if (body) {
            float body_start_y = context()->page_height();
            if (header) {
                body_start_y = header->anchors().bottom_left.y;
            }
            //margins for body content
            context()->cursor().move_to(body->margin_left(), body_start_y-kLineHeightOffset);
            compute_layout(body);
            body->set_position({.x=body->margin_left(), .y=body_start_y});
            body->set_width(compute_width(body));
            body->set_height(context()->page_height()*kBodyHeightRatio_);
        }
        //Layout footer
        if  (footer) {
            float footer_start_y = 0.0F;
            if (body) {
                footer_start_y = body->anchors().bottom_left.y;
            }
            context()->cursor().move_to(footer->margin_left(), footer_start_y-kLineHeightOffset);
            compute_layout(footer);
            footer->set_position({.x=footer->margin_left(), .y=footer_start_y});
            footer->set_width(compute_width(footer));
            footer->set_height(context()->page_height()*kFooterHeightRatio_);
        }
    }
} // docraft

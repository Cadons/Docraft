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

namespace docraft::layout {
    DocraftLayoutEngine::DocraftLayoutEngine(const std::shared_ptr<DocraftPDFContext> &context) : context_(context) {
        configure_handlers(context);
        context->cursor().move_to(0, context->page_height());
    }

    void DocraftLayoutEngine::
    configure_handlers(const std::shared_ptr<DocraftPDFContext> &context) {
        handlers_.emplace_back(std::make_unique<handler::DocraftLayoutTextHandler>(context));
        handlers_.emplace_back(std::make_unique<handler::DocraftLayoutHandler>(context));
        handlers_.emplace_back(std::make_unique<handler::DocraftLayoutTableHandler>(context));
        handlers_.emplace_back(std::make_unique<handler::DocraftLayoutBlankLine>(context));
        handlers_.emplace_back(std::make_unique<handler::DocraftBasicLayoutHandler>(context));
    }

    const std::shared_ptr<DocraftPDFContext> &DocraftLayoutEngine::context() const {
        return context_;
    }

    void DocraftLayoutEngine::layout(const std::shared_ptr<model::DocraftNode> &node,
                                     const std::shared_ptr<DocraftPDFContext> &context) {
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

    model::DocraftTransform DocraftLayoutEngine::compute_layout(const std::shared_ptr<model::DocraftNode> &node) {
        std::vector<model::DocraftTransform> child_boxes;
        auto &cursor = context()->cursor();
        const float max_width = context()->page_width() - cursor.x();
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
            for (const auto &child: container_node->children()) {
                float last_child_width = child_boxes.empty() ? 0.0F : child_boxes.back().width();
                if (child->width() == 0.0F) {
                    child->set_width(max_width - last_child_width);
                }
                auto child_box = compute_layout(child);
                child_boxes.emplace_back(child_box);
                child->set_position({.x = child_box.position().x, .y = child_box.position().y});
                child->set_width(child_box.width());
                child->set_height(child_box.height());
            }
        }

        auto max_rect = compute_max_rect(child_boxes);

        if (!compute_node(node, &max_rect)) {
            throw std::runtime_error("compute node failed");
        }
        if (cursor.direction() == DocraftCursorDirection::kHorizontal) {
            cursor.move_to(max_rect.anchors().top_right.x, max_rect.anchors().top_right.y);
        } else {
            cursor.move_to(max_rect.anchors().bottom_left.x, max_rect.anchors().bottom_left.y);
        }
        return max_rect;
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
        const float kLineHeightOffset_ = 12.0F;
        //Layout header
        if (header) {
            context()->cursor().move_to(0, context()->page_height());
            (void)compute_layout(header);
            header->set_position({.x=0, .y=context()->page_height()});
            header->set_width(context()->page_width());
            header->set_height(context()->page_height()*kHeaderHeightRatio_);
        }
        //Layout body
        if (body) {
            float body_start_y = context()->page_height();
            if (header) {
                body_start_y = header->anchors().bottom_left.y;
            }
            context()->cursor().move_to(0, body_start_y-kLineHeightOffset_);
            compute_layout(body);
            body->set_position({.x=0, .y=body_start_y});
            body->set_width(context()->page_width());
            body->set_height(context()->page_height()*kBodyHeightRatio_);
        }
        //Layout footer
        if  (footer) {
            float footer_start_y = 0.0F;
            if (body) {
                footer_start_y = body->anchors().bottom_left.y;
            }
            context()->cursor().move_to(0, footer_start_y-kLineHeightOffset_);
            compute_layout(footer);
            footer->set_position({.x=0, .y=footer_start_y});
            footer->set_width(context()->page_width());
            footer->set_height(context()->page_height()*kFooterHeightRatio_);
        }
    }
} // docraft

#include "layout/handler/docraft_basic_layout_handler.h"

#include <print>

#include "layout/docraft_layout_engine.h"
#include "model/docraft_layout.h"

namespace docraft::layout::handler {
    void DocraftBasicLayoutHandler::compute(const std::shared_ptr<model::DocraftNode> &node) {
        set_node_position(node);
        if (node->width() == 0) {
            node->set_width(context()->current_rect_width()-context()->cursor().offset_x());
        }
        //for Debug
        std::print("Basic Layout Handler: Node ID {} positioned at ({}, {}), size: {}x{}\n",
                   node->id(), node->x(), node->y(), node->width(), node->height());
    }

    bool DocraftBasicLayoutHandler::handle(const std::shared_ptr<model::DocraftNode> request) {
        compute(request);
        set_node_transform_box(request);
        if (auto *container=dynamic_cast<model::DocraftChildrenContainerNode*>(request.get())) {
            context()->cursor().move_x(request->transform_box().top_left_.x);
            context()->cursor().move_y(request->transform_box().top_left_.y);
            context()->set_current_rect_width(request->width());
            for (const auto &child: container->children()) {
                docraft::layout::DocraftLayoutEngine::layout(child, context());
                std::print("Child Node ID {} drawn at ({}, {}), size: {}x{}\n", child->id(), child->x(), child->y(),
                           child->width(), child->height());
            }
        }
        return true;
    }
} // docraft
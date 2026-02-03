#include "layout/handler/docraft_basic_layout_handler.h"

#include <print>

#include "layout/docraft_layout_engine.h"
#include "model/docraft_layout.h"

namespace docraft::layout::handler {
    void DocraftBasicLayoutHandler::compute(const std::shared_ptr<model::DocraftNode> &node,
                                            model::DocraftTransform *box) {
        if (box == nullptr) {
            throw std::invalid_argument("box is null");
        }
        if (context()->available_space()<node->width()) {
            node->set_width(context()->available_space());
        }
        box->set_width(node->width());
        box->set_height(node->height());
        box->set_position({.x=context_->cursor().x(), .y=context()->cursor().y()});
    }

    bool DocraftBasicLayoutHandler::handle(const std::shared_ptr<model::DocraftNode> &request,
                                           model::DocraftTransform *result) {
        compute(request, result);
        return true;
    }
} // docraft

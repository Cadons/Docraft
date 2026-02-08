#include "layout/handler/docraft_basic_layout_handler.h"

#include <print>

#include "layout/docraft_layout_engine.h"
#include "model/docraft_layout.h"

namespace docraft::layout::handler {
    void DocraftBasicLayoutHandler::compute(const std::shared_ptr<model::DocraftNode> &node,
                                            model::DocraftTransform *box,
                                            DocraftCursor& cursor) {
        if (box == nullptr) {
            throw std::invalid_argument("box is null");
        }

        box->set_position({.x=cursor.x(), .y=cursor.y()});

        if (context()->available_space()<node->width()||node->width()==0) {
            //put the node in the available space
            box->set_width(context()->available_space());
        }else {
            box->set_width(node->width());
        }
        box->set_height(node->height());
    }

    bool DocraftBasicLayoutHandler::handle(const std::shared_ptr<model::DocraftNode> &request,
                                           model::DocraftTransform *result,
                                           DocraftCursor& cursor) {
        compute(request, result, cursor);
        return true;
    }
} // docraft

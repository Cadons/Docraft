#include "layout/handler/docraft_basic_layout_handler.h"

#include <print>

#include "layout/docraft_layout_engine.h"
#include "model/docraft_layout.h"
#include "model/docraft_rectangle.h"

namespace docraft::layout::handler {
    void DocraftBasicLayoutHandler::compute(const std::shared_ptr<model::DocraftNode> &node,
                                            model::DocraftTransform *box,
                                            DocraftCursor& cursor) {
        if (box == nullptr) {
            throw std::invalid_argument("box is null");
        }

        if (node->position_mode()==model::DocraftPositionType::kBlock) {
            box->set_position({.x=cursor.x(), .y=cursor.y()});
        }else {
            box->set_position({.x=node->position().x, .y=node->position().y});
        }

        const bool is_rectangle = static_cast<bool>(std::dynamic_pointer_cast<model::DocraftRectangle>(node));
        const float child_width = box->width();
        const float child_height = box->height();

        if (node->position_mode() == model::DocraftPositionType::kBlock) {
            if (node->width() > 0.0F) {
                box->set_width(node->width());
            } else if (node->auto_fill_width()) {
                box->set_width(std::max(context()->available_space(), child_width));
            } else if (is_rectangle) {
                box->set_width(child_width);
            } else if (context()->available_space() < node->width() || node->width() == 0.0F) {
                box->set_width(context()->available_space());
            } else {
                box->set_width(node->width());
            }
        } else {
            box->set_width(node->width());
        }

        if (node->height() > 0.0F) {
            box->set_height(node->height());
        } else if (is_rectangle) {
            box->set_height(child_height);
        } else {
            box->set_height(node->height());
        }
    }

    bool DocraftBasicLayoutHandler::handle(const std::shared_ptr<model::DocraftNode> &request,
                                           model::DocraftTransform *result,
                                           DocraftCursor& cursor) {
        compute(request, result, cursor);
        return true;
    }
} // docraft

#include "docraft/layout/handler/docraft_layout_handler.h"
#include <print>

#include "docraft/layout/docraft_layout_engine.h"
#define PADDING 20.0F

namespace docraft::layout::handler {
    void DocraftLayoutHandler::compute(const std::shared_ptr<model::DocraftLayout> &node,
                                       model::DocraftTransform *box,
                                       DocraftCursor& cursor) {
        if (box == nullptr) {
            throw std::invalid_argument("box is null");
        }
        // If the layout has a weight, the parent already scoped available_space to that share.
        if (node->weight()!=-1.0F) {
            node->set_width(context()->available_space());
            box->set_width(node->width());
        }
        cursor.pop_direction(); //remove layout direction
    }

    bool DocraftLayoutHandler::handle(const std::shared_ptr<model::DocraftNode> &request,
                                      model::DocraftTransform *result,
                                      DocraftCursor& cursor) {
        if (auto layout_node = std::dynamic_pointer_cast<model::DocraftLayout>(request)) {
            compute(layout_node, result, cursor);
            return true;
        }
        return false;
    }
} // docraft

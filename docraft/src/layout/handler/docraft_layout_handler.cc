#include "layout/handler/docraft_layout_handler.h"
#include <print>
#include "layout/docraft_layout_engine.h"
#define PADDING 20.0F

namespace docraft::layout::handler {
    void DocraftLayoutHandler::compute(const std::shared_ptr<model::DocraftLayout> &node, model::DocraftTransform* box) {
        if (box == nullptr) {
            throw std::invalid_argument("box is null");
        }
        context()->cursor().pop_direction();//remove layout direction
    }

    bool DocraftLayoutHandler::handle(const std::shared_ptr<model::DocraftNode>& request,
                                      model::DocraftTransform * result) {
        if (auto layout_node = std::dynamic_pointer_cast<model::DocraftLayout>(request)) {
            compute(layout_node, result);
            return true;
        }
        return false;
    }
} // docraft

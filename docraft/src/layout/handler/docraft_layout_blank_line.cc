#include "layout/handler/docraft_layout_blank_line.h"

namespace docraft::layout::handler {
    void DocraftLayoutBlankLine::compute(const std::shared_ptr<model::DocraftBlankLine> &node, model::DocraftTransform* box) {
        if (box == nullptr) {
            throw std::invalid_argument("box is null");
        }
        node->set_weight(1.0F); //blank line takes full width
        box->set_height(1.0F); //default height for blank line
        box->set_position({.x=context()->cursor().x(), .y=context()->cursor().y()});

    }

    bool DocraftLayoutBlankLine::handle(const std::shared_ptr<model::DocraftNode> &request,
                                        model::DocraftTransform * result) {
        if (auto blank_line_node = std::dynamic_pointer_cast<model::DocraftBlankLine>(request)) {
            compute(blank_line_node, result);
            return true;
        }
        return false;
    }
} // docraft